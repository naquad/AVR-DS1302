#ifndef __RTC2_CONFIG_H__
#define __RTC2_CONFIG_H__

// if there is no configuration then apply some defaults
#ifndef RTC2_PORT

// port where DS1302 is connected. currently only one port mode
// is supported :( e.g. you should connect all lines from DS1302 
// to the pin bank aka port
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
// make sure this matches your datasheet
#define RTC2_MEM_START 0xC1
#define RTC2_MEM_END 0xFD

#endif

// define global variable?
#ifndef RTC2_DEFAULT
#define RTC2_DEFAULT 1
#endif

// enable clock read functions?
#ifndef RTC2_READ
#define RTC2_READ 1
#endif

// enable clock write functions?
#ifndef RTC2_WRITE
#define RTC2_WRITE 1
#endif

// enable burst mode? (reading in series of sequential bytes)
#ifndef RTC2_BURST
#define RTC2_BURST 1
#endif

// enable RAM utilities? (write to autonomous RAM in DS1302)
#ifndef RTC2_RAM
#define RTC2_RAM 1
#endif

// RAM strings functions puts/gets. RTC2_RAM must be enabled to use this.
#ifndef RTC2_RAM_STRINGS
#define RTC2_RAM_STRINGS 1
#endif

// enable utility functions: clock halt, charger and protection settings
#ifndef RTC2_UTILITY
#define RTC2_UTILITY 1
#endif

// a macro for getting available memory size. not used anywhere,
// maybe can be used in program
#define RTC2_MEM_SIZE ((RTC2_MEM_START - RTC2_MEM_END) / 2 + 1)

#endif
