#include "stm32f3xx.h"
#include "can.h"

//basic example:
// * get information from just received message
//   (identifier, data lengh, data[])
// * then generate an echo if the message identifier is odd,
//   just inverting data bits, and identifier lsb.  
void CAN_RX0_IRQHandler()
{
	CANMsg msg;
	//receive a message
	msg.idf = CAN->sFIFOMailBox[0].RIR >> 3;
	msg.len = CAN->sFIFOMailBox[0].RDTR & 0xf;
	msg.data[0] = CAN->sFIFOMailBox[0].RDLR       & 0xff;
	msg.data[1] = CAN->sFIFOMailBox[0].RDLR >> 8  & 0xff;
	msg.data[2] = CAN->sFIFOMailBox[0].RDLR >> 16 & 0xff;
	msg.data[3] = CAN->sFIFOMailBox[0].RDLR >> 24 & 0xff;
	msg.data[4] = CAN->sFIFOMailBox[0].RDHR       & 0xff;
	msg.data[5] = CAN->sFIFOMailBox[0].RDHR >> 8  & 0xff;
	msg.data[6] = CAN->sFIFOMailBox[0].RDHR >> 16 & 0xff;
	msg.data[7] = CAN->sFIFOMailBox[0].RDHR >> 24 & 0xff;

	//example: generate an echo if msg.idf is odd 
	if(msg.idf & 1) {
		//just update the identifier (should not be the same)
		msg.idf ^= 1; //invert lsb => idf is even.
		for(int i=0;i < 8; i++) msg.data[i] = ~msg.data[i]; //invert bit on echo
		CANSendMessage(&msg);
	}
	
	//Ok message received: set RFR.RFOM bit to release FIFO mailbox. 
	//if fifo is empty, it acks the interrupt (i.e. FPM returns to 0).
	CAN->RF0R |= CAN_RF0R_RFOM0;
}

void CANSetup()  {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;	// enable clock for GPIOA
	RCC->APB1ENR |= RCC_APB1ENR_CANEN;  // enable clock for CAN
	asm("nop");							// wait until CAN & GPIOA clocks are Ok.
	// CAN_RX is PA11, alternate function 9 (D10 on Nucleo32)
	// CAN_TX is PA12, alternate function 9 (D2  on Nucleo32)
	//p.40 of datasheet (physical part)
	GPIOA->AFR[1] |= 9 << GPIO_AFRH_AFRH3_Pos|	//AFRH3 => pin 8+3=11
	                 9 << GPIO_AFRH_AFRH4_Pos;	//alternate func is AF9
	GPIOA->MODER |= 2 << GPIO_MODER_MODER11_Pos|
	                2 << GPIO_MODER_MODER12_Pos; //alternate function
	//reset can peripheral (mandatory!)
	RCC->APB1RSTR |=  RCC_APB1RSTR_CANRST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_CANRST;
	
	//init state:
	//	* set INRQ bit in MCR and wait until INAK bit in MSR is Ok.
	CAN->MCR = CAN_MCR_INRQ | CAN_MCR_NART;	//no mask: all other fields to 0 (reset state).
	while(!(CAN->MSR & CAN_MSR_INAK));
	CAN->IER = 0;	//no interrupt enabled.
	//input clock is PCLK => 32MHz
	//set baud rate: 250K
	// => 128 input clocks /bit.
	// 1 CAN bit is set to 16 time quantas (tq)
	// => 1 time quanta each 8 input clock bit.
	// 1 CAN bit is set with:
	// 1 sync tq - 11 seg1 tq - 4 seg2 tq => sample point at 75%
	// no mask - define whole register - other fields to default state.
	CAN->BTR =	(8-1)  << CAN_BTR_BRP_Pos | //1tq each 8 input clock
				(11-1) << CAN_BTR_TS1_Pos | //11 tq in seg1
				(4-1)  << CAN_BTR_TS2_Pos | //4 tq in seg2
				(2-1)  << CAN_BTR_SJW_Pos ; //2 tq to jump for resync.

	//debug: loop back mode
	//CAN->BTR |= CAN_BTR_LBKM;
	
	//ok, now leave init mode (and remove the NART bit).
	//We just set ABOM (Automatic Bus Off recovery, in case of big pb…)
	//No mask.
	CAN->MCR = CAN_MCR_ABOM; 
	while(CAN->MCR & CAN_MCR_INRQ);	//wait until it is ok.
	//wait until Transmit box is empty.
	while(!(CAN->TSR & CAN_TSR_TME0));
	
	//no filter on input => all trafic to FIFO0
	//should filter on mask: FSC0=1 (in FS1R reg), FBM0=0 (in FM1R reg) 
	CAN->FMR |= CAN_FMR_FINIT;	//init filter config
	CAN->FA1R &= ~1;			// make sure filter 0 is inactive

	CAN->FS1R |= 1;				// filter 0 scale config
	CAN->FM1R &= ~1;			// filter 0 mode

	CAN->sFilterRegister[0].FR1 = 0; //identifier
	CAN->sFilterRegister[0].FR2 = 0; //mask: all 0 => do not care
	
	CAN->FFA1R &= ~1;			// assign filter to FIFO 0
	CAN->FA1R |= 1;				//filter 0 active
	CAN->FMR &= ~CAN_FMR_FINIT;	//end of filter config

	////Rx interrupt on FIFO0
	CAN->IER |= CAN_IER_FMPIE0;	//Fifo Message Pending Interrupt on Enable Fifo 0
	NVIC_SetPriority(CAN_RX0_IRQn, 1); //near top priority…
	NVIC_EnableIRQ(CAN_RX0_IRQn);
}

int CANSendMessage(CANMsg *msg)
{
	int result;
	//check if there is a transmit mailbox available
	//TME Transmit Buffer Empty => if the 3 buffers waitting to Tx,
	//the code field is set to 0
	if((CAN->TSR  & CAN_TSR_TME) != 0)
	{
		//there is at least one empty Tx buffer. 
		//get one
		const int bufferId = (CAN->TSR & CAN_TSR_CODE) >> CAN_TSR_CODE_Pos;
		if(bufferId == 3) return -2; //should not happen :/
		//and set the message.
		CAN->sTxMailBox[bufferId].TIR = (msg->idf & 0x1FFFFFFF) << CAN_TI0R_EXID_Pos |
                                        CAN_TI0R_IDE; //extended msg.
		CAN->sTxMailBox[bufferId].TDTR = (msg->len & 0xF);
		CAN->sTxMailBox[bufferId].TDLR = msg->data[0]       |
		                         msg->data[1] << 8  |
		                         msg->data[2] << 16 |
		                         msg->data[3] << 24 ;
		CAN->sTxMailBox[bufferId].TDHR = msg->data[4]       |
		                         msg->data[5] << 8  |
		                         msg->data[6] << 16 |
		                         msg->data[7] << 24 ;
		CAN->sTxMailBox[bufferId].TIR |= CAN_TI0R_TXRQ;
		result = 0;
	} else result = -1; //no empty Tx mailbox.
	return result;
}
