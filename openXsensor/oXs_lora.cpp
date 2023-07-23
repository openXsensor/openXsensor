#include "oXs_lora.h"
#include <avr/pgmspace.h>

//#if defined( A_LOCATOR_IS_CONNECTED)  && ( A_LOCATOR_IS_CONNECTED == YES)

//LORA use pin 10, 11,12 and 13 (= PB2 to PB5)
// At oXs side, the principle is to set Lora device in continuous receive mode
// In a loop, we look if a packet has been received (if so, it is one byte long with the Tx power for next transmit)
// When oXs get this byte, it transmits immediately RSSI, SNR, and GPS data (0 if no GPS) and GPS precision and then go back to receive mode 
// receiving and sending are done on 2 different frequencies in order to reduce the channel occupation 


// SX127x series common LoRa registers
#define LORA_REG_FIFO                               0x00  //oXsTx     Receiver
#define LORA_REG_OP_MODE                            0x01  // should be 0b10000xxx
                                                          // bit7=1=Lora mode
                                                          // bit6=0 = do not access FSK registers
                                                          // bit3= LowfrequencyModeOn; 0=access to HF test reg; 1=access to LF test register
                                                          // bits2-0=Mode; 000=sleep, 001=standby, 011=Transmit, 101=Receive continous, 110=Receive single
#define LORA_REG_FRF_MSB                            0x06  //frequency (in steps of 61.035 Hz)
#define LORA_REG_FRF_MID                            0x07  //frequency
#define LORA_REG_FRF_LSB                            0x08  //frequency
#define LORA_REG_PA_CONFIG                          0x09  //power amplifier source and value eg. 0x8F for 17dbm on PA_boost
                                                          // bit 7 = 1 means PA_BOOST is used; min +2,max 17dBm (or 20 with 1%); 0= FRO pin = min -4,max 14 dBm
                                                          // bits 6-4 = MaxPowerPmax = 10.8 + 0.6MaxPower
                                                          // bits 0-3 = OutputPower
                                                          // Pout= 10.8 + 0.6MaxPowerPmax - 15 + OutputPower (if bit7=0)
                                                          // Pout= 2 + OutputPower (if bit7=1)
#define LORA_REG_PA_RAMP                            0x0A  //power amplifier ramp: only bits 3-0; ex: 1000= 62usec; default = 0x09 = 125usec
#define LORA_REG_OCP                                0x0B  // current protection: could be 0b00111011 (enabled at 240 ma)
                                                          // bit 5 = 1 = enabled
                                                          // bits 4-0 = OcpTrim
                                                          // Imax = 45+5*OcpTrim (if OcpTrim <=15 so 120mA)
                                                          //      = -30+10*OcpTrim (sinon et <=27 so 240mA)
                                                          // default 0x0B = 100mA; max value = OcpTrim=27 (décimal) = 11011
#define LORA_REG_LNA                                0x0C  // gain in reception ex: 0b00100011max gain, no boost on LNA)
                                                          // bits 7-5 = LnaGain
                                                          //       000 = not used
                                                          //       001 = max gain   // 110 = min gain
                                                          // bits 4-3 Low frequency LNA current adjustment; must be 00
                                                          // bits 1-0 High frequency LNA current adjustment
                                                          //       00 = Default LNA current
                                                          //       11 = Boost on, 150% LNA current
#define LORA_REG_FIFO_ADDR_PTR                      0x0D  // address of current byte to be read/writen in Fifo 
#define LORA_REG_FIFO_TX_BASE_ADDR                  0x0E  // base of Tx fifo; default 0x80
#define LORA_REG_FIFO_RX_BASE_ADDR                  0x0F  // base of Rx fifo; default 0x00
#define LORA_REG_FIFO_RX_CURRENT_ADDR               0x10  // adress of start of last Rx packet received (can't be written)
#define LORA_REG_IRQ_FLAGS_MASK                     0x11  // Irq flag mask
#define LORA_REG_IRQ_FLAGS                          0x12  // Irq flags (write 1 to bit to clear); write 0xFF clear all flags
                                                          // bit 7=RxTimeout, 6=RxDone, 5=CrcError, 4= validHeader
                                                          //     3=TxDone, 2=CadDone, 1=FhssChange, 0= Cad detected
#define LORA_REG_RX_NB_BYTES                        0x13  // Number of received bytes in payload of last packet received
#define LORA_REG_RX_HEADER_CNT_VALUE_MSB            0x14  // count nr of header received
#define LORA_REG_RX_HEADER_CNT_VALUE_LSB            0x15
#define LORA_REG_RX_PACKET_CNT_VALUE_MSB            0x16  // count nr of packet received
#define LORA_REG_RX_PACKET_CNT_VALUE_LSB            0x17
#define LORA_REG_MODEM_STAT                         0x18  // Live LoRaTM modem status (see page 111 of datasheet)
#define LORA_REG_PKT_SNR_VALUE                      0x19  // SNR of last packet received
                                                          // SNR = (bit 7-0 in 2 complement)/4
#define LORA_REG_PKT_RSSI_VALUE                     0x1A  // RSSI of last packet received
                                                          // see 5.5.5 of datasheet
#define LORA_REG_RSSI_VALUE                         0x1B  // current RSSI (not used)
#define LORA_REG_HOP_CHANNEL                        0x1C  // start of hop channel (not used)
#define LORA_REG_MODEM_CONFIG_1                     0x1D  // config of modem part 1  // e.g. BW=125,CR=4/5 , no Header => 0b01110011
                                                          // bits 7-4 = BW; 0110 = 62.5Khz; 0111=125Khz
                                                          // bits 3-1 = CR ; 001 = 4/5; 100=4/8
                                                          //bit 0: 0=Explicit Header; 1=no Header
#define LORA_REG_MODEM_CONFIG_2                     0x1E  // config of modem part 2 //e.g.SF=10,1 packet,CRCon,=> Ob10100100
                                                          // bits 7-4=SF ; from 6 up to 12
                                                          // bit 3=TxContinous mode;0=one packet only
                                                          // bit2=RxPayloadCrcON ; 1=Enable
                                                          // bits1-0= SymbTimeOut(9:8) = MSB
#define LORA_REG_SYMB_TIMEOUT_LSB                   0x1F  // Receiver timeout value LSB (in single mode) in number of symbols
#define LORA_REG_PREAMBLE_MSB                       0x20  // size of preamble; e.g. 0x0006 (default 000C)
#define LORA_REG_PREAMBLE_LSB                       0x21
#define LORA_REG_PAYLOAD_LENGTH                     0x22  // Payload length; has to be defined when no header 
                                                          // e.g. 0x02 (for Receiver => 150msec) and 0x06 (for oXs => 190msec) 
#define LORA_REG_MAX_PAYLOAD_LENGTH                 0x23  // max payload length (not used??? when no header); for safety, set to 0x06
#define LORA_REG_HOP_PERIOD                         0x24  // frequency hop period (not used)
#define LORA_REG_FIFO_RX_BYTE_ADDR                  0x25  // adress of last byte written in Fifo in receive mode
#define LORA_REG_MODEM_CONFIG_3                     0x26  // config of modem part 3 ; e.g. 0b00001100
                                                          // bit3=LowDataRateOptimize; 1=Enabled mandated when symbol length exceeds 16ms
                                                          // bit2=AgcAutoOn; 1=LNA gain set by internal AGCloop instead of by register LnaGain
#define LORA_REG_FEI_MSB                            0x28  // estimated frequency error
#define LORA_REG_FEI_MID                            0x29
#define LORA_REG_FEI_LSB                            0x2A
#define LORA_REG_RSSI_WIDEBAND                      0x2C  //wideband RSSI measurement (= average) (not used)
#define LORA_REG_DETECT_OPTIMIZE                    0x31  // lora detection optimize ;e.g. 0x03 (for sf10)
                                                          // bit 2-0 = 011 for SF7 to 12; 0101 for sf6 only; default 011
#define LORA_REG_INVERT_IQ                          0x33  // Invert lora I and Q signals
#define LORA_REG_DETECTION_THRESHOLD                0x37  // lora detection threshold default 0X0A is ok for SF7-12; 0x0C for sf6
#define LORA_REG_SYNC_WORD                          0x39  // lora Sync Word (default 0x12)
#define LORA_REG_DIO_MAPPING_1                      0x40  // DIO mapping
#define LORA_REG_DIO_MAPPING_2                      0x41
#define LORA_REG_VERSION                            0x42  // lora version
#define LORA_REG_PA_DAC                             0x4D  // 0x84 = normal power (up to 17 dBm); 0x87= boost (20dBm)

// SX127x common LoRa modem settings
// LORA_REG_OP_MODE                                                 MSB   LSB   DESCRIPTION
#define LORA_SLEEP                                  0b00000000  //  2     0     sleep
#define LORA_STANDBY                                0b00000001  //  2     0     standby
#define LORA_TX                                     0b00000011  //  2     0     transmit
#define LORA_RXCONTINUOUS                           0b00000101  //  2     0     receive continuous
#define LORA_RXSINGLE                               0b00000110  //  2     0     receive single


// IRQ masks
#define IRQ_TX_DONE_MASK           0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define IRQ_RX_DONE_MASK           0x40

// Process depends on loraState; it can be
#define LORA_TO_INIT 0           // device must be initialized
#define LORA_IN_SLEEP 1          // wait a delay and set lora in recieve continous mode
#define LORA_IN_RECEIVE 2             // wait that a package has been received or a max delay; if package has been received,Tx power changes, update Tx power, change mode to LORA_TO_TRANSMIT
#define LORA_TO_TRANSMIT 3           //fill lora with data to be send and ask for sending (but do not wait), change mode to LORA_WAIT_END_OF_TRANSMIT
#define LORA_WAIT_END_OF_TRANSMIT 4   //wait that pakage has been sent (or wait max x sec)

#define TX_FRF_MSB   0xC0 // F / 32E6 * 256 * 256 * 8
#define TX_FRF_MID   0x00   
#define TX_FRF_LSB   0x00

#define RX_FRF_MSB   0xC0 
#define RX_FRF_MID   0x00  
#define RX_FRF_LSB   0x00


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

uint8_t loraRxPacketRssi ;
uint8_t loraRxPacketSnr ;
uint8_t loraRxPacketType ;
uint8_t loraRxPacketTxPower ;
uint8_t loraState = 0 ;


extern uint32_t GPS_last_fix_millis ; // time when last fix has been received (used by lora locator) 
extern int32_t GPS_last_fix_lon ;     // last lon when a fix has been received
extern int32_t GPS_last_fix_lat ;     // last lat when a fix has been received
extern uint16_t GPS_hdop ;

void initSpi(){     // configure the SPI
  DDRB |= (1<<2)|(1<<3)|(1<<5);    // SCK, MOSI and SS as outputs on PORTB (PB2,PB3,PB5)
  DDRB &= ~(1<<4);                 // MISO as input (PB4)
  PORTB |= (1<<2)  ;                // Set chip select high (PB2)
  
  SPCR =  (1<<MSTR) |               // Set as Master, send most significant bit first (because bit is left on 0)
          (1<<SPR0) |               // divided clock by 16
          (1<<SPE);                 // Enable SPI, disable interrupt (bit7 on 0)
  SPSR = 1 ;                        // Bit0 allows to multiply frequency by 2 (in master mode)
}

//shifts out 8 bits of data
//  uint8_t data - the data to be shifted out
//  returns uint8_t - the data received during sending
uint8_t spiSend(uint8_t value){
  uint8_t result;
  SPDR = value; //shift the first byte of the value
  while(!(SPSR & (1<<SPIF))); //wait for the SPI bus to finish
  result = SPDR; //get the received data
  return result;
}

#define SPI_SELECT (PORTB &= ~(1<<2) ) // macro for selecting LORA device
#define SPI_UNSELECT (PORTB |= (1<<2) ) // macro for unselecting LORA device  

uint8_t loraSingleTransfer(uint8_t reg, uint8_t value) {  // only for internal use; Write and read one LORA register
  uint8_t response;
  SPI_SELECT ;
  spiSend(reg);
  response = spiSend(value);
  SPI_UNSELECT ;
  return response;
}

void loraWriteRegister(uint8_t reg, uint8_t value) {   // write a LORA register, discard the read value
  loraSingleTransfer(reg | 0x80, value);
}

uint8_t loraReadRegister(uint8_t reg) {                // Read a LORA register; send a dummy value because it is just a read
  return loraSingleTransfer(reg & 0x7f, 0x00);
}

void loraReadRegisterBurst( uint8_t reg, uint8_t* dataIn, uint8_t numBytes) {
  SPI_SELECT ;
  spiSend(reg & 0x7f);
  for(size_t n = 0; n < numBytes; n++) {
        dataIn[n] = spiSend(0x00);
  }
  SPI_UNSELECT ;
}

void loraWriteRegisterBurst( uint8_t reg, uint8_t* dataOut, uint8_t numBytes) {
  SPI_SELECT ;
  spiSend(reg | 0x80);
  for(size_t n = 0; n < numBytes; n++) {
        spiSend(dataOut[n]);
  }
  SPI_UNSELECT ;
}



#define SLEEP_TIME 55000 // sleep during 55 sec
#define SHORT_RECEIVE_TIME 5000  // wait 5 sec for a packet
#define LONG_RECEIVE_TIME 60000  // stay in receive for 1 min after receiving a packet

void loraHandle(){ 
  uint8_t loraIrqFlags ;
  static uint32_t loraStateMillis ;
  //static uint32_t receiveMillis ;
  uint32_t currentMillis = millis() ;
  //Serial.println(loraState);
  //Serial.println(loraStateMillis ) ;
  switch (loraState) { 
    case  LORA_TO_INIT :
        initSpi();   // init spi
        loraSetup() ; // init lora with some set up that need to be done only once
        loraState = LORA_IN_SLEEP ;
        loraStateMillis = currentMillis + SLEEP_TIME ;
#ifdef DEBUGLORA
        Serial.println("End of init") ;
#endif        
        break ;
    case  LORA_IN_SLEEP :
        if (currentMillis > loraStateMillis ){ 
#ifdef DEBUGLORA
          Serial.println("End of sleep; enter receive mode for 5 sec");
#endif          
          loraRxOn(); 
          loraState = LORA_IN_RECEIVE ; 
          loraStateMillis = currentMillis + SHORT_RECEIVE_TIME ;
        }
        break;    
    case  LORA_IN_RECEIVE :
        // check if a packet has been received with a correct CRC
        loraIrqFlags = loraReadRegister(LORA_REG_IRQ_FLAGS);
        if ( loraIrqFlags & IRQ_RX_DONE_MASK  ) {
          if ( loraIrqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) {
#ifdef DEBUGLORA            
            Serial.println("wrong crc received") ;
#endif
            loraRxOn() ; // restart reading in continous mode if CRC is wrong (reset address and interrupt)
          } else {
              loraReadPacket() ; // read the data in fifo (type and TxPower) and Rssi+SNR 
              loraInSleep() ;
              loraState = LORA_TO_TRANSMIT;
#ifdef DEBUGLORA
              Serial.println("packet received") ;
#endif
          }
        } else if (currentMillis > loraStateMillis) {
#ifdef DEBUGLORA
           Serial.println("receive timeout; go to sleep") ;
#endif
           loraInSleep() ;
           loraState = LORA_IN_SLEEP;
           loraStateMillis = currentMillis + SLEEP_TIME ;
        }
        break;
    case  LORA_TO_TRANSMIT :
        loraFillTxPacket() ; // set mode to standby, fill fifo with data to be sent (6 bytes)  
        loraTxOn(loraRxPacketTxPower) ; // set TxOn  (adjust frequency, number of bytes, Txpower, start Tx)  // set lora in transmit mode
        loraStateMillis = currentMillis + 400 ; // start a timeout ; normally sending is done in less than 200msec
        loraState = LORA_WAIT_END_OF_TRANSMIT ;
#ifdef DEBUGLORA
        Serial.println("packet redy for sending") ;
#endif
        break;
    case  LORA_WAIT_END_OF_TRANSMIT :
        // check if transmit is done or if timeout occurs
        // if transmitted, put lora in receive, change loraState to LORA_IN_RECEIVE , change loraStateMillis = currentMillis+LONG_RECEIVE
        // else, if timeOut, go in sleep for the SLEEP_TIME
        loraIrqFlags = loraReadRegister(LORA_REG_IRQ_FLAGS);
        if ( loraIrqFlags & IRQ_TX_DONE_MASK ) {
#ifdef DEBUGLORA
            Serial.println("packet has been sent; go to receive for 60sec") ;
#endif
            loraRxOn();
            loraState = LORA_IN_RECEIVE ; 
            loraStateMillis = currentMillis + LONG_RECEIVE_TIME ;
        } else if ( currentMillis > loraStateMillis ) {
#ifdef DEBUGLORA
            Serial.println("transmit timeout; go to sleep for 66") ;
#endif
            loraInSleep() ;
            loraState = LORA_IN_SLEEP;
            loraStateMillis = currentMillis + SLEEP_TIME ;
        }
        
        break;

    } // end of switch
}

void loraSetup() {         // parameters that are set only once
  loraWriteRegister(LORA_REG_OP_MODE,  0x00);           //RegOpMode, need to set to sleep mode before configure for LoRa mode
  loraWriteRegister(LORA_REG_OP_MODE,  0x80);           // activate LORA mode and High frequency, stay in sleep mode
  loraWriteRegister(LORA_REG_PA_CONFIG, 0x8F) ;         // activate PA_Boost and max power = 17 dBm
  //loraWriteRegister(LORA_REG_PA_RAMP, 0x09)) ;         // keep default value
  loraWriteRegister(LORA_REG_OCP, 0b00111011) ;         // current protection On with max value of 240 mA
  loraWriteRegister(LORA_REG_LNA, 0x23) ;               // max gain in reception , boost on LNA
  loraWriteRegister(LORA_REG_MODEM_CONFIG_1,0b01110011) ;// BW=125khz,CR=4/5,No header
                                                          // bits 7-4 = BW; 0110 = 62.5Khz; 0111=125Khz
                                                          // bits 3-1 = CR ; 001 = 4/5; 100=4/8
                                                          //bit 0: 0=Explicit Header; 1=no Header
  loraWriteRegister(LORA_REG_MODEM_CONFIG_2,0b10100100) ; // SF=10,One Tx packet,CrcOn, timeOutMsb=0
                                                          // bits 7-4=SF ; from 6 up to 12
                                                          // bit 3=TxContinous mode;0=one packet only
                                                          // bit2=RxPayloadCrcON ; 1=Enable
                                                          // bits1-0= SymbTimeOut(9:8) = MSB
  loraWriteRegister(LORA_REG_SYMB_TIMEOUT_LSB, 0xFF) ;         // Receiver time out in single mode
  //loraWriteRegister(LORA_REG_PREAMBLE_MSB, 0x00) ;     // keep 00 default value
  loraWriteRegister(LORA_REG_PREAMBLE_LSB, 6) ;          // 6 preamble sysbols 
  loraWriteRegister(LORA_REG_MODEM_CONFIG_3, 0b00000100);  //
                                                          // bit3=LowDataRateOptimize; 1=Enabled mandated when symbol length exceeds 16ms
                                                          // bit2=AgcAutoOn; 1=LNA gain set by internal AGCloop instead of by register LnaGain
}

void loraTxOn(uint8_t txPower){                           // if TX power <= 15, do not use the boost
  loraWriteRegister(LORA_REG_OP_MODE,  0x80 | LORA_STANDBY);
  if ( txPower <= 15) {
    loraWriteRegister(LORA_REG_PA_CONFIG, 0x80 | txPower) ; // use PA_boost (power is from 2 up to 17dBm
    loraWriteRegister(LORA_REG_PA_DAC , 0x84 ) ;            // 0x84 = normal power (up to 17 dBm); 0x87= boost (20dBm)
  } else {                                                  // for this project, we use only normal power (no boost
    loraWriteRegister(LORA_REG_PA_CONFIG, 0x80 | 15) ; // use PA_boost (power is from 2 up to 17dBm
    loraWriteRegister(LORA_REG_PA_DAC , 0x84 ) ;            // 0x84 = normal power (up to 17 dBm); 0x87= boost (20dBm)  
  }
  loraWriteRegister(LORA_REG_FRF_MSB, TX_FRF_MSB);    //frequency (in steps of 61.035 Hz)
  loraWriteRegister(LORA_REG_FRF_MID, TX_FRF_MID);      
  loraWriteRegister(LORA_REG_FRF_LSB, TX_FRF_LSB);
  loraWriteRegister(LORA_REG_IRQ_FLAGS, 0xFF);       //reset interrupt flags
  loraWriteRegister(LORA_REG_PAYLOAD_LENGTH,6);      // set payload on 6 (because it is the same time on air as 5
  loraWriteRegister(LORA_REG_OP_MODE,  0x80 | LORA_TX);  
}


void loraRxOn(){
  loraWriteRegister(LORA_REG_OP_MODE,  0x80);
  loraWriteRegister(LORA_REG_FIFO_RX_BASE_ADDR, 0x00); // reset base Rx adress to 0
  loraWriteRegister(LORA_REG_FRF_MSB, RX_FRF_MSB);    //frequency (in steps of 61.035 Hz)
  loraWriteRegister(LORA_REG_FRF_MID, RX_FRF_MID);      
  loraWriteRegister(LORA_REG_FRF_LSB, RX_FRF_LSB);
  loraWriteRegister(LORA_REG_IRQ_FLAGS, 0xFF);       //reset interrupt flags
  loraWriteRegister(LORA_REG_PAYLOAD_LENGTH,2);      // set payload on 6 (because it is the same time on air as 5
  loraWriteRegister(LORA_REG_OP_MODE,  0x80 | LORA_RXCONTINUOUS); 
}

void loraInSleep(){
  loraWriteRegister(LORA_REG_OP_MODE,  0x80);
}  

void loraReadPacket()            // read a packet with 2 bytes ; PacketType and PacketTxPower
{
  loraRxPacketRssi = loraReadRegister( LORA_REG_PKT_RSSI_VALUE );
  loraRxPacketSnr = loraReadRegister( LORA_REG_PKT_RSSI_VALUE );
  loraWriteRegister(LORA_REG_FIFO_ADDR_PTR, 0);        //set RX FIFO ptr
  SPI_SELECT ;       // start of read burst on fifo
  spiSend(LORA_REG_FIFO);                //address for fifo
  loraRxPacketType = spiSend(0);
  loraRxPacketTxPower = spiSend(0);
  SPI_UNSELECT ;
#ifdef DEBUGLORA  
  Serial.print("Rssi=");Serial.println(loraRxPacketRssi ) ;
  Serial.print("Snr=");Serial.println( loraRxPacketSnr) ;
  Serial.print("Type=");Serial.println(loraRxPacketType ) ;
  Serial.print("Power=");Serial.println(loraRxPacketTxPower ) ;
#endif
}


void loraFillTxPacket() {
  // data to be sent are type of packet, Long, Lat, Gps accuracy, rssi, snr, nbr of min since last GPS
  // in order to stay with 6 bytes per packet, we will send 1 byte with
  //         Bit 7/ 0 = Long, 1=Lat ; this is type of packet
  //         Bit 6/3 = GPS accuracy 0= very good; 15 = bad (we discard décimal); it is gps_HDOP/128, if >15, then becomes 15
  //         Bit 2/0/= gps oldier than than x sec....1h; 0 = no GPS (yet)
  //         Then there is one byte with Rssi and 4 bytes for long/lat (or replaced by 00 if GPS is unavailable)
  //         Note: gps_last_fix_lon and lat are filled with last value when a fix was available and if a fix has never been available they are filled with 0
  uint8_t loraTxBuffer[6] ;
  static uint8_t packetType = 0 ;
  uint16_t temp16 = 0 ;
  uint8_t temp8  = 0 ;
  int32_t gpsPos = 0 ;
  packetType = (~packetType ) & 0x80 ;                  // reverse first bit
#if defined( A_GPS_IS_CONNECTED ) && ( A_GPS_IS_CONNECTED == YES)
  temp16 = (GPS_hdop >> 7) ;  // Divide by 128 instead of 100 to go faster and keep only 4 bytes
  if ( temp16 > 0x0F) temp16 = 0x0F ;
  packetType |= (temp16 << 3 ) ; // shift in pos 3 to 6
  if ( GPS_last_fix_millis > 0 ) { // if we receive at least one fix then we calculate the nelapsed time 
    temp16 = ( millis() - GPS_last_fix_millis) >> 16 ; // to save bytes, we divide millisec by 1024
    if ( temp16 > 3515 ) {
      temp8 = 6 ; // more than 1 h (3515 = 3600000/1024 
    } else if ( temp16 > 585 ) {
      temp8 = 5 ; // more than 10 min (585 = 600000/1024
    } else if ( temp16 > 59 ) {
      temp8 = 4 ; // more than 1 min (59 = 60000/1024
    } else if ( temp16 > 10 ) {
      temp8 = 3 ; // more than 10 sec (10 = 10000/1024
    } else if ( temp16 > 1 ) {
      temp8 = 2 ; // more than 1 sec (1 = 1000/1024
    } else {
      temp8 = 1 ; // less than 1 sec
    }
  }
#endif
  loraTxBuffer[0] = packetType | temp8 ;
  loraTxBuffer[1] = loraRxPacketRssi ; // RSSI value of last packet
#if defined( A_GPS_IS_CONNECTED ) && ( A_GPS_IS_CONNECTED == YES)
  if (packetType & 0x80) {
      gpsPos = GPS_last_fix_lat ;
  } else {
    gpsPos = GPS_last_fix_lon ;
  }
#endif
  loraTxBuffer[2] = gpsPos >> 24 ;
  loraTxBuffer[3] = gpsPos >> 16 ;
  loraTxBuffer[4] = gpsPos >> 8 ;
  loraTxBuffer[5] = gpsPos ;
   
  loraWriteRegister(LORA_REG_OP_MODE, 0x80 | LORA_STANDBY) ; //  set mode in standby ( to write FIFO)
  loraWriteRegister(LORA_REG_FIFO_ADDR_PTR, 0x80 );        // set FifoAddrPtr to 80 (base adress of byte to transmit)
  loraWriteRegisterBurst( LORA_REG_FIFO , loraTxBuffer, 6) ; // write the 6 bytes in lora fifo
}

//#endif
