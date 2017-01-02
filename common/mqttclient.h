#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//const char* mqtt_server = "broker.mqtt-dashboard.com";
IPAddress mqtt_server(192, 168, 1, 212); // mosquitto address

//long lastMsg = 0;
//long lastLightMsg = 0;
//char msg[50];
//int value = 0;





class MQTTClient
{
public:
    MQTTClient():
        m_client(m_espClient)
    {
        m_client.setServer(mqtt_server, 1883);
    }

    void subscribe(const char *pTopic, MQTT_CALLBACK_SIGNATURE)
    {
        m_client.setCallback(callback);
        m_sSubscribeTopic = pTopic;
        Serial.printf("Subscribed to %s (%d)\n", m_sSubscribeTopic.c_str(), m_sSubscribeTopic.length());
    }

    void loop()
    {
        if (m_client.connected())
        {
            m_client.loop();
        }
        else
        {
            if (m_sSubscribeTopic.length() > 0)
            {
                Serial.print("Attempting MQTT connection...");
                if (m_client.connect("ESP8266Client3", "test", "test")) {
                    Serial.println("Connected to mqtt broker");
                    // ... and resubscribe
                    m_client.subscribe(m_sSubscribeTopic.c_str());
                }
                else
                {
                    Serial.print("failed, rc=");
                    Serial.print(m_client.state());
                }
            }
        }
    }

private:
    String m_sSubscribeTopic;
    WiFiClient m_espClient;
    PubSubClient m_client;
};


#endif // MQTTCLIENT_H
