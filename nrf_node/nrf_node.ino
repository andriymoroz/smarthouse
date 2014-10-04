
#include <stdint.h>

//#define DEBUG_SERIAL_PRINT

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include "nrfmessage.h"
#include "common.h"
#include "sleep.h"

#define DEBUG_SERIAL_PRINT
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

//This is for sleep mode. It is not really required, as users could just use the number 0 through 10
typedef enum { wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s } wdt_prescalar_e;

NrfMessage msg;

void setup(void)
{
	// debug initialization
	debug_init(9600);
	debug_print("Debug log. Node:");
	debug_println(NODE_ADDRESS);

	// network initialization
	SPI.begin();
	radio.begin();
	radio.setPALevel(RF24_PA_MAX);
	network.begin(NRF_CHANNEL_MAIN, NODE_ADDRESS);

	network.update();
	strncpy(msg.id.name, NODE_NAME, sizeof(NODE_NAME));

	RF24NetworkHeader header(0, MsgType_ID);
	if (!network.write(header, &msg, sizeof(msg)))
	{
		debug_println("Error sending node ID");
		return;
	}

#ifdef NODE_SLEEP
	network.setup_watchdog(wdt_8s);
#endif

	// sensors initialization
#ifdef SENSOR_DHT11
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

}

void send_sensor_data()
{
#ifdef SENSOR_DHT
	{
#ifdef NODE_SLEEP
		POWER_UP(DHT_POWER_PIN);
		delay(100);
#endif
		// warm up
		dht.readHumidity();
		dht.readTemperature();

		float h = dht.readHumidity();
		float t = dht.readTemperature();

		debug_print("Temp: "); debug_print(t); debug_println("*C");
		debug_print("Humidity: "); debug_print(h); debug_println("%");

		msg.dht.temperature = t;
		msg.dht.humidity = h;

		RF24NetworkHeader header(0, MsgType_TempHum);
		if (!network.write(header, &msg, sizeof(msg)))
		{
			debug_println("Error sending DHT sensor data");
			return;
		}

#ifdef NODE_SLEEP
		POWER_DOWN(DHT_POWER_PIN);
#endif
	}
#endif //dht11

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
	send_sensor_data();

#ifdef NODE_SLEEP
	radio.powerDown();
	//network.sleepNode(1,0);
	sleep_sleep_sleep();
#else
	delay(8000); ggg
#endif
}

