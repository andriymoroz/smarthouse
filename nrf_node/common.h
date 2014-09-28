#ifndef COMMON_H
#define COMMON_H

//

// 0 = Master
// 1-2 (02,05)   = Children of Master(00)

// 3,5 (012,022) = Children of (02)
// 4,6 (015,025) = Children of (05)
// 7   (032)     = Child of (02)
// 8,9 (035,045) = Children of (05)



#define NODE_KITCHEN


// Global definitions
#define DEBUG

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
#define NRF_CE_PIN   9
#define NRF_CSN_PIN  10
#define NRF_CHANNEL_MAIN 99




#ifdef NODE_BASEMENT
#define SENSOR_DHT
#define DHT_TYPE DHT11   // DHT 11
//#define DHT_TYPE DHT22   // DHT 22  (AM2302)
//#define DHT_TYPE DHT21   // DHT 21 (AM2301)
#define SENSOR_ONOFF
#endif

//const uint16_t node_adress = NODE_ADDRESS;		// Address of our node in Octal format ( 04,031, etc)

#ifdef NODE_KITCHEN
#define NODE_NAME "Kitchen"
#define NODE_ADDRESS 01
#define SENSOR_DHT
#define DHT_TYPE DHT11   // DHT 11
#define NODE_SLEEP 0
#endif


#endif // COMMON_H
