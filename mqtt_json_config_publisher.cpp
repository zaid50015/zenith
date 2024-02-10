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

using namespace rapidjson;
using namespace libconfig;

// MQTT broker details
#define MQTT_HOST "localhost"
#define MQTT_PORT 1883
#define MQTT_TOPIC "test"

int main() {
    struct mosquitto *mosq = NULL;
    int rc;

    mosquitto_lib_init();
    Config cfg;

    try {

        cfg.readFile("./config/configfile.cfg");

        const Setting& root = cfg.getRoot();

        std::string host;
        int port;
        std::string topic;
        std::string jsonfile;

        root.lookupValue("host", host);
        root.lookupValue("port", port);
        root.lookupValue("topic", topic);
        root.lookupValue("jsonfile", jsonfile);
 

    // Create a new mosquitto client instance
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        std::cerr << "Error: Out of memory when creating client." << std::endl;
        return 1;
    }

    // Connect to MQTT broker
    //rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    rc = mosquitto_connect(mosq, host.c_str(), port, 60);

    if (rc) {
        std::cerr << "Error: Could not connect to MQTT broker." << std::endl;
        return rc;
    }

    // Loop to publish messages
    while (true) {
        // Read JSON object from file
        //std::ifstream ifs("data.json");
        std::ifstream ifs(jsonfile);
        if (!ifs.is_open()) {
            std::cerr << "Error: Failed to open data.json" << std::endl;
            return 1;
        }

        std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        ifs.close();

        Document document;
        document.Parse(content.c_str());

        if (document.HasParseError()) {
            std::cerr << "Error: Failed to parse JSON." << std::endl;
            return 1;
        }

        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        document.Accept(writer);
        std::string payload = buffer.GetString();

        // Publish the JSON object as a message
        rc = mosquitto_publish(mosq, NULL, MQTT_TOPIC, payload.length(), payload.c_str(), 0, false);
        if (rc) {
            std::cerr << "Error: Could not publish message." << std::endl;
            return rc;
        }

        std::cout << "Message published: " << payload << std::endl;

        // Sleep for a while before publishing the next message
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    } catch (const FileIOException &fioex) {
        std::cerr << "Error reading config file: " << fioex.what() << "Filename :"<<std::endl;
        return 1;
    }
    catch (const ParseException &pex) {
        std::cerr << "Error parsing config file at line " << pex.getLine() << ": " << pex.getError() << std::endl;
        return 1;
    }
    catch (const SettingNotFoundException &nfex) {
        std::cerr << "Error setting not found in config file: " << nfex.getPath() << std::endl;
        return 1;
    }

    // Clean up
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
