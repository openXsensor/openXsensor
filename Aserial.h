/* ============================================================
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================*/

// Author: Mike Blandford

#ifdef FRSKY_SPORT
struct t_sportData
{
	struct t_sportData *next ;
	uint8_t data[7] ;
	uint8_t dataLock ;
	uint8_t serialSent ;
} ;

void setNewData( struct t_sportData *pdata, uint16_t id, uint32_t value ) ;
void initSportUart( struct t_sportData *pdata ) ;

extern uint8_t DataSent ;
extern uint8_t SportSync ;


extern volatile uint8_t sendStatus ;

#else // Hub protocol ================================

#define maxSizeBuffer 70  // max size of the buffer used to store the data to be sent in the hub protocol

struct t_hubData
{
	uint8_t data[maxSizeBuffer] ; 
	uint8_t currentData ; //index of current data
	uint8_t maxData ;   // max number of data prepared to be send
} ;

void initHubUart( struct t_hubData *pdata ) ;
//void sendHubByte( uint8_t byte ) ;
void setNewData( struct t_hubData *pdata ) ;
void startTransmit( void ) ;

#endif // FRSKY_SPORT

 
extern volatile bool RpmSet ;
extern volatile uint16_t Rpm ;


unsigned long micros( void ) ;
unsigned long millis( void ) ;


