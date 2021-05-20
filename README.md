# Touch Sensor to UDP Handler for ESP8266 boards

This repo contains basic examples of handling touch sensors and sending out messages via UDP. 
This project was created and tested using an ESP8266 board, but could easily work for any arduino board

## Settings

you will need to modify settings in file:
* `include/wifi_info.h` 

add your wifi information and preferred ports for communication. 

If you wish to receive heartbeat information, uncomment line 5 in `udphandler.h`:

* `//#define LISTENMODE`

This adds a listener on the port defined in `wifi_info.h`
It is currently configured to assume messages are heartbeats, and will print them on the serial console. 


