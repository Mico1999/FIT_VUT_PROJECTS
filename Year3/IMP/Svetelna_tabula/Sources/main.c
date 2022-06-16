/* Header file with all the essential definitions for a given type of MCU */
#include "MK60DZ10.h"


/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))

/* Macro for deleting actual column on matrix... all rows will be shut down */
#define DEL() PTA->PDOR &= GPIO_PDOR_PDO(0x00000000)

/* Constants specifying delay loop duration */
#define	tdelay1			1000
#define tdelay2 		4

/* Each letter on LED display has constant width (spaces included)*/
#define letter_len 5

/* Masks for button pins which will be used for changing text on LED display */
#define BTN_SW2 0x400 		//< Port E, pin 10
#define BTN_SW3 0x1000 		//< Port E, pin 12
#define BTN_SW4 0x8000000 	//< Port E, pin 27
#define BTN_SW5 0x4000000 	//< Port E, pin 26

/* Each message printed on display has id */
#define HI_TXT 1
#define HEY_TXT 2
#define BYE_TXT 3
#define END_TXT 4

/* Array with pin numbers of port B...will be used to init this pins */
int BTN_Pin_Array[4] = {10, 12, 27, 26};

/* Flags which indicate that user pushed one of the buttons */
int bnt_sw2_pressed = 0, bnt_sw3_pressed = 0, bnt_sw4_pressed = 0, bnt_sw5_pressed = 0;
int switch_text = 0;

/* Each array defines for specific letter which rows will be shining in each column...each value will be stored to PTA->PDOR */
int H_row_def_array[4] = {0x3F000FC0, 0x10000D40, 0x10000D40, 0x3F000FC0};
int I_row_def_array[5] = {0x24000D40, 0x24000D40, 0x3F000FC0, 0x24000D40, 0x24000D40};
int B_row_def_array[4] = {0x3F000FC0, 0x36000D40, 0x36000D40, 0x09000FC0};
int E_row_def_array[4] = {0x3F000FC0, 0x34000D40, 0x34000D40, 0x34000D40};
int Y_row_def_array[5] = {0x04000D40, 0x01000D40, 0x3A000FC0, 0x01000D40, 0x04000D40};


/**
 * @brief Configuration of the necessary MCU peripherals
 */
void SystemConfig() {
	/* Turn on all port clocks */
	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK;


	/* Set corresponding PTA pins (column activators of 74HC154) for GPIO functionality */
	PORTA->PCR[8] = ( 0|PORT_PCR_MUX(0x01) );  // A0
	PORTA->PCR[10] = ( 0|PORT_PCR_MUX(0x01) ); // A1
	PORTA->PCR[6] = ( 0|PORT_PCR_MUX(0x01) );  // A2
	PORTA->PCR[11] = ( 0|PORT_PCR_MUX(0x01) ); // A3

	/* Set corresponding PTA pins (rows selectors of 74HC154) for GPIO functionality */
	PORTA->PCR[26] = ( 0|PORT_PCR_MUX(0x01) );  // R0
	PORTA->PCR[24] = ( 0|PORT_PCR_MUX(0x01) );  // R1
	PORTA->PCR[9] = ( 0|PORT_PCR_MUX(0x01) );   // R2
	PORTA->PCR[25] = ( 0|PORT_PCR_MUX(0x01) );  // R3
	PORTA->PCR[28] = ( 0|PORT_PCR_MUX(0x01) );  // R4
	PORTA->PCR[7] = ( 0|PORT_PCR_MUX(0x01) );   // R5
	PORTA->PCR[27] = ( 0|PORT_PCR_MUX(0x01) );  // R6
	PORTA->PCR[29] = ( 0|PORT_PCR_MUX(0x01) );  // R7

	/* Set corresponding PTE pins (output enable of 74HC154) for GPIO functionality */
	PORTE->PCR[28] = ( 0|PORT_PCR_MUX(0x01) ); // #EN


	/* Change corresponding PTA port pins as outputs */
	PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0);

	/* Change corresponding PTE port pins as outputs */
	PTE->PDDR = GPIO_PDDR_PDD( GPIO_PIN(28) );

	/* Configuration of pins for buttons which will be used to change message on display */
	for(int i=0; i<4; i++) {
		int btn_pin = (BTN_Pin_Array[i]);

			PORTE->PCR[btn_pin] = ( PORT_ISFR_ISF(0x01)/* Make ISF zero (Interrupt Status Flag) */
					| PORT_PCR_IRQC(0x0A) /* Interrupt enable on failing edge */
					| PORT_PCR_MUX(0x01)); /* Pin Mux Control to GPIO */
	}

	NVIC_ClearPendingIRQ(PORTE_IRQn); /* Clear pending interrupts from port E */
	NVIC_EnableIRQ(PORTE_IRQn);       /* Enable interrupt from port E */
}

/**
 * @brief Configuration of Periodic Interrupt Timer
 */
void PIT_Init()
{
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;

	/* enable clocking mode and do not stop timer in debug mode */
	PIT->MCR &= 0x0u;

	/* Initialize PIT0 to count down from argument */
	PIT->CHANNEL[0].LDVAL =  0x2FAF07FF; 	//< trigger interrupt each 20 seconds ((20*40 MHz) - 1)

	/* No chaining */
	PIT->CHANNEL[0].TCTRL &= 0x0u;

	/* Generate interrupts */
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;

	/* Enable Interrupts */
	NVIC_SetPriority(PIT0_IRQn, 3);
	NVIC_ClearPendingIRQ(PIT0_IRQn);
	NVIC_EnableIRQ(PIT0_IRQn);

}

/**
 * @brief Reset PIT when some button was pushed to count again from start value
 */
void reset_PIT_timer() {

	/*Stop the timer channel */
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;

	/*Start the timer channel*/
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}

/**
 * @brief Delay loop called between shining of columns
 */
void delay(int t1, int t2)
{
	int i, j;

	for(i=0; i<t1; i++) {
		for(j=0; j<t2; j++);
	}
}

/**
 * @brief Delay loop to avoid multiple closure and opening of contact of button
 */
void button_delay(uint64_t bound) {
	for (uint64_t i=0; i < bound; i++) { __NOP(); }
}


/**
 * @brief Conversion of requested column number into the 4-to-16 decoder control
 */
void column_select(unsigned int col_num)
{
	unsigned i, result, col_sel[4];

	for (i =0; i<4; i++) {
		result = col_num / 2;	  // Whole-number division of the input number
		col_sel[i] = col_num % 2;
		col_num = result;

		switch(i) {

			// Selection signal A0
		    case 0:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(8))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(8)));
				break;

			// Selection signal A1
			case 1:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(10))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(10)));
				break;

			// Selection signal A2
			case 2:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(6))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(6)));
				break;

			// Selection signal A3
			case 3:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(11))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(11)));
				break;

			// Otherwise nothing to do...
			default:
				break;
		}
	}
}

/**
 * @brief Writes to port A output register which rows on LED will be shining
 * @param row_selector Value to be written to port A output register
 */
void row_select(int row_selector) {

	PTA->PDOR = GPIO_PDOR_PDO(row_selector);

}

/**
 * @brief Writes letter on display
 * @param letter_row_array[] Specifies rows of actual letter, values of array represent which rows will be shining in each column
 * @param column Column number from which the writing will start
 * @param arr_len Len of letter_row_array, which differs for letters
 */
void write_letter(int letter_row_array[], int column, int arr_len) {

	for (int i = 0; i < arr_len; i++) {

		// if user push some button then end function and switch off column
		if (switch_text == 1){
			DEL();
			return;
		}

		// switch on column
		row_select(letter_row_array[i]);
		column_select(column + i);
		PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28) );
		delay(20, 20);
		PTE->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	}
}

/**
 * @brief Writes HI messages on display
 * @param column Column number from which the writing will start
 */
void print_hi(int column) {

	write_letter(H_row_def_array, column, 4);

	write_letter(I_row_def_array, column + letter_len, 5);
}

/**
 * @brief Writes HEY messages on display
 * @param column Column number from which the writing will start
 */
void print_hey(int column) {

	write_letter(H_row_def_array, column, 4);

	write_letter(E_row_def_array, column + letter_len, 4);

	write_letter(Y_row_def_array, column + 2*letter_len, 5);
}

/**
 * @brief Writes BYE messages on display
 * @param column Column number from which the writing will start
 */
void print_bye(int column) {

	write_letter(B_row_def_array, column, 4);

	write_letter(Y_row_def_array, column + letter_len, 5);

	write_letter(E_row_def_array, column + 2*letter_len, 4);
}

/**
 * @brief Simulates ending of writing on display.
 * 		  Whole display will be shining and no text will be displayed.
 */
void finish_writing() {

	for (int i = 15; i >=0; i--) {

		// if user push some button then end function and switch off column
		if (switch_text == 1)
		{
			DEL();
			switch_text = 0;
			return;
		}

		// switch on all rows and columns
		row_select(0x3F000280);
		column_select(i);
		PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28) );
		delay(tdelay1/10, tdelay2);
		PTE->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	}
}

/**
 * @brief Delays calling of functions for writing messages on display
 * 		  After whole message is written on display, there is a delay to slow down moving of message across display
 * @param void (*fun)(int) Function to be delayed
 * @param column Column number from which the writing will start
 */
void DelayFunction( void (*fun)(int), int column)
{

	int i, j;

	for(i=0; i<15; i++)
	{
		for(j=0; j<15; j++)
		{
			fun(column);

			/* if user pushed some button break this delay to not wait too long for switching message*/
			if (switch_text == 1) return;
		}
	}

}

/**
 * @brief Simulates starting of writing on display.
 * @param txt_id Defines which message will be written on display (HI | HEY | BYE)
 */
void start_writing(int txt_id) {

	for (int i = 15; i >=0; i--)
	{
		/* if user pushed some button break this delay to not wait too long for switching message*/
		if (switch_text == 1) {
			switch_text = 0;
			return;
		}

		/* choose message which will be written on display */
		switch(txt_id)
		{
			case  HI_TXT:
				DelayFunction(print_hi, i);
				break;

			case  HEY_TXT:
				DelayFunction(print_hey, i);
				break;

			case  BYE_TXT:
				DelayFunction(print_bye, i);
				break;

			default:
				break;
		}
	}
}

/**
 *@brief  Interrupt handler after user pushed on of SW2, SW3, SW4, SW4 buttons
*/
void PORTE_IRQHandler(void) {

	// Delay to avoid multiple closure and opening of contact of button
	button_delay(2000);

	// reset PIT to count again from start value
	reset_PIT_timer();

	// determine which button was pushed

	if (PORTE->ISFR == BTN_SW4 && !(PTE->PDIR & BTN_SW4))
	{
		bnt_sw2_pressed = bnt_sw3_pressed = bnt_sw5_pressed = 0;
		bnt_sw4_pressed = 1;
		switch_text = 1;
	}

	else if (PORTE->ISFR == BTN_SW3 && !(PTE->PDIR & BTN_SW3))
	{
		bnt_sw2_pressed = bnt_sw4_pressed = bnt_sw5_pressed = 0;
		bnt_sw3_pressed = 1;
		switch_text = 1;
	}

	else if (PORTE->ISFR == BTN_SW2 && !(PTE->PDIR & BTN_SW2))
	{
		bnt_sw3_pressed = bnt_sw4_pressed = bnt_sw5_pressed = 0;
		bnt_sw2_pressed = 1;
		switch_text = 1;
	}

	else if (PORTE->ISFR == BTN_SW5 && !(PTE->PDIR & BTN_SW5))
	{
		bnt_sw2_pressed = bnt_sw3_pressed = bnt_sw4_pressed = 0;
		bnt_sw5_pressed = 1;
		switch_text = 1;
	}

	// clear register by #ffff
	PORTE->ISFR = ~0;
}

/**
 *@brief  Interrupt handler after timer made interruption
*/
void PIT0_IRQHandler(void) {

	/* clear pending IRQ */
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		NVIC_ClearPendingIRQ(PIT0_IRQn);
	}

	// this interruption is same as user would push button 5...end of writing on display
	bnt_sw2_pressed = bnt_sw3_pressed = bnt_sw4_pressed = 0;
	bnt_sw5_pressed = 1;
	switch_text = 1;
}


int main(void)
{
	SystemConfig();
	PIT_Init();


    for (;;) {

    	/* Determine which button was pushed and on that basis choose message to be written
    	   SW4 => HI message, SW3 => HEY message, SW4 => BEY message, SW5 => end of writing
    	*/

    	if(bnt_sw4_pressed == 1)
    	{
    		// print HI message
    		start_writing(HI_TXT);
    	}
    	else if (bnt_sw3_pressed == 1)
    	{
    		// print HEY message
    		start_writing(HEY_TXT);
    	}
    	else if (bnt_sw2_pressed == 1)
		{
    		// print BYE message
    		start_writing(BYE_TXT);
		}
    	else if (bnt_sw5_pressed == 1)
		{
    		// lit on whole display to end writing
    		finish_writing();
		}
    	else
    	{
    		// print HI message on default
    		start_writing(HI_TXT);
    	}
    }

    /* Never leave main */
    return 0;
}
