#ifndef SLEEP_H
#define SLEEP_H

#include <avr/wdt.h>
#include <avr/sleep.h>

void sleep_sleep_sleep()
{
	// disable ADC
	ADCSRA = 0;

	// clear various "reset" flags
	MCUSR = 0;
	// allow changes, disable reset
	WDTCSR = bit (WDCE) | bit (WDE);
	// set interrupt mode and an interval
	WDTCSR = bit (WDIE) | bit (WDP0) | bit (WDP3);    // set WDIE, and 8 second delay
	wdt_reset();  // pat the dog

	set_sleep_mode (SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	// turn off brown-out enable in software
	MCUCR = bit (BODS) | bit (BODSE);
	MCUCR = bit (BODS);
	sleep_cpu ();

	// cancel sleep as a precaution
	sleep_disable();

} // end of loop

#endif // SLEEP_H
