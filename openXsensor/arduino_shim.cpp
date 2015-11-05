
#include "arduino_shim.h"

#include "I2C.h"

_EXTERN_ATTRIB int shim_i2c_write(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char const *data) {
//	return (I2Cdev::writeBytes(slave_addr,reg_addr,length,(uint8_t *)data) ? 0 : -1); // return 0 on success
	  return (I2c.write( (uint8_t) slave_addr, (uint8_t) reg_addr,  (uint8_t) length , (uint8_t *)data )  ? 0 : -1); // return 0 on success	
}

_EXTERN_ATTRIB int shim_i2c_read(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char *data) {
//	return (I2Cdev::readBytes(slave_addr,reg_addr,length,(uint8_t *)data) ? 0 : -1); // return 0 on success
    return (I2c.read( (uint8_t) slave_addr, (uint8_t) reg_addr, (uint8_t) length , (uint8_t *)data )  ? 0 : -1); // return 0 on success	
}

_EXTERN_ATTRIB void shim_get_ms(unsigned long *count) {
	*count = millis();
}

