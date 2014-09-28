#ifndef DEBUG_H
#define DEBUG_H

#if defined(DEBUG_SERIAL_PRINT)

#define debug_init Serial.begin
#define debug_print Serial.print
#define debug_println Serial.println
#else
#define debug_init
#define debug_print
#define debug_println
#endif

#endif // DEBUG_H
