#include <iostream>
#include <fstream>
#include <string>
#include <mosquitto.h>
#include <chrono>
#include <thread>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <libconfig.h++>

using namespace std;
using namespace rapidjson;
using namespace libconfig;

// // MQTT broker details
// #define MQTT_HOST "localhost"
// #define MQTT_PORT 1883
// #define MQTT_TOPIC "test"
void changeValuesInJson(Value &value)
{
    if (value.IsObject())
    {

        for (auto &member : value.GetObject())
        {
            changeValuesInJson(member.value);
        }
    }
    else if (value.IsArray())
    {

        for (SizeType i = 0; i < value.Size(); ++i)
        {

            changeValuesInJson(value[i]);
        }
    }
    else if (value.IsString())
    {

        value.SetString("hello");
    }
    else if (value.IsInt())
    {

        value.SetInt(0);
    }
}
int main()
{
    struct mosquitto *mosq = NULL;
    int rc;

    mosquitto_lib_init();
    Config cfg;

    try
    {

        cfg.readFile("./config/configfile.cfg");

        const Setting &root = cfg.getRoot();

        string host;
        int port;
        string topic;
        string jsonfile;

        root.lookupValue("host", host);
        root.lookupValue("port", port);
        root.lookupValue("topic", topic);
        root.lookupValue("jsonfile", jsonfile);

        // Create a new mosquitto client instance
        mosq = mosquitto_new(NULL, true, NULL);
        if (!mosq)
        {
            cerr << "Error: Out of memory when creating client." << endl;
            return 1;
        }

        // Connect to MQTT broker
        // rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
        rc = mosquitto_connect(mosq, host.c_str(), port, 60);

        if (rc)
        {
            cerr << "Error: Could not connect to MQTT broker." << endl;
            return rc;
        }

        // Loop to publish messages
        while (true)
        {
            // Read JSON object from file
            // ifstream ifs("data.json");
            ifstream ifs(jsonfile);
            if (!ifs.is_open())
            {
                cerr << "Error: Failed to open data.json" << endl;
                return 1;
            }

            string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
            ifs.close();

            Document document;
            document.Parse(content.c_str());
            changeValuesInJson(document);

            if (document.HasParseError())
            {
                cerr << "Error: Failed to parse JSON." << endl;
                return 1;
            }

            StringBuffer buffer;
            Writer<StringBuffer> writer(buffer);
            document.Accept(writer);
          
            string payload = buffer.GetString();

            // Publish the JSON object as a message
            rc = mosquitto_publish(mosq, NULL, topic.c_str(), payload.length(), payload.c_str(), 0, false);
            if (rc)
            {
                cerr << "Error: Could not publish message." << endl;
                return rc;
            }

            cout << "Message published: " << payload << endl;

            // Sleep for a while before publishing the next message
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
    catch (const FileIOException &fioex)
    {
        cerr << "Error reading config file: " << fioex.what() << "Filename :" << endl;
        return 1;
    }
    catch (const ParseException &pex)
    {
        cerr << "Error parsing config file at line " << pex.getLine() << ": " << pex.getError() << endl;
        return 1;
    }
    catch (const SettingNotFoundException &nfex)
    {
        cerr << "Error setting not found in config file: " << nfex.getPath() << endl;
        return 1;
    }

    // Clean up
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
