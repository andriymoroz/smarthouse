
#include <stdint.h>

//#define DEBUG_SERIAL_PRINT

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "nrfmessage.h"
#include "common.h"
#include "debug.h"

RF24 radio(9,10);								// nRF24L01(+) radio attached using Getting Started board
RF24Network network(radio);						// Network uses that radio

NrfMessage msg;

void setup(void)
{
	debug_init(9600);
	debug_println("");

	SPI.begin();
	radio.begin();
	network.begin(/*channel*/ 99, /*node address*/ NODE_ADDRESS);
}

void send_sensor_data()
{

}


void loop(void){

	network.update();								// Check the network regularly


	while (network.available())
	{	// Is there anything ready for us?
		RF24NetworkHeader header;
		payload_t payload;
		network.read(header,&payload,sizeof(payload));
		//    debug_print("Received packet #");
		//    debug_print(payload.counter);
		//    debug_print(" at ");
		//    debug_print(payload.ms);
		//    debug_print(" from ");
		//    debug_println(header.from_node);
	}
}

