// vim: foldmethod=marker
#ifndef __RTC2_H__
#define __RTC2_H__

#include <stdint.h>
#include <stddef.h>
#include "rtc2_config.h"

// Clock formats {{{
#define RTC2_FORMAT_AM 0x80
#define RTC2_FORMAT_PM 0xA0
#define RTC2_FORMAT_24 0x00
// }}}

// Available fields for using with rtc2_preset, rtc2_set,
// rtc2_update, rtc2_get functions {{{
#define RTC2_SECONDS_FIELD   0x01
#define RTC2_MINUTES_FIELD   0x02
#define RTC2_HOURS_FIELD     0x04
#define RTC2_DATE_FIELD      0x08
#define RTC2_MONTH_FIELD     0x10
#define RTC2_WDAY_FIELD      0x20
#define RTC2_YEAR_FIELD      0x40

#define RTC2_ALL_FIELDS      0x7F // all fields mean update/preset all above
// }}}

// Charger flags. See datasheet for details. {{{
#define RTC2_CHARGER_DISABLE  0x0
#define RTC2_CHARGER_ENABLED  0xA0
#define RTC2_CHARGER_1_DIODES 0x08
#define RTC2_CHARGER_2_DIODES 0x04
#define RTC2_CHARGER_ROUTE_1  0x1
#define RTC2_CHARGER_ROUTE_2  0x2
#define RTC2_CHARGER_ROUTE_3  0x3
// }}}

// Timestamp base date {{{
#if RTC2_TIMESTAMP
#define RTC2_BASE_TIMESTAMP 946684800
#endif
// }}}

// You can disable clock and use just RAM/utility functions,
// but why do you need DS1302 then?

// Clock definitions {{{
#if RTC2_READ || RTC2_WRITE

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

typedef rtc2_datetime_t* rtc2_datetime;

#endif
// }}}

void rtc2_init(void);

// Clock reading/writing functions will use burst mode
// only if you read/write all fields and RTC2_BURST is non zero

// Write (preset) functions {{{
#if RTC2_WRITE
void rtc2_preset(rtc2_datetime src);
void rtc2_set(rtc2_datetime src, uint8_t fields);
#endif
// }}}

// Read (update) functions {{{
#if RTC2_READ
// gets all clock value from DS1302
void rtc2_update(rtc2_datetime dst);
// gets specified fields from DS1302. fields are given
// as bitwise concatenation of RTC2_*_FIELD constants.
// providing RTC2_ALL_FIELDS is effectively the same
// as rtc2_update.
void rtc2_get(rtc2_datetime dst, uint8_t fields);

// Timestamp conversion functions {{{
#if RTC2_TIMESTAMP
// **WARNING1**: IT IS IN LOCAL TIMEZONE
// **WARNING2**: Because we have unsigned chars everywhere
// and today is 2014 after all, base line for calculation
// is 2000/1/1 00:00:00. E.g. all dates are treated like they're
// after the this date.
//
// returns UNIX timestamp (since 1970/1/1 00:00:00)
// month starts from 1, years are two digits since 2000
uint32_t rtc2_mktime(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t date, uint8_t month, uint8_t year);
// wrapper passing rtc2_datetime fields to rtc2_mktime
uint32_t rtc2_timestamp(rtc2_datetime src);
// populates rtc2_datetime from timestamp
// will return 0 if timestamp is < RTC2_BASE_TIMESTAMP which is 1st January 2000
uint8_t rtc2_localtime(rtc2_datetime dst, uint32_t timestamp);
#endif
//}}}

#endif
// }}}

// RAM access functions {{{
#if RTC2_RAM

// offset in RAM functions is relative to RTC2_MEM_START.
// do not pass actual address.
// whenever those functions see that target have offset not fiting
// into memory range they'll silently cancel.
//
// functions operating on memory chunks verify both first byte (offset)
// and last byte (offset + size).
//
// also those functions will work in burst mode only if offset is 0
// that is reading from beginning of memory and RTC2_BURST is non-zero.

void rtc2_mem_write_byte(uint8_t offset, uint8_t value);
uint8_t rtc2_mem_read_byte(uint8_t offset);

void rtc2_mem_write(uint8_t offset, size_t size, const void *src);
void rtc2_mem_read(uint8_t offset, size_t size, void *dst);

/// RAM string helpers {{{
#if RTC2_RAM_STRINGS
void rtc2_mem_puts(uint8_t offset, const char *src);
void rtc2_mem_gets(uint8_t offset, size_t maxsize, char *dst);
#endif
// }}}

#endif
// }}}

// NOTE: true_false are exactly 1 bit, if you'll pass something else
// it can result in undesired effect.

// Utility functions {{{
#if RC2_UTILITY
// get trickle charger state. see RTC2_CHARGER_*
// constants above and datasheet for possible results.
uint8_t rtc2_get_charger(void);
// sets trickle charger state. see datasheet.
void rtc2_set_charger(uint8_t flags);

// are clock/RAM values write-protected?
uint8_t rtc2_halt(void);
// enables/disables clock/RAM write-protection
void rtc2_set_halt(uint8_t true_false);

// sets/gets clock setting protection
uint8_t rtc2_protection(void);
void rtc2_set_protection(uint8_t true_false);
#endif
// }}}

// Default global variable (actually initialized pointer) {{{
#if RTC2_DEFAULT && (RTC2_READ || RTC2_WRITE)
volatile rtc2_datetime RTC2_VALUE;
#endif
// }}}

#endif
