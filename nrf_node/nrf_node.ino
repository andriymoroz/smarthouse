
#include <stdint.h>

//#define DEBUG_SERIAL_PRINT

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "nrfmessage.h"
#include "common.h"

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
	radio.setPALevel(RF24_PA_HIGH);
	network.begin(NRF_CHANNEL_MAIN, NODE_ADDRESS);

	// sensors initialization
#ifdef SENSOR_DHT11
	dht.begin();
#endif

#ifdef NODE_SLEEP
#endif

#ifdef SENSOR_DS
	pinMode(DS_POWER_PIN, OUTPUT);
	digitalWrite(DS_POWER_PIN, HIGH);

	sensors.begin();
	sensors.getAddress(sensorAddress, 0);
	sensors.setResolution(sensorAddress, 9);
#endif

}

void send_sensor_data()
{
#ifdef SENSOR_DHT
#ifdef NODE_SLEEP
	pinMode(DHT_POWER_PIN, OUTPUT);
	digitalWrite(DHT_POWER_PIN, HIGH);
#endif

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
	digitalWrite(DHT_POWER_PIN, LOW);
	pinMode(DHT_POWER_PIN, INPUT);
#endif
#endif //dht11

#ifdef SENSOR_DS
#ifdef NODE_SLEEP
	pinMode(DS_POWER_PIN, OUTPUT);
	digitalWrite(DS_POWER_PIN, HIGH);
#endif

	sensors.requestTemperatures();
	float t = sensors.getTempC(sensorAddress);

	debug_print("Temp: "); debug_print(t); debug_println("*C");

	msg.ds.temperature = t;

	RF24NetworkHeader header(0, MsgType_Temp);
	if (!network.write(header, &msg, sizeof(msg)))
	{
		debug_println("Error sending DS sensor data");
		return;
	}

#ifdef NODE_SLEEP
	digitalWrite(DS_POWER_PIN, LOW);
	pinMode(DS_POWER_PIN, INPUT);
#endif
#endif

}


void loop(void){

	network.update();
	send_sensor_data();

	delay(8000);
}

