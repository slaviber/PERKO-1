/*
 * GccApplication5.c
 *
 * Created: 15.01.07 17:52:05 
 *  Author: Administrator
 */ 

//#define STACK_SIZE 112640 

#include <sam4e8c.h>
#include <fastmath.h>

#define IOPORT_PIOD     3
#define IOPORT_CREATE_PIN(port, pin) ((IOPORT_ ## port) * 32 + (pin))
#define LED_0_PIN    IOPORT_CREATE_PIN(PIOD, 24)

#define OPA_EN	IOPORT_CREATE_PIN(PIOD, 10)
#define D_CS	IOPORT_CREATE_PIN(PIOD, 11)
#define D_RW	IOPORT_CREATE_PIN(PIOD, 12)
#define ENABLE_L IOPORT_CREATE_PIN(PIOD, 13)
#define ENABLE_H IOPORT_CREATE_PIN(PIOD, 14)
/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */

typedef unsigned char uchar;
//uchar video_H[1000] = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
//uchar video_L[31000] = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
//#include "header.h"
//const unsigned short indexes[512][240] = {{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30}};
#include "index.h"

//const  uchar sine256[]  = {
	//127,130,133,136,139,143,146,149,152,155,158,161,164,167,170,173,176,178,181,184,187,190,192,195,198,200,203,205,208,210,212,215,217,219,221,223,225,227,229,231,233,234,236,238,239,240,
	//242,243,244,245,247,248,249,249,250,251,252,252,253,253,253,254,254,254,254,254,254,254,253,253,253,252,252,251,250,249,249,248,247,245,244,243,242,240,239,238,236,234,233,231,229,227,225,223,
	//221,219,217,215,212,210,208,205,203,200,198,195,192,190,187,184,181,178,176,173,170,167,164,161,158,155,152,149,146,143,139,136,133,130,127,124,121,118,115,111,108,105,102,99,96,93,90,87,84,81,78,
	//76,73,70,67,64,62,59,56,54,51,49,46,44,42,39,37,35,33,31,29,27,25,23,21,20,18,16,15,14,12,11,10,9,7,6,5,5,4,3,2,2,1,1,1,0,0,0,0,0,0,0,1,1,1,2,2,3,4,5,5,6,7,9,10,11,12,14,15,16,18,20,21,23,25,27,29,31,
	//33,35,37,39,42,44,46,49,51,54,56,59,62,64,67,70,73,76,78,81,84,87,90,93,96,99,102,105,108,111,115,118,121,124
//
//};
//
//const uchar sine32[] = { 16, 19, 22, 24, 27, 29, 30, 31, 31, 31, 30, 29, 27, 24, 22, 19, 16, 12, 9, 7, 4, 2, 1, 0, 0, 0, 1, 2, 4, 7, 9, 12 };
//const uchar sine16[] = { 8, 11, 13, 15, 15, 15, 13, 11, 8, 4, 2, 0, 0, 0, 2, 4 };

Pio* base;
Pio* data;
Uart* pf_uart;
uint32_t mask;
TcChannel *tc_channel;

unsigned char c = 0;

uint8_t buffer[30] = {149, 43, 21, 149, 43, 21, 149, 43, 21, 149, 43, 21, 149, 43, 21, 149, 43, 21, 149, 43, 21, 149, 43, 21, 149, 43, 21, 149, 43, 21};
uint8_t buffer_usart[20] = {55};

static int incr = 0;

Pwm* p_pwm = (Pwm*)(uintptr_t)PWM;

__attribute__((optimize("unroll-loops")))
int main(void)
{
	/* Initialize the SAM system */
	SystemInit();
	
	PMC->PMC_PCER0 = 1 << ID_PIOD; //enable port D
	base = (Pio *)(uintptr_t)PIOD;
    mask = (1U << (LED_0_PIN & 0x1F)) | (1U << (OPA_EN & 0x1F)) | (1U << (D_RW & 0x1F)) | (1U << (D_CS & 0x1F)) | (1U << (ENABLE_L & 0x1F)) | (1U << (ENABLE_H & 0x1F)) ;
	
		//prevent short circuit - set output bits before enabling the output register
		base->PIO_SODR = 1U << (OPA_EN & 0x1F); //enable output
		base->PIO_CODR = 1U << (D_RW & 0x1F); //set to low for write
		
		base->PIO_SODR = 1U << (ENABLE_H & 0x1F); //disable LED HIGH
		base->PIO_SODR = 1U << (ENABLE_L & 0x1F); //disable LED LOW
	
	
	base->PIO_OER = mask | 1023;
	base->PIO_OWER = 1023;
	
	MATRIX->CCFG_SYSIO = CCFG_SYSIO_SYSIO4 | CCFG_SYSIO_SYSIO5 | CCFG_SYSIO_SYSIO6 | CCFG_SYSIO_SYSIO7; //PORTB config disables JTAG
//	
	PMC->PMC_PCER0 = 1 << ID_PIOB; //enable port B
	data = (Pio *)(uintptr_t)PIOB;
	data->PIO_OER = 255;
	data->PIO_OWER = 255;
	

	//base->PIO_MDER = mask;
	//base->PIO_PUER = mask;
	
	
	//PMC->PMC_PCER0 = 1 << ID_PIOA;
	//PIOA->PIO_OER = 1<<9 | 1<<10 | 0b1111111111<<16;
	//PMC->PMC_PCER0 = 1 << ID_PIOB;
	//PIOB->PIO_OER = 0b11111111;
	//
	//
	//
	PMC->PMC_PCER0 = 1 << ID_TC0;
	tc_channel = TC0->TC_CHANNEL + 0;//TC0->TC_CHANNEL[0]
	tc_channel->TC_CCR = TC_CCR_CLKDIS;
	tc_channel->TC_IDR = 0xFFFFFFFF;
	tc_channel->TC_SR;
	tc_channel->TC_CMR = 0 | TC_CMR_CPCTRG;
	tc_channel->TC_IER = TC_IER_CPCS;
	tc_channel->TC_RC = 7813;//4002365 //reg=4000280 (7813=reg/512) (32=reg/512/240)
	tc_channel->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	
	

	
	
	
	
	
	//Pio* uartbase = (Pio *)(uintptr_t)PIOA;
	//uint32_t ul_mask = PIO_PA10A_UTXD0 | PIO_PA9A_URXD0;
	//uartbase->PIO_PDR = ul_mask;
	//uartbase->PIO_IDR = ul_mask;
	//uint32_t ul_sr = uartbase->PIO_ABCDSR[0];
	//uartbase->PIO_ABCDSR[0] &= (~ul_mask & ul_sr);
	//ul_sr = uartbase->PIO_ABCDSR[1];
	//uartbase->PIO_ABCDSR[1] &= (~ul_mask & ul_sr);
//
	//PMC->PMC_PCER0 = (1 << ID_UART0);
	//pf_uart = (Uart*)(uintptr_t)UART0;
	//pf_uart->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;
	//pf_uart->UART_BRGR = 2;
	//pf_uart->UART_MR = UART_MR_PAR_NO;
	//pf_uart->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;
	//pf_uart->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
	//
	//
	//Pdc* uart = (Pdc *)(uintptr_t)PDC_UART0;
	//uart->PERIPH_RPR = (uint32_t)buffer;
	//uart->PERIPH_RCR = 200;
	//uart->PERIPH_PTCR = PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN;
	//pf_uart->UART_IER = UART_IER_RXBUFF;
	
	
	
	
	
	
	
	
	
	
	
	//Pio* usartbase = (Pio *)(uintptr_t)PIOB;
	//uint32_t ul2_mask = PIO_PB1C_TXD0;
	//usartbase->PIO_PDR = ul2_mask;
	//usartbase->PIO_IDR = ul2_mask;
	//uint32_t ul_sr2 = usartbase->PIO_ABCDSR[0];
	//usartbase->PIO_ABCDSR[0] &= (~ul_mask & ul_sr2);
	//ul_sr2 = usartbase->PIO_ABCDSR[1];
	//usartbase->PIO_ABCDSR[1] &= (~ul_mask & ul_sr2);
	//
	//PMC->PMC_PCER0 = (1 << ID_USART0);
	//Usart* pf_usart = (Usart*)(uintptr_t)USART0;
	//pf_usart->US_WPMR = US_WPMR_WPKEY_PASSWD;
	//pf_usart->US_MR = 0;
	//pf_usart->US_RTOR = 0;
	//pf_usart->US_TTGR = 0;
	//pf_usart->US_CR = US_CR_RSTTX | US_CR_TXDIS;
	//pf_usart->US_CR = US_CR_RSTRX | US_CR_RXDIS;
	//pf_usart->US_CR = US_CR_RSTSTA;
	//pf_usart->US_CR = US_CR_RTSDIS;
	//pf_usart->US_CR = US_CR_DTRDIS;
	////pf_usart->US_MR |= US_MR_OVER; //8x
	//pf_usart->US_BRGR = (2 << US_BRGR_CD_Pos) | (0 << US_BRGR_FP_Pos);
	//pf_usart->US_MR |= US_MR_CHRL_8_BIT | US_MR_PAR_NO | US_MR_CHMODE_NORMAL | US_MR_NBSTOP_1_BIT;
	//pf_usart->US_IDR = 0xffffffff;
	//pf_usart->US_CR = US_CR_TXEN;
	//pf_usart->US_CR = US_CR_RXEN;
	//
	//Pdc* usart = (Pdc *)(uintptr_t)PDC_USART0;
	//usart->PERIPH_PTCR = PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN;
	//usart->PERIPH_TPR = (uint32_t)buffer_usart;
	//usart->PERIPH_TCR = 20;
	//pf_usart->US_IER = US_IER_TXBUFE;
	
	
	//
	//
	//PMC->PMC_PCER0 = 1 << ID_PIOD;
	//base = (Pio *)(uintptr_t)PIOD;
	//mask = 1U << (LED_0_PIN & 0x1F);
	//base->PIO_PDR = mask;
	//base->PIO_IDR = mask;
	//
	//
	//int PWMs = ID_PWM-32;
	//PMC->PMC_PCER1 = (1 << PWMs);
	//p_pwm->PWM_DIS = (1 << 0); //channel 0
	//p_pwm->PWM_CLK = 0; //no divider
	//uint32_t 	tmp_reg =
	//0 |
	//(1 << 9) |
	//(0) |
	//(0 << 16) |
	//(0 << 17) |
	//(0 << 18) |
	//(0 << 8);
	////no prescaler
	////polarity high
	////counter event at end
	////no dead time
	////pwmh not inverted
	////pwml not inverted
	////left alignment
	//
	//p_pwm->PWM_CH_NUM[0].PWM_CMR = tmp_reg;
	//
	//p_pwm->PWM_CH_NUM[0].PWM_CDTY = 8;
	//p_pwm->PWM_CH_NUM[0].PWM_CPRD = 15;
	//
	//
	//
	//
	//
	//p_pwm->PWM_IER1 = (1 << 0) | (1 << (0 + 16));
//
	///* Configure interrupt and enable PWM interrupt */
	//NVIC_DisableIRQ(PWM_IRQn);
	//NVIC_ClearPendingIRQ(PWM_IRQn);
	////NVIC_SetPriority(PWM_IRQn, 0);
	//NVIC_EnableIRQ(PWM_IRQn);
	//
	//p_pwm->PWM_ENA = (1 << 0); //channel 0
//
//
//
//
	//
	//
	
	
	
	
	










	NVIC_DisableIRQ(ID_TC0);
	NVIC_DisableIRQ(UART0_IRQn);
	NVIC_DisableIRQ(USART0_IRQn);
	
	NVIC_DisableIRQ(PWM_IRQn);
	
	NVIC_ClearPendingIRQ(ID_TC0);
	NVIC_SetPriority(ID_TC0,0);
	//NVIC_ClearPendingIRQ(UART0_IRQn);
	//NVIC_SetPriority(UART0_IRQn,1);
	//NVIC_ClearPendingIRQ(USART0_IRQn);
	//NVIC_SetPriority(USART0_IRQn,0);
	NVIC_EnableIRQ(ID_TC0);
	//NVIC_EnableIRQ(UART0_IRQn);
	//NVIC_EnableIRQ(USART0_IRQn);
	//short num = 0;
	//char even = 0;
	//int a = 0;
	
		base->PIO_SODR = 1U << (OPA_EN & 0x1F); //enable output
		base->PIO_CODR = 1U << (D_RW & 0x1F); //set to low for write
		
		base->PIO_SODR = 1U << (ENABLE_H & 0x1F); //disable LED HIGH
		base->PIO_SODR = 1U << (ENABLE_L & 0x1F); //disable LED LOW
		base->PIO_CODR = 1023; //reset output data
		
		base->PIO_CODR = 1U << (ENABLE_H & 0x1F); //enable LED HIGH
		base->PIO_CODR = 1U << (ENABLE_L & 0x1F); //enable LED LOW
		
		
	//unsigned int i = 0;
	//unsigned int counter = 0;
	//unsigned int led = 0;
	//
	while(1){
		
	}
	//while (1)
	//{
		//++counter;
		//counter %= 800;
		//if(!counter)++i;
		//if(!i){
		//	++led;
		//	led %= 12;
		//}
		//i %= 1024;
		
		//num = 0;
		//incr = 0;
		//for(int g = 0; g < 2; ++g){
			//int f = 0;
			//while(f < 15){
				//short C = buffer[indexes[num][incr++]] - 16;
				//short D = buffer[indexes[num][incr++]] - 128;
				//short E = buffer[indexes[num][incr++]] - 128;
				//
				//if(g == f)++f;
				//
				//base->PIO_SODR = 1U << (ENABLE_H & 0x1F); //disable LED HIGH
				//base->PIO_SODR = 1U << (ENABLE_L & 0x1F); //disable LED LOW
				//data->PIO_CODR = 255;
				//data->PIO_SODR = g | f<<4;
				//base->PIO_CODR = 1U << (ENABLE_H & 0x1F); //enable LED HIGH
				//base->PIO_CODR = 1U << (ENABLE_L & 0x1F); //enable LED LOW
				//
				////RED
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				//base->PIO_CODR = 1023; //reset output data
				//base->PIO_SODR = 512; //set output data
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				//
				//++f;
				//if(g == f)++f;
				//base->PIO_SODR = 1U << (ENABLE_H & 0x1F); //disable LED HIGH
				//base->PIO_SODR = 1U << (ENABLE_L & 0x1F); //disable LED LOW
				//data->PIO_CODR = 255;
				//data->PIO_SODR = g | f<<4;
				//base->PIO_CODR = 1U << (ENABLE_H & 0x1F); //enable LED HIGH
				//base->PIO_CODR = 1U << (ENABLE_L & 0x1F); //enable LED LOW
				//
				////GREEN
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				//base->PIO_CODR = 1023; //reset output data
				//base->PIO_SODR = 0; //set output data
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				//
				//++f;
				//if(g == f)++f;
				//base->PIO_SODR = 1U << (ENABLE_H & 0x1F); //disable LED HIGH
				//base->PIO_SODR = 1U << (ENABLE_L & 0x1F); //disable LED LOW
				//data->PIO_CODR = 255;
				//data->PIO_SODR = g | f<<4;
				//base->PIO_CODR = 1U << (ENABLE_H & 0x1F); //enable LED HIGH
				//base->PIO_CODR = 1U << (ENABLE_L & 0x1F); //enable LED LOW
				//
				////BLUE
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				//base->PIO_CODR = 1023; //reset output data
				//base->PIO_SODR = 0; //set output data
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				//
			//}
		//}
		//}
		//base->PIO_CODR = 1U << (ENABLE_H & 0x1F); //enable LED HIGH
		//base->PIO_CODR = 1U << (ENABLE_L & 0x1F); //enable LED LOW
		//
		//incr = 0;
		//for(int g = 0; g < 2; ++g){
			//for(int f = 0; f < 16; ++f){
				//
				//
				//signed int C = 235 - 16;
				//signed int D = 128 - 128;
				//signed int E = 128 - 128;
				//
				////if(g == f)++f;
//
				////data->PIO_CODR = 255;
				////data->PIO_SODR = g | f<<4;
//
				//
				////RED
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				////base->PIO_CODR = 1023; //reset output data
				////base->PIO_SODR = 260+(( 298 * C           + 409 * E + 128) >> 8)*2.89f; //set output data
				//base->PIO_ODSR = 260+(( 298 * C           + 409 * E + 128) >> 8)*2.89f;
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				//
				////++f;
				//if(g == f)++f;
//
				////data->PIO_CODR = 255;
				////data->PIO_SODR = g | f<<4;
				//data->PIO_ODSR = g | f<<4;
				//
				////GREEN
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				////base->PIO_CODR = 1023; //reset output data
				////base->PIO_SODR = 315+(( 298 * C - 100 * D - 208 * E + 128) >> 8)*1.11f; //set output data
				//base->PIO_ODSR = 315+(( 298 * C - 100 * D - 208 * E + 128) >> 8)*1.11f;
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				//
				//++f;
				//if(g == f)++f;
				//
				////data->PIO_CODR = 255;
				////data->PIO_SODR = g | f<<4;
				//data->PIO_ODSR = g | f<<4;
				//
				////BLUE
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				////base->PIO_CODR = 1023; //reset output data
				////base->PIO_SODR = 350+(( 298 * C + 516 * D           + 128) >> 8)*1.36f; //set output data
				//base->PIO_ODSR = 350+(( 298 * C + 516 * D           + 128) >> 8)*1.36f;
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				//
				//++f;
				//if(g == f)++f;
				//
				////data->PIO_CODR = 255;
				////data->PIO_SODR = g | f<<4;
				//data->PIO_ODSR = g | f<<4;
				//
				////RESET equal
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				//base->PIO_CODR = 1023; //reset output data
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
//
				//////RED
				////base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				////base->PIO_CODR = 1023; //reset output data
				////base->PIO_SODR = buffer[indexes[0][incr++]]*4; //set output data
				////base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				////
				////data->PIO_CODR = 255;
				////data->PIO_SODR = g | i<<4;
			//}
		//}

				
			//}
		//}
		
		/*
				base->PIO_SODR = mask;
				asm("nop");
				asm("nop");
				asm("nop");
				for(int i = 0; i < 15; ++i){
					asm("nop");
				}
				base->PIO_CODR = mask;
				for(int i = 0; i < 15; ++i){
					asm("nop");
				}
		*/
		
		
		
		
		//while (!(UART0->UART_SR & UART_SR_TXRDY));
		//pf_uart->UART_THR = 'z';
		//while(!(UART0->UART_SR & UART_SR_RXRDY));
		//pf_uart->UART_THR = (uint8_t) pf_uart->UART_RHR;
		
		
		//for(int i = 0; i < 100; ++i){
		//	array[i] *= 10;
		//	array[99-i] = i;
		//	array[i] = array[i]*array[99-i];
		//}
		
		
		
		
		//base->PIO_CODR = mask;
		//float f = sin(0.5f);
		//char a = array[0];
		//even = !even;
		//++a;
		//a = a%100;
		//if(even){
		//base->PIO_SODR = mask;
			//////base->PIO_OER = mask;
			//////base->PIO_SODR = mask;
			//////base->PIO_SODR = mask;
			//asm("nop");
			//asm("nop");
			//asm("nop");
			//asm("nop");
			//asm("nop");
			//asm("nop");
			//asm("nop");
			//asm("nop");
			//
			//
			//for(int i = 0; i < 15; ++i){
				//asm("nop");
			//}
////
		//////}
		//////else {
		//base->PIO_CODR = mask;
			//////base->PIO_ODR = mask;
			//////base->PIO_CODR = mask;
			//////base->PIO_CODR = mask;
			//for(int i = 0; i < 15; ++i){
				//asm("nop");
			//}
			
		//}

		//++num;
		//num %= 512;
		
		//short C;
		//short D;
		//short E;
		//
		//incr = 0;
		////256 everywhere + *3!
		////green = 225
		////max 2ma???
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOB->PIO_CODR = 15;
		//PIOB->PIO_SODR = 1;
		//C = video_H[indexes[num][incr++]] - 16;
		//D = video_H[indexes[num][incr++]] - 128;
		//E = video_H[indexes[num][incr++]] - 128;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C           + 409 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C - 100 * D - 208 * E + 128))<<16;
		//PIOB->PIO_CODR = 15<<4;
		//PIOB->PIO_SODR = 1<<4;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//PIOA->PIO_SODR = (205+( 298 * C + 516 * D           + 128))<<16;
		//PIOA->PIO_CODR = 0b1111111111<<16;
		//
		
			
			
    //}
	
}




void PWM_Handler(void)
{
	//static uint32_t ul_count = 0;  /* PWM counter value */
	//static uint32_t ul_duty = 128;  /* PWM duty cycle rate */
	//static uint8_t fade_in = 1;  /* LED fade in flag */
	static uchar sin;
	uint32_t events = p_pwm->PWM_ISR1;

	/* Interrupt on PIN_PWM_LED0_CHANNEL */
	if ((events & (1 << 0)) ==
	(1 << 0)) {
		++sin;
		sin %= 16;
		p_pwm->PWM_CH_NUM[0].PWM_CDTYUPD = 0; // sine16[sin]; //
		//ul_count++;
//
		///* Fade in/out */
		//if (ul_count == (500000 / (255 - 128))) {
			///* Fade in */
			//if (fade_in) {
				//ul_duty++;
				//if (ul_duty == 255) {
					//fade_in = 0;
				//}
				//} else {
				///* Fade out */
				//ul_duty--;
				//if (ul_duty == 128) {
					//fade_in = 1;
				//}
			//}
//
			///* Set new duty cycle */
			//ul_count = 0;
			//p_pwm->PWM_CH_NUM[0].PWM_CDTYUPD = ul_duty;
		//}
	}

}


void TC0_Handler(void){
		(void)(tc_channel->TC_SR);
		
		//static int row = 0;
		//static int pixel = 0;
		//if(!pixel){
			//++row;
			//row %= 512;
		//}
		//
		//++pixel;
		//pixel %= 240;
		

		
		
		//static uint8_t n_pixel = 0;
		//++n_pixel;
		//if(n_pixel/16 == n_pixel%16)++n_pixel; //invalid bus combinations
		
		
		
		//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
		
		//uint8_t Ydata = hbuf[led_index[row*80+pixel/3] + offset[row*80+pixel/3]];
		//int16_t C = Ydata - 16;
		//base->PIO_ODSR = ((298*C)>>8)*3.9; //B/W only
		//if(row == 10)base->PIO_ODSR = 1023;
		//else base->PIO_ODSR = 0;
		
		//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data

		//data->PIO_ODSR = (n_pixel/16) | (n_pixel%16)<<4;
		
		
		
		
		
		static int row = 0;
		uint8_t n_pixel = 0;
		for(int i = 0; i < 80; ++i){
			
			int pos = row*80+i;
			int lind = led_index[pos];
			
			uint16_t Ydata = (hbuf[lind + offset[pos]] - 16)*298;
			uint16_t Udata = hbuf[lind] - 128;
			uint16_t Vdata = hbuf[lind+1] - 128;
			
			
			base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
			base->PIO_ODSR = (( Ydata           + 409 * Vdata + 128) >> 6);
			base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
			
			++n_pixel;
			if(n_pixel/16 == n_pixel%16)++n_pixel; //invalid bus combinations

			data->PIO_ODSR = (n_pixel/16) | (n_pixel%16)<<4;
			


			base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
			base->PIO_ODSR = (( Ydata - 100 * Udata - 208 * Vdata + 128) >> 6);
			base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
			
			++n_pixel;
			if(n_pixel/16 == n_pixel%16)++n_pixel; //invalid bus combinations

			data->PIO_ODSR = (n_pixel/16) | (n_pixel%16)<<4;
			
			
			
			base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
			base->PIO_ODSR = (( Ydata + 516 * Udata           + 128) >> 6);
			base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
			
			++n_pixel;
			if(n_pixel/16 == n_pixel%16)++n_pixel; //invalid bus combinations

			data->PIO_ODSR = (n_pixel/16) | (n_pixel%16)<<4;
			
			base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
			base->PIO_ODSR = 0; //reset
			base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
			
		}
		++row;
		row %= 512;
		
		//static int row = 0;
		//row++;
		//row %= 512;
		////c = !c;
		////
		////if(!c){
		////	base->PIO_CODR = mask;
		////}
		////else{
		////	base->PIO_SODR = mask;
		////}
		//base->PIO_CODR = 1U << (ENABLE_H & 0x1F); //enable LED HIGH
		//base->PIO_CODR = 1U << (ENABLE_L & 0x1F); //enable LED LOW
		//
		//incr = 0;
		//for(int g = 0; g < 2; ++g){
			//char first = 1;
			//for(int f = 0; f < 16; ++f){
				//
				//uint8_t R = row%255;
				//uint8_t G = row%255;
				//uint8_t B = row%255;
				//
				//uint8_t Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
				//uint8_t U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
				//uint8_t V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;
				//
				//signed int C = Y - 16;
				//signed int D = U - 128;
				//signed int E = V - 128;
				//
//
				//
				////RED
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				//base->PIO_ODSR = (( 298 * C           + 409 * E + 128) >> 8)*4;
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				//
				////++f;
				//if(g == f)++f;
//
				//data->PIO_ODSR = g | f<<4;
				//
				//if(first){
					//first = 0;
					////wait for the current to swap
					//__asm("nop");
					//__asm("nop");
					//__asm("nop");
					//__asm("nop");
					//__asm("nop");
					//__asm("nop");
					//__asm("nop");
					//__asm("nop");
					//__asm("nop");
					//__asm("nop");
					//
				//}
				//
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
	//
				////GREEN
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				//base->PIO_ODSR = (( 298 * C - 100 * D - 208 * E + 128) >> 8)*4;
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				//
				//++f;
				//if(g == f)++f;
				//
				//data->PIO_ODSR = g | f<<4;
				//
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
				//
				////BLUE
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				//base->PIO_ODSR = (( 298 * C + 516 * D           + 128) >> 8)*4;
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				//
				//++f;
				//if(g == f)++f;
				//
				//data->PIO_ODSR = g | f<<4;
				//
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
//__asm("nop");
				//
				////RESET equal
				//base->PIO_CODR = 1U << (D_CS & 0x1F); //set CS to low when loading data
				//base->PIO_CODR = 1023; //reset output data
				//base->PIO_SODR = 1U << (D_CS & 0x1F); //set CS to high and flush data
				//
			//}
		//}
		//
		//
		//
		////base->PIO_SODR = 1U << (ENABLE_H & 0x1F); //disable LED HIGH
		////base->PIO_SODR = 1U << (ENABLE_L & 0x1F); //disable LED LOW
		//
		//
		//
		
}

void UART0_Handler(void)
{
	if ((UART0->UART_SR & UART_SR_RXBUFF) == UART_SR_RXBUFF) {
		for(int i = 0; i < 200; ++i){
			while (!(UART0->UART_SR & UART_SR_TXRDY));
			UART0->UART_THR = buffer[i];
		}
		PDC_UART0->PERIPH_RPR = (uint32_t)buffer;
		PDC_UART0->PERIPH_RCR = 200;
	}
}

void USART0_Handler(void)
{
	if ((USART0->US_CSR & US_CSR_TXBUFE) == US_CSR_TXBUFE) {
		PDC_USART0->PERIPH_TPR = (uint32_t)buffer_usart;
		PDC_USART0->PERIPH_TCR = 20;
	}
}
