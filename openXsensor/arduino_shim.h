#ifndef __ARDUINO_SHIM
#define __ARDUINO_SHIM

#include "Arduino.h"

#define get_ms 		shim_get_ms
#define delay_ms    delay

//#define SERIAL_OUTPUT_DEVICE Serial // comment out if no debugging required

#ifdef SERIAL_OUTPUT_DEVICE

extern HardwareSerial Serial;
#define log_i       SERIAL_OUTPUT_DEVICE.println
#define log_e		SERIAL_OUTPUT_DEVICE.println

#else // No Debug Output

#define log_i(...) do { } while (0)
#define log_e(...) do { } while (0)

#endif

#define __no_operation() __asm__("nop\n\t") // emit AVR no-op 

#ifdef __cplusplus
#define _EXTERN_ATTRIB extern "C"
#else
#define _EXTERN_ATTRIB
#endif

_EXTERN_ATTRIB int shim_i2c_write(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char const *data);
_EXTERN_ATTRIB int shim_i2c_read(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char *data);
_EXTERN_ATTRIB void shim_get_ms(unsigned long *count);

#endif