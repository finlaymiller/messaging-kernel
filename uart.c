/*
 * uart.c
 *
 * Defines UART initialization and handler functions
 *
 *  Created on: Sep 19, 2019
 *  Author: Derek Capone
 */

#include "queue.h"
#include "uart.h"

/*  globals */
volatile char data_rx;
volatile int got_data;

/*
 * Initializes UART0 by setting up UART registers
 * 8 data bits, no parity, one stop bit
 * Baud rate = 115200
 */
void UART0_Init(void)
{
    volatile int wait;

    /* Initialize UART0 */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOA;   // Enable Clock Gating for UART0
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART0;   // Enable Clock Gating for PORTA
    wait = 0; // give time for the clocks to activate

    UART0_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    UART0_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART0_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556

    UART0_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    GPIO_PORTA_AFSEL_R = 0x3;        // Enable Receive and Transmit on PA1-0
    GPIO_PORTA_PCTL_R = (0x01) | ((0x01) << 4);         // Enable UART RX/TX pins on PA1-0
    GPIO_PORTA_DEN_R = EN_DIG_PA0 | EN_DIG_PA1;        // Enable Digital I/O on PA1-0

    UART0_CTL_R = UART_CTL_UARTEN;        // Enable the UART
    wait = 0; // wait; give UART time to enable itself.

    InterruptEnable(INT_VEC_UART0);       		// Enable UART0 interrupts
	UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts
	INTERRUPT_MASTER_ENABLE();
}

/*
 * Enables interrupts for UART0
 */
void UART0_IntEnable(unsigned long flags)
{
    /* Set specified bits for interrupt */
    UART0_IM_R |= flags;
}

void InterruptEnable(unsigned long InterruptIndex)
{
    /* Indicate to CPU which device is to interrupt */
    if(InterruptIndex < 32)
        NVIC_EN0_R = 1 << InterruptIndex;       // Enable the interrupt in the EN0 Register
    else
        NVIC_EN1_R = 1 << (InterruptIndex - 32);    // Enable the interrupt in the EN1 Register
}

/*
 * Interrupt service routine for UART0
 * Handles all TX and RX interrupts for UART0
 */
void UART0_IntHandler(void)
{
	// Receiving character
	INTERRUPT_MASTER_DISABLE();
	if (UART0_MIS_R & UART_INT_RX)
	{
		/* RECV done - clear interrupt and make char available to application */
		UART0_ICR_R |= UART_INT_RX;

		/* send data to data_rx variable and set data received flag */
		data_rx = UART0_DR_R;
		//got_data = TRUE;

		enqueue(UART_RX, data_rx);	// send to RX queue
	}

	// Transmitting character
	if (UART0_MIS_R & UART_INT_TX)
	{
		/* XMIT done - clear interrupt */
		UART0_ICR_R |= UART_INT_TX;

		/* transmit char if one is available */
		if(!isQEmpty(UART_TX))
			UART0_TXChar(dequeue(UART_TX));
	}
	INTERRUPT_MASTER_ENABLE();
}

/*
 * This function makes it easier to transmit an entire string via UART
 *
 * @param	string: The string to be transmitted
 * @returns:		None
 */
void UART0_TXStr(char *string)
{
	unsigned int len = strlen(string);
	unsigned int i = 0;

	while(i < len)
		UART0_TXChar(string[i++]);
}

/*
 * This function makes it easier to transmit a character via UART
 *
 * @param	data:	The character to be transmitted
 * @returns:		None
 */
void UART0_TXChar(char data)
{
    while(!UART0_TXReady());	// wait till UART0 is ready
    UART0_DR_R = data;			// send character to UART0 data register
}

int UART0_TXReady(void)
{
	// 1 if ready, 0 if busy
	return !(UART0_FR_R & UART_FR_BUSY);
}

void UART_force_out_char(char c)
{
    UART0_DR_R = c;
}

