# Plant Salvation
## IoT Plant Monitoring Device
### Description
 This project focused on designing an end-to-end plant monitoring system that collects environmental data, sends it to the could, stores in a PostGreSQL database, and visualizes trends over time. The system monitors soil moisture, light levels, temperature, and humidity (with temperature and humidity provided by a single sensor). Data flows from our embedded hardware to a cloud backend (AWS) and is ultimately displayed through a python flask web dashboard.

## System Architecture
Sensors --> Ardunio --> ESP32 (MQTT publishes data) --> AWS IoT Core --> Python script inserts into PostGreSQL database --> Web App pulls intial data from database --> API is called to update website in real time


## How To Use
The most challenging part is going to setting up AWS...

First you are going to want to log into AWS IoT Core and create two new things. One will be for the esp32 MQTT and the other will be for the python script that is subscribed to the AWS topic.
Download the certificates and make sure the thing policies allow the thing to publish and subscribe.
For the python script called subscriber.py you need to update your certificate paths. This can be done by downloading and moving one of your certifiactes to the certificate folder.
You will also need to do the same for the ESP32 MQTT server, the file is call mqttAWSesp32.io. However you will be using the file contents instead of the path. It will look like a bunch of giberish but thats ok. Copy the entire text and paste each certificate into its coresponding location.
Now that we are connected to AWS you need to connect to a local wifi or hotspot. The ssid and password can be updated in mqttAWSesp32.io.
Finaly we need to create a local PostGreSQL database. The SQL code can be found in the PlantSalvation-Server-WebApp folder. Then update your connections to the database in both the subscriber.py & webApp.py.
Now that everything is connected insert the soil moisture sensor into you plants soild and place the other sensors near the plant.
Spin up all of the programs and then watch the data flood in!
Limitations
Local database limits availability for others to implement this project.
AWS IoT Core limits availability for others to implement this project.
Connection issues to wifi
Everything is powered by wires, thus your laptop must be close to the plant to gather readings