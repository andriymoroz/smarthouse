
#include <stdint.h>

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include "nrfmessage.h"
#include "common.h"
#include "sleep.h"

//#define DEBUG_SERIAL_PRINT
#include "debug.h"

#ifdef SENSOR_DHT
#include "DHT.h"
#endif

#ifdef SENSOR_DS
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

// NRF Network
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
RF24Network network(radio);						// Network uses that radio

#ifdef SENSOR_DHT
DHT dht(DHT_DATA_PIN, DHT_TYPE);
#endif

#ifdef SENSOR_DS
OneWire oneWire(DS_DATA_PIN);
DallasTemperature sensors(&oneWire);
DeviceAddress sensorAddress;
#endif

#ifndef NODE_SLEEP
unsigned long last_time_sent;
#endif

//This is for sleep mode. It is not really required, as users could just use the number 0 through 10
typedef enum { wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s } wdt_prescalar_e;

NrfMessageV2 msg;
NrfMessageV2 id_msg(0);

void setup(void)
{
	// debug initialization
	debug_init(9600);

	// network initialization
	SPI.begin();
	radio.begin();
	radio.setPALevel(RF24_PA_MAX);
	network.begin(NRF_CHANNEL_MAIN, NODE_ADDRESS);
	strncpy(id_msg.data.id.name, NODE_NAME, MAX_NODE_NAME_LEN);
	id_msg.data.id.name[MAX_NODE_NAME_LEN] = '\0';
	debug_print("Debug log. Node: '");
	debug_print(id_msg.data.id.name);
	debug_print("'. Address: ");
	debug_println(NODE_ADDRESS);

#ifdef NODE_SLEEP
	network.setup_watchdog(wdt_8s);
#endif

	// sensors initialization
#ifdef SENSOR_DHT
	POWER_UP(DHT_POWER_PIN);
	dht.begin();
#ifdef NODE_SLEEP
	POWER_DOWN(DHT_POWER_PIN);
#endif
#endif

#ifdef SENSOR_DS
	POWER_UP(DS_POWER_PIN);
	sensors.begin();
	sensors.getAddress(sensorAddress, 0);
	sensors.setResolution(sensorAddress, 9);

#ifdef NODE_SLEEP
	POWER_DOWN(DS_POWER_PIN);
#endif
#endif

#ifndef NODE_SLEEP
	last_time_sent = 0;
#endif


}

void send_sensor_data()
{
	{
		RF24NetworkHeader header(0, MsgType_ID);
		if (!network.write(header, &id_msg, sizeof(id_msg)))
		{
			debug_println("Error sending node ID");
			return;
		}
	}

#ifdef SENSOR_DHT
	{
#ifdef NODE_SLEEP
		POWER_UP(DHT_POWER_PIN);
		delay(1000);
#endif
		// warm up
		dht.readHumidity();
		dht.readTemperature();

		float h = dht.readHumidity();
		float t = dht.readTemperature();

#ifdef NODE_SLEEP
		POWER_DOWN(DHT_POWER_PIN);
#endif

		debug_print("Temp: "); debug_print(t); debug_println("*C");
		debug_print("Humidity: "); debug_print(h); debug_println("%");

		msg.data.dht.temperature = t;
		msg.data.dht.humidity = h;

		RF24NetworkHeader header(0, MsgType_TempHum);
		if (!network.write(header, &msg, sizeof(msg)))
		{
			debug_println("Error sending DHT sensor data");
			return;
		}


	}
#endif

#ifdef SENSOR_DS
	{
#ifdef NODE_SLEEP
		POWER_UP(DS_POWER_PIN);
		delay(100);
#endif

		sensors.requestTemperatures();
		float ds_t = sensors.getTempC(sensorAddress);

		debug_print("Temp: "); debug_print(ds_t); debug_println("*C");

		msg.ds.temperature = ds_t;

		RF24NetworkHeader header(0, MsgType_Temp);
		if (!network.write(header, &msg, sizeof(msg)))
		{
			debug_println("Error sending DS sensor data");
			return;
		}

#ifdef NODE_SLEEP
		POWER_DOWN(DS_POWER_PIN);
#endif
	}
#endif

}


void loop(void)
{
#ifdef NODE_SLEEP
	radio.powerUp();
#endif

	network.update();
#ifndef NODE_SLEEP
	unsigned long now = millis();                         // Send a ping to the next node every 'interval' ms
	if (now - last_time_sent < NODE_SEND_PERIOD )
		return;
	last_time_sent = now;
#endif

	send_sensor_data();

#ifdef NODE_SLEEP
	//radio.stopListening();
	radio.powerDown();
	//network.sleepNode(1,0);
	sleep_sleep_sleep();
#endif
}
