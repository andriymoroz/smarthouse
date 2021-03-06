#ifndef COMMON_H
#define COMMON_H

//

// 0 = Master
// 1-2 (02,05)   = Children of Master(00)

// 3,5 (012,022) = Children of (02)
// 4,6 (015,025) = Children of (05)
// 7   (032)     = Child of (02)
// 8,9 (035,045) = Children of (05)

#define NODE_SEND_PERIOD 8000

//#define NODE_KITCHEN
//#define NODE_BASEMENT
//#define NODE_ATTIC
//#define NODE_BEDROOM1
#define NODE_BEDROOM2

// Global definitions
//#define DEBUG

#define POWER_UP(pin)		pinMode(pin, OUTPUT); digitalWrite(pin, HIGH);
#define POWER_DOWN(pin)		digitalWrite(pin, LOW); pinMode(pin, INPUT);

//					,-++--++-.
//				----+10     9+----
//				----+11     8+---- DHT/DS18B20 data
//				----+12     7+---- DHT/DS18B20 power
//				----+13     6+----
//				----+A0     5+----
//				----+A1     4+----
//				----+A2     3+----
//				----+A3     2+----
//				----+Vcc  GND+
//				----+RST  RST+
//				----+GND   RX+
//				----+RAW   TX+
//					`-++++++-'

// sensors definition
// DS18B20
#define DS_DATA_PIN		8
#define DS_POWER_PIN	7

// DHT11/21
#define DHT_DATA_PIN	8
#define DHT_POWER_PIN	7


// NRF Network definitions
//#define NRF_POWER_PIN 6
#define NRF_CE_PIN			9
#define NRF_CSN_PIN			10
#define NRF_CHANNEL_MAIN	99

#ifdef NODE_BASEMENT
#define NODE_NAME "Basement"
#define NODE_ADDRESS 011
//#define SENSOR_DHT
//#define DHT_TYPE DHT11
//#define SENSOR_ONOFF
//#define NODE_SLEEP
#define NODE_SEND_PERIOD 1000
#endif

//const uint16_t node_adress = NODE_ADDRESS;		// Address of our node in Octal format ( 04,031, etc)

#ifdef NODE_KITCHEN
#define NODE_NAME "Kitchen"
#define NODE_ADDRESS 01
#define SENSOR_DHT
#define DHT_TYPE DHT11   // DHT 11
//#define NODE_SLEEP
#define NODE_SEND_PERIOD 10000
#endif

#ifdef NODE_ATTIC
#define NODE_NAME "Attic"
#define NODE_ADDRESS 02
#define NODE_SLEEP

#define SENSOR_DS
#define SENSOR_DHT
#define DHT_TYPE DHT11
// alternative DHT11/21 connection
#define DHT_DATA_PIN	5
#define DHT_POWER_PIN	6
#endif

#ifdef NODE_BEDROOM1 // our bedroom
#define NODE_NAME "Big bedroom"
#define NODE_ADDRESS 03
#define NODE_SLEEP
#define SENSOR_DS
#define NODE_SEND_PERIOD 1000
#endif

#ifdef NODE_BEDROOM2 // N bedroom
#define NODE_NAME "Nanika's bedroom"
#define NODE_ADDRESS 04
#define SENSOR_DHT
#define DHT_TYPE DHT22
#define NODE_SLEEP
#define NODE_SEND_PERIOD 1000
#endif


#endif // COMMON_H
