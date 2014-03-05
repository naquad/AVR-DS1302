#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include "USART.h"
#include "rtc2.h"

int main(void)	{
  char buf[64] = {0};

  //initUSART();

  rtc2_init();

  RTC2_VALUE->seconds = 0;
  RTC2_VALUE->minutes = 0;
  RTC2_VALUE->hours = 0;
  RTC2_VALUE->date = 1;
  RTC2_VALUE->month = 1;
  RTC2_VALUE->year = 14;
  RTC2_VALUE->format = RTC2_FORMAT_24;

  rtc2_preset(RTC2_VALUE);
 
  rtc2_mem_puts(0, "test\r\n");
  rtc2_mem_gets(0, 9, buf);
  //printString(buf);
 
  while (1){
    uint32_t t;
    rtc2_update(RTC2_VALUE);
    t = rtc2_timestamp(RTC2_VALUE);

    /*sprintf(buf, "%lu %02i/%02i/20%02i %02i:%02i:%02i %.2s\r\n",
        t,
        RTC2_VALUE->date, RTC2_VALUE->month, RTC2_VALUE->year,
        RTC2_VALUE->hours, RTC2_VALUE->minutes, RTC2_VALUE->seconds,
        RTC2_VALUE->format == RTC2_FORMAT_PM ? "PM" : "AM");

    printString(buf);*/

    _delay_ms(1000);
  }

  return 0;
}
