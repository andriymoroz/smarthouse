/**
 * httpUpdate.ino
 *
 *  Created on: 27.11.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include "credentials.h"
#include "espotaupdate.h"
#include "mqttclient.h"

#define IMAGE_NAME __FILENAME__
#define IMAGE_VERSION "0.3"
#define MQTT_MSG_TOPIC "room/smallbedroom/mainlight/state"
#define USE_SERIAL Serial
#define ONOFF_PIN 12

const char wmVer[] = "VWM_"IMAGE_VERSION;

ESP8266WiFiMulti WiFiMulti;

ESPOTAUpdate otaUpdater(IMAGE_NAME, IMAGE_VERSION, &USE_SERIAL);
MQTTClient *pMQTTClient;
char lightState = '1';

void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
    char buf[length + 1];

    if (length)
    {
        for (int i = 0; i < length; i++)
        {
            buf[i] = ((char)payload[i]);
        }
        buf[length] = '\0';
        Serial.printf("Message arrived '%s'\n", buf);
        if (buf[0] != lightState)
        {
            digitalWrite(ONOFF_PIN, (buf[0] == '1')? HIGH : LOW);
            pMQTTClient->publish(MQTT_MSG_TOPIC, buf, true);
            lightState = buf[0];
        }
    }
}

void setup()
{
    pinMode(ONOFF_PIN, OUTPUT);
    digitalWrite(ONOFF_PIN, HIGH);

    USE_SERIAL.begin(9600);
    USE_SERIAL.println(wmVer);

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] Image: %s, version %s. Update in %d...\n", IMAGE_NAME, IMAGE_VERSION, t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

    while (1)
    {
        if((WiFiMulti.run() == WL_CONNECTED))
        {
            USE_SERIAL.printf("[SETUP] Trying to do OTA update...\n");
            int updStatus = otaUpdater.fwUpdate();
            switch(updStatus)
            {
            case HTTP_UPDATE_FAILED:
                USE_SERIAL.printf("failed\n");
                USE_SERIAL.printf("%s\n", otaUpdater.getLastErrorString().c_str());
                USE_SERIAL.flush();
                break;
            case HTTP_UPDATE_NO_UPDATES:
                USE_SERIAL.println("no updates\n");
                USE_SERIAL.println();
                break;

            case HTTP_UPDATE_OK:
                USE_SERIAL.println("OK\n");
                USE_SERIAL.println();
                break;
            }

            String clientName(__FILENAME__);
            clientName += "_ESP8266MQTTClient1";
            pMQTTClient = new MQTTClient(clientName.c_str());
            if (pMQTTClient->publish(MQTT_MSG_TOPIC, "1", true))
                Serial.printf("Published '1'\n");
            pMQTTClient->subscribe(MQTT_MSG_TOPIC, mqtt_callback);
            break;
        }
        else
        {
            USE_SERIAL.println("Wi-Fi connection error\n");
            USE_SERIAL.println();
            delay(3000);
        }
    }
}

void loop() {

    //yield();
    //pDotMatrix->loop();
    //USE_SERIAL.printf("MQTT loop...\n");
    pMQTTClient->loop();
    //delay(1000);
}
