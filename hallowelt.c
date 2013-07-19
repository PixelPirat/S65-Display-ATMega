/*

  Dies ist eine kleine Demo mit den grundlegenden Funktionen.
  
*/

#define F_CPU 16000000

#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

#include "s65display.c"

int main(void)
{
  spi_init();
  lcd_init();
  
  pwm_init();
  setpwm(80,8);
    
  _delay_ms(100);
  
  stdout = &lcdout;
     
  textcolor = black;
  bkcolor   = white;
  clrscr();
  
  setoutmode(horizontal);
  
  gotoxy(1,1);
  printf("Hello World");
  
  gotoxy(2,3);
  textsize  = 1;
  textcolor = blue;
  printf(":)");
    
  for(;;);
}

