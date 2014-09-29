#include <cstdlib>
#include <iostream>
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <ctime>
#include <stdio.h>
/*#include <rrd.h>*/
#include <time.h>


// Setup for GPIO 25 CE and CE1 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS1, BCM2835_SPI_SPEED_8MHZ);

RF24Network network(radio);

// Address of our node in Octal format
#define NODE_ADDRESS 00
#define NRF_CHANNEL_MAIN 99

int main(int argc, char** argv)
{
	// Refer to RF24.h or nRF24L01 DS for settings
	radio.begin();
	delay(5);
	network.begin(NRF_CHANNEL_MAIN, NODE_ADDRESS);
	radio.printDetails();

	while(1)
	{
		network.update();
		while ( network.available() ) {     // Is there anything ready for us?

			RF24NetworkHeader header;        // If so, grab it and print it out
			payload_t payload;
			network.read(header,&payload, sizeof(payload));

			printf("Received payload # %lu at %lu \n",payload.counter,payload.ms);
		}
		sleep(2);
		//fclose(pFile);
	}

	return 0;
}
