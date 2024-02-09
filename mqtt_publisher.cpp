
#include <iostream>
#include <mosquitto.h>
#include <cstring>
#include <thread>   // Include this for std::this_thread::sleep_for
#include <chrono>   // Include this for std::chrono::seconds


// MQTT broker details
#define MQTT_HOST "localhost"
#define MQTT_PORT 1883
#define MQTT_TOPIC "test"

int main() {
    struct mosquitto *mosq = NULL;
    int rc;

    mosquitto_lib_init();

    // Create a new mosquitto client instance
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        std::cerr << "Error: Out of memory when creating client." << std::endl;
        return 1;
    }

    // Connect to MQTT broker
    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if (rc) {
        std::cerr << "Error: Could not connect to MQTT broker." << std::endl;
        return rc;
    }

    // Loop to publish messages
    while(true) {
        // Publish a message
        const char *payload = "Hello, MQTT!";
        rc = mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(payload), payload, 0, false);
        if (rc) {
            std::cerr << "Error: Could not publish message." << std::endl;
            return rc;
        }

        std::cout << "Message published: " << payload << std::endl;

        // Sleep for a while before publishing the next message
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Clean up
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
