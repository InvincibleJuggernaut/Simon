#include <LPC23xx.H>                    /* LPC23xx definitions                */


/* Function that initializes LEDs                                             */
void LED_Init(void) {
  PINSEL10 = 0;                         /* Disable ETM interface, enable LEDs */
  FIO2DIR  = 0x000000FF;                /* P2.0..7 defined as Outputs         */
  FIO2MASK = 0x00000000;
}

/* Function that turns on requested LED                                       */
void LED_On (unsigned int num) {
  FIO2SET = (1 << num);
}

/* Function that turns off requested LED                                      */
void LED_Off (unsigned int num) {
  FIO2CLR = (1 << num);
}

/* Function that outputs value to LEDs                                        */
void LED_Out(unsigned int value) {
  FIO2CLR = 0xFF;                       /* Turn off all LEDs                  */
  FIO2SET = (value & 0xFF);             /* Turn on requested LEDs             */
}


/* Import external IRQ handlers from IRQ.c file                               */
extern __irq void T0_IRQHandler  (void);
extern __irq void ADC_IRQHandler (void);



/* Import external variables from IRQ.c file                                  */
extern short AD_last;
extern unsigned char clock_1s;


int main (void) {
  int i;
  short AD_old, AD_value, AD_print;

  LED_Init();                           /* LED Initialization                 */

  /* Enable and setup timer interrupt, start timer                            */
  T0MR0         = 11999;                       /* 1msec = 12000-1 at 12.0 MHz */
  T0MCR         = 3;                           /* Interrupt and Reset on MR0  */
  T0TCR         = 1;                           /* Timer0 Enable               */
  VICVectAddr4  = (unsigned long)T0_IRQHandler;/* Set Interrupt Vector        */
  VICVectCntl4  = 15;                          /* use it for Timer0 Interrupt */
  VICIntEnable  = (1  << 4);                   /* Enable Timer0 Interrupt     */

  /* Power enable, Setup pin, enable and setup AD converter interrupt         */
  PCONP        |= (1 << 12);                   /* Enable power to AD block    */
  PINSEL1       = 0x4000;                      /* AD0.0 pin function select   */
  AD0INTEN      = (1 <<  0);                   /* CH0 enable interrupt        */
  AD0CR         = 0x00200301;                  /* Power up, PCLK/4, sel AD0.0 */
  VICVectAddr18 = (unsigned long)ADC_IRQHandler;/* Set Interrupt Vector       */
  VICVectCntl18 = 14;                          /* use it for ADC Interrupt    */
  VICIntEnable  = (1  << 18);                  /* Enable ADC Interrupt        */

  for (i = 0; i < 20000000; i++);       /* Wait for initial display           */

  while (1) {                           /* Loop forever                       */
    AD_value = AD_last;                 /* Read AD_last value                 */
    if (AD_value != AD_last)            /* Make sure that AD interrupt did    */
      AD_value = AD_last;               /* not interfere with value reading   */
    AD_print  = AD_value;               /* Get unscaled value for printout    */
    AD_value /= 13;                     /* Scale to AD_Value to 0 - 78        */
    if (AD_old != AD_value)  {          /* If AD value has changed            */
      AD_old = AD_value;
      
    }
    if (clock_1s) {
      clock_1s = 0;
      printf ("AD value = 0x%03x\n\r", AD_print);
    }
  }
}
