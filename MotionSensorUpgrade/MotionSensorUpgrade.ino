/**
 *  SonoffLights.ino
 *
 *  SonOff alternative firmware
 *
 */

//#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//#include <WiFiUdp.h>
#include <TimeLib.h>
#include <NtpClientLib.h>

#include "credentials.h"
#include "espotaupdate.h"
#include "mqttclient.h"

#define IMAGE_VERSION "0.3"
#define MQTT_MOTION_MSG_TOPIC "room/greenroom/motion/state"
#define MQTT_TEMP_MSG_TOPIC "room/greenroom/weather/temperature"
#define MOTION_PIN 13
#define DS18B20_PIN 5

const char wmVer[] = "VWM_"IMAGE_VERSION;

ESP8266WiFiMulti WiFiMulti;
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

String helloTopic(MQTT_HELLO_TOPIC);
ESPOTAUpdate otaUpdater(&Serial);
MQTTClient *pMQTTClient;
String helloMsg("setup...");
volatile int motionPinState = -1;

void processSyncEvent(NTPSyncEvent_t ntpEvent)
{
    if (ntpEvent)
    {
        Serial.print("Time Sync error: ");
        if (ntpEvent == noResponse)
            Serial.println("NTP server not reachable");
        else if (ntpEvent == invalidAddress)
            Serial.println("Invalid NTP server address");
    }
    else
    {
        sensors.requestTemperatures();
        // hello message
        helloMsg = NTP.getTimeDateString(NTP.getLastNTPSync());
        Serial.printf("Got NTP time: %s\n", helloMsg.c_str());
        // temp message
        String temp(sensors.getTempCByIndex(0));
        Serial.printf("Temp: %s\n", temp.c_str());
        if (pMQTTClient)
        {
            pMQTTClient->publish(helloTopic.c_str(), helloMsg.c_str());
            pMQTTClient->publish(MQTT_TEMP_MSG_TOPIC, temp.c_str());
        }
        Serial.flush();
    }
}

void pinChanged()
{
    motionPinState = digitalRead(MOTION_PIN);
}

boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

void setup()
{
    Serial.begin(9600);
    Serial.println(wmVer);

    pinMode(MOTION_PIN, INPUT);
    sensors.begin();
//    sensors.setResolution(0, 12);
    String imageName(__FILENAME__);
    imageName.replace(".ino", "");

    WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);

    int conn_attempt = 0;
    while (1)
    {
        if((WiFiMulti.run() == WL_CONNECTED))
        {
            Serial.printf("[SETUP] Wi-Fi connected...\n");
            Serial.printf("[SETUP] - MAC address: %s\n", WiFi.macAddress().c_str());
            Serial.printf("[SETUP] - AP name: %s\n", WiFi.SSID().c_str());
            Serial.printf("[SETUP] - IP address: %s\n", WiFi.localIP().toString().c_str());
            Serial.printf("[SETUP] - subnet: %s\n", WiFi.subnetMask().toString().c_str());
            Serial.printf("[SETUP] - gateway: %s\n", WiFi.gatewayIP().toString().c_str());
            Serial.printf("[SETUP] - image name: %s\n", imageName.c_str());
            Serial.printf("[SETUP] - image version: %s\n", IMAGE_VERSION);
            Serial.flush();
            break;
        }
        else
        {
            Serial.printf("[SETUP] Wi-Fi not connected... Attempt: %d\n", conn_attempt);
            Serial.flush();
            conn_attempt++;
            delay(1000);
        }
    }

    Serial.printf("[SETUP] Trying to do OTA update...\n");
    int updStatus = otaUpdater.fwUpdate((imageName + ".bin").c_str(), IMAGE_VERSION);
    switch(updStatus)
    {
    case HTTP_UPDATE_FAILED:
        Serial.printf("Failed\n");
        Serial.printf("%s\n", otaUpdater.getLastErrorString().c_str());
        Serial.flush();
        break;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("No updates\n");
        Serial.println();
        break;

    case HTTP_UPDATE_OK:
        Serial.println("Update OK\n");
        Serial.println();
        break;
    }

    helloTopic = helloTopic + "/" + imageName + "_" + WiFi.macAddress();
    pMQTTClient = new MQTTClient((imageName + "_" + WiFi.macAddress()).c_str());
    if (pMQTTClient->publish(helloTopic.c_str(), helloMsg.c_str()))
        Serial.printf("Published to hello\n");

    NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
            ntpEvent = event;
            syncEventTriggered = true;
    });

    NTP.begin("pool.ntp.org", 2, true);
    NTP.setInterval(20);
    attachInterrupt(MOTION_PIN, pinChanged, CHANGE);
}

int pin = 0;

void loop()
{
    static int i = 0;
    static int last = 0;
    static int lastPinState = LOW;

    if (syncEventTriggered) {
            processSyncEvent(ntpEvent);
            syncEventTriggered = false;
    }

    if (motionPinState != lastPinState)
    {
        delay(5);
        int state = digitalRead(MOTION_PIN);
        if (state != lastPinState)
        {
            lastPinState = state;

            if (state == LOW)
            {
                if (pMQTTClient)
                    pMQTTClient->publish(MQTT_MOTION_MSG_TOPIC, "LOW");
                Serial.println("Motion: LOW");
            }
            else
            {
                if (pMQTTClient)
                    pMQTTClient->publish(MQTT_MOTION_MSG_TOPIC, "HIGH");
                Serial.println("Motion: HIGH");
            }
        }
    }

    if ((millis() - last) > 5100)
    {
        //Serial.println(millis() - last);
        last = millis();
        Serial.print(i); Serial.print(" ");
        Serial.print(NTP.getTimeDateString()); Serial.print(" ");
        Serial.print(NTP.isSummerTime() ? "Summer Time. " : "Winter Time. ");
        Serial.print("WiFi is ");
        Serial.print(WiFi.isConnected() ? "connected" : "not connected"); Serial.print(". ");
        Serial.print("Uptime: ");
        Serial.print(NTP.getUptimeString()); Serial.print(" since ");
        Serial.println(NTP.getTimeDateString(NTP.getFirstSync()).c_str());
        i++;
    }

    pMQTTClient->loop();

    yield();
}
