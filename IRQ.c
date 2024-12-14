#include <LPC23xx.H>                    /* LPC23xx definitions                */


short AD_last;                          /* Last converted value               */

unsigned char clock_1s;                 /* Flag activated each second         */


/* Timer0 IRQ: Executed periodically                                          */
__irq void T0_IRQHandler (void) {
  static int clk_cntr;
  

  clk_cntr++;
  if (clk_cntr >= 1000) {
    clk_cntr = 0;
    clock_1s = 1;                       /* Activate flag every 1 second       */
  }

  
  AD0CR |= 0x01000000;                  /* Start A/D Conversion               */

  T0IR        = 1;                      /* Clear interrupt flag               */
  VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}

/* A/D IRQ: Executed when A/D Conversion is done                              */
__irq void ADC_IRQHandler(void) {

   AD_last = (AD0DR0 >> 6) & 0x3FF;      /* Read Conversion Result             */

  VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}
