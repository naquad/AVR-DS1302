# AVR DS1302 Library

This library provides access to all functionality of [DS1302](http://www.maximintegrated.com/datasheet/index.mvp/id/2685) real time clock IC.

## Notes

Library is very raw, I've tested basic functionality (clock itself, burst mode, memory) in conjunction with ATmega168P microcontroller.

No support for multiple DS1302 (because of defines) and no support for interrupt-based I/O.

Library provides a lot and has pretty big size (2.7K with AVR-GCC 4.8.2), in future I plan to refactor functionality so one could select only required functions. Currently removing unnecessary functionality to save some space is up to user.

To make included example run get `USART.h` and `USART.c` from [hexagon5un](https://github.com/hexagon5un/AVR-Programming.git).

Slightly modified `Makefile` is also his.

## Reference

Library is configured with `rtc2_config.h` file. Make sure registers, memory size match your configuration.

Clock data is stored in following structure:

```
typedef struct {
  // time
  uint8_t seconds;
  uint8_t hours;
  uint8_t minutes;

  // date
  uint8_t wday;
  uint8_t date;
  uint8_t month;
  uint8_t year;

  // format: 24 hours or PM/AM
  uint8_t format;
} rtc2_datetime_t;
```

You'll see that functions accept `rtc_datetime`, this is just a pointer to structure above.

Library can also provide global rtc2_datetime instance. For this `#define RTC2_DEFAULT` in library config or before `#include`ing it.

### Fields list:

```
#define RTC2_SECONDS_FIELD 0x01
#define RTC2_MINUTES_FIELD 0x02
#define RTC2_HOURS_FIELD 0x04
#define RTC2_DATE_FIELD 0x08
#define RTC2_MONTH_FIELD 0x10
#define RTC2_WDAY_FIELD 0x20 // week day
#define RTC2_YEAR_FIELD 0x40

#define RTC2_ALL_FIELDS 0x7F // concatenation of all above
```

### Trickle charger

This is undested functionality. Following flags can be combined (`|`) to configure charger:

```
#define RTC2_CHARGER_DISABLE 0x0
#define RTC2_CHARGER_ENABLED 0xA0
#define RTC2_CHARGER_1_DIODES 0x08
#define RTC2_CHARGER_2_DIODES 0x04
#define RTC2_CHARGER_ROUTE_1 0x1
#define RTC2_CHARGER_ROUTE_2 0x2
#define RTC2_CHARGER_ROUTE_3 0x3
```

### Function list:

```
void rtc2_init(void); // presets ports. must be called before any other function.

void rtc2_update(rtc2_datetime ptr); // retrieves time from DS1302 to given structure instance
void rtc2_preset(rtc2_datetime ptr); // sets time in DS1302 from given structure instance

void rtc2_set(rtc2_datetime ptr, uint8_t fields); // sets specific fields in DS1302
void rtc2_get(rtc2_datetime ptr, uint8_t fields); // retrieves specific fields from DS1302
// if fields & RTC2_ALL_FIELDS then functions will perform operation on all fields

void rtc2_mem_write_byte(uint8_t offset, uint8_t byte); // writes one byte to DS1302 RAM
uint8_t rtc2_mem_read_byte(uint8_t offset); // read one byte from DS1302 RAM


void rtc2_mem_write(uint8_t offset, size_t size, const void *buffer); // write size bytes from buffer to RAM
void rtc2_mem_read(uint8_t offset, size_t size, void *buffer); // read size bytes from RAM to buffer

void rtc2_mem_puts(uint8_t offset, const char *str); // a helper to put string into memory. will measure string with strlen(3)
void rtc2_mem_gets(uint8_t offset, size_t maxlength, char *str); // a helper to read string from memory. will read until null-terminator or max length is achieved.
// offset in all functions above is relative, address is not supported.
// if offset is not in range of memory then operation is quietly cancelled.

void rtc2_set_charger(uint8_t); // trickle charger
uint8_t rtc2_get_charger(void);

uint8_t rtc2_halt(void); // clock stopped?
void rtc2_set_halt(uint8_t stop); // 0 - continue, 1 - stop

uint8_t rtc2_protection(void); // protect from setting time?
void rtc2_set_protection(uint8_t); // 0 - no protection, 1 - protect

// for details on trickle, halt and protection functionality see datasheet

```

## TODO

* Better documentation;
* More conditional functionality to reduce library size;
* More testing;
* Testing with other models of AVR and maybe other compilers;
