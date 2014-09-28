#ifndef COMMON_H
#define COMMON_H

#define DEBUG


#ifdef NODE_BASEMENT
#define SENSOR_DHT11
#define SENSOR_ONOFF
#endif

//const uint16_t node_adress = NODE_ADDRESS;		// Address of our node in Octal format ( 04,031, etc)

#ifdef NODE_KITCHEN
#define NODE_NAME "Kitchen"
#define NODE_ADDRESS 001
#define SENSOR_DHT11
#endif


#endif // COMMON_H
