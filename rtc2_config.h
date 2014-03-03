#ifndef __RTC2_CONFIG_H__
#define __RTC2_CONFIG_H__

// if this is defined then you'll have
// a global volatile variables __rtc2_default (structure)
// and RTC2_VALUE (pointer to it)
// #define RTC2_DEFAULT

#ifndef RTC2_PORT

// currently library supports only single port,
// you can't have lines on different ports or more than one DS1302 :(

#define RTC2_PORT PORTC
#define RTC2_PIN  PINC
#define RTC2_DDR  DDRC

// DS1302 SCLK line
#define RTC2_CLK PC5
// DS1302 I/O line
#define RTC2_IO  PC4
// DS1302 CE line
#define RTC2_CE  PC3

// memory read address range. write address = read address - 1
#define RTC2_MEM_START 0xC1
#define RTC2_MEM_END 0xFD

#endif

// burst mode can be disabled with #define RTC2_BURST 0
#ifndef RTC2_BURST
#define RTC2_BURST 1
#endif

#define RTC2_MEM_SIZE ((RTC2_MEM_START - RTC2_MEM_END) / 2 + 1)

#endif
