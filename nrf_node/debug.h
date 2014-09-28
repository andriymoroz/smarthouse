#ifndef DEBUG_H
#define DEBUG_H

#if defined(DEBUG_SERIAL_PRINT)

#define debug_init Serial.begin
#define debug_print Serial.print
#define debug_println Serial.println
#else
#endif
#endif // DEBUG_H
