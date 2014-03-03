#ifndef __RTC2_H__
#define __RTC2_H__

#include <stdint.h>
#include <stddef.h>
#include "rtc2_config.h"

#define RTC2_FORMAT_AM 0x80
#define RTC2_FORMAT_PM 0xA0
#define RTC2_FORMAT_24 0x00

#define RTC2_SECONDS_READ  0x81
#define RTC2_SECONDS_WRITE 0x80
#define RTC2_SECONDS_FIELD 0x01

#define RTC2_MINUTES_READ  0x83
#define RTC2_MINUTES_WRITE 0x82
#define RTC2_MINUTES_FIELD 0x02

#define RTC2_HOURS_READ  0x85
#define RTC2_HOURS_WRITE 0x84
#define RTC2_HOURS_FIELD 0x04

#define RTC2_DATE_READ  0x87
#define RTC2_DATE_WRITE 0x86
#define RTC2_DATE_FIELD 0x08

#define RTC2_MONTH_READ  0x89
#define RTC2_MONTH_WRITE 0x88
#define RTC2_MONTH_FIELD 0x10

#define RTC2_WDAY_READ  0x8B
#define RTC2_WDAY_WRITE 0x8A
#define RTC2_WDAY_FIELD 0x20

#define RTC2_YEAR_READ  0x8D
#define RTC2_YEAR_WRITE 0x8C
#define RTC2_YEAR_FIELD 0x40

#define RTC2_WP_READ  0x8F
#define RTC2_WP_WRITE 0x8E

#define RTC2_ALL_FIELDS 0x7F

#define RTC2_CHARGER_READ  0x91
#define RTC2_CHARGER_WRITE 0x90

#define RTC2_CHARGER_DISABLE  0x0
#define RTC2_CHARGER_ENABLED  0xA0
#define RTC2_CHARGER_1_DIODES 0x08
#define RTC2_CHARGER_2_DIODES 0x04
#define RTC2_CHARGER_ROUTE_1  0x1
#define RTC2_CHARGER_ROUTE_2  0x2
#define RTC2_CHARGER_ROUTE_3  0x3

#define RTC2_BURST_READ  0xBF
#define RTC2_BURST_WRITE 0xBE

#define RTC2_BURST_MEM_READ  0xFF
#define RTC2_BURST_MEM_WRITE 0xFE

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

void rtc2_init(void);

void rtc2_update(rtc2_datetime);
void rtc2_preset(rtc2_datetime);

void rtc2_set(rtc2_datetime, uint8_t);
void rtc2_get(rtc2_datetime, uint8_t);

void rtc2_mem_write_byte(uint8_t, uint8_t);
uint8_t rtc2_mem_read_byte(uint8_t);

void rtc2_mem_write(uint8_t, size_t, const void*);
void rtc2_mem_read(uint8_t, size_t, void*);

void rtc2_mem_puts(uint8_t, const char*);
void rtc2_mem_gets(uint8_t, size_t, char*);

void rtc2_set_charger(uint8_t);
uint8_t rtc2_get_charger(void);

uint8_t rtc2_halt(void);
void rtc2_set_halt(uint8_t);

uint8_t rtc2_protection(void);
void rtc2_set_protection(uint8_t);

#ifdef RTC2_DEFAULT
volatile rtc2_datetime_t __rtc2_default = {0};
volatile rtc2_datetime RTC2_VALUE = &__rtc2_default;
#endif

#endif
