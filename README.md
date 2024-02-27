
# How to run the code
1. Firstly run the mosquitto server on your machine if it is not already running
2. First compile the code using the terminal by following the steps below
3. Then execute the code
# Compiling the code
 
## Compiling the publisher
g++ -o mqtt_json_config_publisher mqtt_json_config_publisher.cpp -lmosquitto -I./rapidjson -lconfig++

## Compiling the subscriber
g++ -o mqtt_subscriber mqtt_subscriber.cpp -lmosquitto

# Executing the code
**Execute the subscriber and publisher in seperate terminal.**

## 1.Executing the subscriber
./mqtt_subscriber

## 2.Executing the publisher
./mqtt_json_config_publisher


 publisher read the data.json file and push the data on the topic ( test ) on mosquito server.
