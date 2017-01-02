#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "credentials.h"

//const char* mqtt_server = "broker.mqtt-dashboard.com";
IPAddress mqtt_server_ip; //((const uint8_t *)MQTT_HOST); //  (192, 168, 1, 212); // mosquitto address

class MQTTClient
{
public:
    MQTTClient(const char *clientName):
      m_client(m_espClient),
      m_sClientName(clientName)
    {
        mqtt_server_ip.fromString(MQTT_HOST);
        m_client.setServer(mqtt_server_ip, 1883);
        loop();
    }

    void subscribe(const char *pTopic, MQTT_CALLBACK_SIGNATURE)
    {
        m_client.setCallback(callback);
        m_sSubscribeTopic = pTopic;
        if (m_client.connected())
        {
            resubscribe();
        }
    }

    boolean publish(const char* topic, const char* payload, boolean retained = false)
    {
        return m_client.publish(topic,(const uint8_t*)payload, strlen(payload), retained);
    }

    void loop()
    {
        if (m_client.connected())
        {
            m_client.loop();
        }
        else
        {
            Serial.print("Attempting MQTT connection...");
            if (m_client.connect(m_sClientName.c_str(), "test", "test"))
            {
                Serial.println("Connected to mqtt broker");
                // ... and resubscribe
                resubscribe();
            }
            else
            {
                Serial.print("failed, rc=");
                Serial.print(m_client.state());
            }
        }
    }

private:
    void resubscribe()
    {
        if (m_sSubscribeTopic.length() > 0)
        {
            if (m_client.subscribe(m_sSubscribeTopic.c_str()))
            {
                Serial.printf("Subscribed to %s (%d)\n", m_sSubscribeTopic.c_str(), m_sSubscribeTopic.length());
            }
        }
    }

    String m_sSubscribeTopic;
    String m_sClientName;
    WiFiClient m_espClient;
    PubSubClient m_client;
};


#endif // MQTTCLIENT_H
