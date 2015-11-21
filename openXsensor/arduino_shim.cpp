
#include "arduino_shim.h"

#include "I2C.h"

#define INV6050_addrBis 0x68

_EXTERN_ATTRIB int shim_i2c_write(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char const *data) {
	  return I2c.write( (uint8_t) slave_addr, (uint8_t) reg_addr,  (uint8_t) length , (uint8_t *)data ) ; // return 0 on success	
}

_EXTERN_ATTRIB int shim_i2c_writeByte( unsigned char reg_addr, unsigned char data) {
    return I2c.write( INV6050_addrBis , (uint8_t) reg_addr,  (uint8_t) data ) ; // return 0 on success 
}


_EXTERN_ATTRIB int shim_i2c_read(unsigned char slave_addr, unsigned char reg_addr,unsigned char length, unsigned char *data) {
    return I2c.read( (uint8_t) slave_addr, (uint8_t) reg_addr, (uint8_t) length , (uint8_t *)data ) ; // return 0 on success	
}


_EXTERN_ATTRIB int shim_i2c_writeBlock( unsigned char reg_addr,unsigned char length, unsigned char const *data) {
    return I2c.write(  INV6050_addrBis, (uint8_t) reg_addr,  (uint8_t) length , (uint8_t *)data ) ; // return 0 on success  
}

