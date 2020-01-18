#ifndef OXS_LORA_h
#define OXS_LORA_h

#include "oXs_config_basic.h"
#include "oXs_config_macros.h"

//#if defined( A_LOCATOR_IS_CONNECTED)  && ( A_LOCATOR_IS_CONNECTED == YES)
#include "Arduino.h"

void initSpi() ; // initialise SPI (with only 1 slave)
uint8_t spiSend(uint8_t value); 
uint8_t loraSingleTransfer(uint8_t reg, uint8_t value) ;  // only for internal use; Write and read one LORA register
void loraWriteRegister(uint8_t reg, uint8_t value) ;   // write a LORA register, discard the read value
uint8_t loraReadRegister(uint8_t reg) ;                // Read a LORA register; send a dummy value because it is just a read
void loraReadRegisterBurst( uint8_t reg, uint8_t* dataIn, uint8_t numBytes) ;
void loraWriteRegisterBurst( uint8_t reg, uint8_t* dataOut, uint8_t numBytes) ;

void loraHandle() ;
void loraSetup() ;         // parameters that are set only once
void loraTxOn(uint8_t txPower);
void loraRxOn() ;
void loraInSleep() ;
void loraReadPacket() ;           // read a packet with 2 bytes ; packetType and TxPower
void loraFillTxPacket() ;



//#endif
#endif
