
#include <stdint.h>

//#define DEBUG_SERIAL_PRINT

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "nrfmessage.h"
#include "common.h"
#include "debug.h"
#ifdef SENSOR_DHT
#include "DHT.h"
#endif

// NRF Network
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
RF24Network network(radio);						// Network uses that radio

#ifdef SENSOR_DHT
DHT dht(DHT_DATA_PIN, DHT_TYPE);
#endif



NrfMessage msg;

void setup(void)
{
	// debug initialization
	debug_init(9600);
	debug_println("");

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

}

void send_sensor_data()
{
#ifdef SENSOR_DHT
#ifdef NODE_SLEEP
	pinMode(DHT_POWER_PIN, OUTPUT);
	digitalWrite(DHT_POWER_PIN, HIGH);
#endif

	float h = dht.readHumidity();
	// Read temperature as Celsius

	float t = dht.readTemperature();
	// Read temperature as Fahrenheit

	debug_print("Temp: "); debug_print(t); debug_println("*C");
	debug_print("Humidity: "); debug_print(h); debug_println("%");

	msg.msgtype = MsgType_Data;
	msg.msgsubtype = DataMsgSubtype_Temp;
	msg.data[0] = *((int*)(&t));

	RF24NetworkHeader header(0);
	if (!network.write(header, &msg,sizeof(msg)))
	{
		debug_println("Error sending DHT sensor data");
		return;
	}

#ifdef NODE_SLEEP
	digitalWrite(DHT_POWER_PIN, LOW);
	pinMode(DHT_POWER_PIN, INPUT);
#endif

#endif //dht11
}


void loop(void){

	network.update();								// Check the network regularly


	while (network.available())
	{
		// Is there anything ready for us?
		RF24NetworkHeader header;
		network.read(header, &msg, sizeof(msg));
		send_sensor_data();
	}
}

