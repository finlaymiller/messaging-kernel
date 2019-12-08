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
#include "train_phy.h"

/*  globals */
volatile char data_rx;
volatile int got_data;

/*
 * Initializes UART0 by setting up UART registers
 * 8 data bits, no parity, one stop bit
 * Baud rate = 115200
 */
void initUART(void)
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

	UART0_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupt
}

void initUART1(void)
{
    volatile int wait;

    /* Initialize UART1 */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCUART_GPIOB;
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCGPIO_UART1;
    wait = 0;   // wait required before accessing the UART config regs

    UART1_CTL_R &= ~UART_CTL_UARTEN;        // Disable the UART
    wait = 0;   // wait required before accessing the UART config regs

    // Setup the BAUD rate
    UART1_IBRD_R = 8;   // IBRD = int(16,000,000 / (16 * 115,200)) = 8.680555555555556
    UART1_FBRD_R = 44;  // FBRD = int(.680555555555556 * 64 + 0.5) = 44.05555555555556

    UART1_LCRH_R = (UART_LCRH_WLEN_8);  // WLEN: 8, no parity, one stop bit, without FIFOs)

    GPIO_PORTB_AFSEL_R = 0x3;        // Enable Receive and Transmit on PA1-0
    GPIO_PORTB_PCTL_R = (0x01) | ((0x01) << 4);         // Enable UART RX/TX pins on PA1-0
    GPIO_PORTB_DEN_R = EN_DIG_PB0 | EN_DIG_PB1;        // Enable Digital I/O on PA1-0

    UART1_CTL_R = UART_CTL_UARTEN;        // Enable the UART
    wait = 0;   // wait required before accessing the UART config regs

    InterruptEnable(INT_VEC_UART1);

    UART1_IntEnable(UART_INT_RX | UART_INT_TX); // Enable Receive and Transmit interrupts

    InterruptMasterEnable();
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
 * Enables interrupts for UART1
 */
void UART1_IntEnable(unsigned long flags)
{
    /* Set specified bits for interrupt */
    UART1_IM_R |= flags;
}

/*
 * Interrupt service routine for UART1
 * Handles all TX and RX interrupts for UART1
 */
void UART1_IntHandler(void)
{
    // Receiving character
    InterruptMasterDisable();
    if (UART1_MIS_R & UART_INT_RX)
    {
        /* RECV done - clear interrupt and make char available to application */
        UART1_ICR_R |= UART_INT_RX;

        /* send data to data_rx variable and set data received flag */
        data_rx = UART1_DR_R;
    }

    // Transmitting character
    if (UART1_MIS_R & UART_INT_TX)
    {
        /* XMIT done - clear interrupt */
        UART1_ICR_R |= UART_INT_TX;
    }
    InterruptMasterEnable();
}

/*
 * Interrupt service routine for UART0
 * Handles all TX and RX interrupts for UART0
 */
void UART0_IntHandler(void)
{
	// Receiving character
	InterruptMasterDisable();
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
	InterruptMasterEnable();
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
 * This function makes it easier to transmit an entire string via UART1
 *
 * @param   string: The string to be transmitted
 * @returns:        None
 */
void UART1_TXStr(char *string, char len)
{
    int i;
    for(i=0; i<len; i++){

        /* Check if byte needs to be preceeded by DLE */
        //TODO: clean this long check up
        if(string[i] == DLE || string[i] == STX || string[i] == ETX){
            if((i != len-1) && (i != 0)){
                /* Transmit DLE if necessary */
                UART1_TXChar(DLE);
            }
        }

        /* Transmit byte on UART1 */
        UART1_TXChar(string[i]);
    }
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

void UART1_TXChar(char data)
{
    while(!UART1_TXReady());    // wait till UART0 is ready
    UART1_DR_R = data;          // send character to UART0 data register
}

int UART0_TXReady(void)
{
	// 1 if ready, 0 if busy
	return !(UART0_FR_R & UART_FR_BUSY);
}

int UART1_TXReady(void)
{
    // 1 if ready, 0 if busy
    return !(UART1_FR_R & UART_FR_BUSY);
}
