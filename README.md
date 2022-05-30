# MidiFlower
BLE Midi music generation from BioSensor data with Web control board


This firmware has the same features as [MidiFlower](https://github.com/crocsg/MidiFlower). But we add a web gui for music generation configuration.

This software was made after an hackathon with [Climate Change Lab](https://climatechangelab.org/) and [La Fabrique Janze](https://lafabrique.rafcom.bzh/)
The purpose of the hackathon was the creation of small artistic installation based on music génération from bio data sensor
We used the sensor made by [Sam Cusumano](https://github.com/electricityforprogress/MIDIsprout)

If you want to build the sensor with an ESP32 board, the [documentation is available here](https://wikifab.org/wiki/Capteur_BioData_pour_ESP32)

More information about the event [Sylvestre Orchestre version de poche](https://climatechangelab.org/2021/12/13/sylvestre-orchestre-version-de-poche-retour-sur-le-hackathon-de-decembre-2021/)




# Configuration feature
We use the ESP32 wifi to enable a Wifi  acces point named **MidiFlower_XXXXXXXX**.
**The default password of the access point is 12345678** you can chage it in wifiap.cpp
Use your mobile phone to connect to the acces point
The configuration board can be reach on **http://192.168.163.1**. On Android phone you can access the configuration by using "Router Configuration" button
in wifi channel parameters.

with the web gui, you can configure :
- Base BPM of generated music
- Adapt a channel BPM from the base BPM (OFF x1 x2 x4)
- Loop size from 8 notes to 128
- Adapt channel filling (how much notes are stored in loop 25%, 50%, 75%, 100%)
- Choose a scale (Chromatic, Major, Pentatonic, Blues...)
- Choose root scale note

All options are **saved in flash**, so you get the same options on restart


# Dependencies
You will need the folowing library to build the firmware with arduino ide (use library manager to install them)
see [documentation available here](https://wikifab.org/wiki/Capteur_BioData_pour_ESP32) to install arduino ide installation for ESP32 Boards

- BLE-Midi
- ESP32-Ble-Midi
- ArduinoJson

# SPIFFS  Schema
in Arduino ide select **"No OTA (2MB App / 2MB SPIFFS)" in partition scheme**

# Board configuration
You can configure the pin used for the flower sensor in board.h
There is already some configuration for basic board
- Velleman ESP32
- LOLIN 32D

Simply change the line 
**#define BOARD_CONFIG    BOARD_VELLEMAN_ESP32**
to get your favorite board working

# Roadmap
- Build a better web GUI



