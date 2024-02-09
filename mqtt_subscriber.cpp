#include <iostream>
#include <mosquitto.h>

// MQTT broker details
#define MQTT_HOST "localhost"
#define MQTT_PORT 1883
#define MQTT_TOPIC "test"

// Callback function for message arrivals
void message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message) {
    if (message->payloadlen) {
        std::cout << "Topic: " << message->topic << ", Message: " << (char *)message->payload << std::endl;
    } else {
        std::cout << "Empty message received!" << std::endl;
    }
}

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

    // Set callback function for message arrivals
    mosquitto_message_callback_set(mosq, message_callback);

    // Connect to MQTT broker
    rc = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60);
    if (rc) {
        std::cerr << "Error: Could not connect to MQTT broker." << std::endl;
        return rc;
    }

    // Subscribe to a topic
    rc = mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0);
    if (rc) {
        std::cerr << "Error: Could not subscribe to topic." << std::endl;
        return rc;
    }

    // Loop forever, processing messages
    rc = mosquitto_loop_forever(mosq, -1, 1);

    // Clean up
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return rc;
}
