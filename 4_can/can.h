#ifndef __CAN_H__
#define __CAN_H__

#include <stdint.h>

typedef struct  {
  uint32_t idf;     //29 bits	
  uint8_t  data[8];	//8 bytes max
  uint8_t  len;     //data length: 0 to 8
} CANMsg;

// init CAN hardware, @500KHz
// CAN_RX is PA11, alternate function 9 (D10 on Nucleo32)
// CAN_TX is PA12, alternate function 9 (D2  on Nucleo32)
// * filter messages : all trafic got to the receive FIFO0
// * interrupt on fifo0 (
void CANSetup();

//send a frame (extended id) using a transmit mailbox.
//if there is no mailbox available (3 CAN messages are 
//still waiting to be transmitted), then it returns -1
//It returns 0 if all is Ok
//NOTE: this is not a blocking function, and the message may not be
//      sent at the end of function. It only sets the message and 
//      gives the order to transmit. 
int CANSendMessage(CANMsg *msg);

#endif
