// vim: foldmethod=marker
#include <avr/io.h>
#include <util/delay.h>

#include "rtc2.h"

#if RTC2_RAM_STRINGS
#include <string.h>
#endif

// RTC2 utility macro handling I/O {{{
#define RTC2_IO_OUTPUT (RTC2_DDR |= _BV(RTC2_IO))
#define RTC2_IO_INPUT (RTC2_DDR &= ~_BV(RTC2_IO))
#define RTC2_IO_HIGH (RTC2_PORT |= _BV(RTC2_IO))
#define RTC2_IO_LOW (RTC2_PORT &= ~_BV(RTC2_IO))

#define RTC2_CLK_HIGH (RTC2_PORT |= _BV(RTC2_CLK))
#define RTC2_CLK_LOW (RTC2_PORT &= ~_BV(RTC2_CLK))

#define RTC2_CE_HIGH (RTC2_PORT |= _BV(RTC2_CE))
#define RTC2_CE_LOW (RTC2_PORT &= ~_BV(RTC2_CE))

#define RTC2_MEM_END_WRITE (RTC2_MEM_END - 1)
#define RTC2_MEM_START_WRITE (RTC2_MEM_START - 1)

#define RTC2_MEM_READ_INVALID(x) ((x) > RTC2_MEM_END || (x) < RTC2_MEM_START)
#define RTC2_MEM_WRITE_INVALID(x) ((x) > RTC2_MEM_END_WRITE || (x) < RTC2_MEM_START_WRITE)

#define RTC2_START_TRANSMISSION(kind) { rtc2_reset(); rtc2_write_byte(kind); } while(0)
#define RTC2_STOP_TRANSMISSION { RTC2_CE_LOW; RTC2_CLK_LOW; } while(0)
// }}}

// Default global variable {{{
#if RTC2_DEFAULT && (RTC2_READ || RTC2__WRITE)
static rtc2_datetime_t rtc2_default;
#endif
// }}}

// Initializer. Configures I/O ports and maybe sets up global variable {{{
void rtc2_init(void){
  RTC2_DDR |= _BV(RTC2_CE) | _BV(RTC2_CLK) | _BV(RTC2_IO);
  RTC2_PORT &= ~(_BV(RTC2_CE) | _BV(RTC2_CLK) | _BV(RTC2_IO));

#if RTC2_DEFAULT && (RTC2_READ || RTC2__WRITE)
  RTC2_VALUE = &rtc2_default;
#endif
}
// }}}

// Utility stuff used to reset current transfer state {{{
static inline void rtc2_reset(void){
  RTC2_STOP_TRANSMISSION;
  RTC2_CE_HIGH;
}
// }}}

// Low level write functions {{{

// Routine used for pretty much any operation {{{
#if RTC2_READ || RTC2_WRITE ||RTC2_RAM || RTC2_UTILITY
static void rtc2_write_byte(uint8_t byte){
  uint8_t i;

  RTC2_IO_OUTPUT;

  for(i = 0; i < 8; ++i){
    if(byte & 1)
      RTC2_IO_HIGH;
    else
      RTC2_IO_LOW;

    RTC2_CLK_LOW;
    _delay_us(2);
    RTC2_CLK_HIGH;
    _delay_us(2);
    byte >>= 1;
  }
}
#endif
// }}}

// Write to register {{{
#if RTC2_WRITE || RTC2_RAM || RTC2_UTILITY

static void rtc2_write(uint8_t reg, uint8_t val){
  RTC2_START_TRANSMISSION(reg);
  rtc2_write_byte(val);
  RTC2_STOP_TRANSMISSION;
}

#endif
// }}}

// }}}

// Low level read functions {{{
#if RTC2_READ || RTC2_UTILITY || RTC2_RAM

static uint8_t rtc2_read_byte(void){
  uint8_t i, ret = 0;

  RTC2_IO_INPUT;

  for(i = 0; i < 8; ++i){
    RTC2_CLK_HIGH;
    _delay_us(2);
    RTC2_CLK_LOW;
    _delay_us(2);
    ret >>= 1;

    if(bit_is_set(RTC2_PIN, RTC2_IO))
      ret |= _BV(7);
  }

  return ret;
}

static uint8_t rtc2_read(uint8_t reg){
  uint8_t ret;
  RTC2_START_TRANSMISSION(reg);
  ret = rtc2_read_byte();
  RTC2_STOP_TRANSMISSION;
  return ret;
}

#endif
// }}}

// Writing (presetting clock stuff) {{{
#if RTC2_WRITE

void rtc2_preset(rtc2_datetime ptr){
  rtc2_set(ptr, RTC2_ALL_FIELDS);
}

static uint8_t rtc2_store_field(uint8_t field, uint8_t val){
  switch(field){
    case RTC2_SECONDS_WRITE:
    case RTC2_MINUTES_WRITE:
      val = (((val / 10) << 4) & 0x70) | (val % 10);
      break;
    case RTC2_HOURS_WRITE:
      val = (val & RTC2_FORMAT_PM) | (((val / 10) << 4) & 0x10) | (val % 10);
      break;
    case RTC2_DATE_WRITE:
      val = (((val / 10) << 4) & 0x30) | (val % 10);
      break;
    case RTC2_MONTH_WRITE:
      val = (((val / 10) << 4) & 0x10) | (val % 10);
      break;
    case RTC2_WDAY_WRITE:
      val &= 0x03;
      break;
    case RTC2_YEAR_WRITE:
      val = (((val / 10) << 4) & 0xF0) | (val % 10);
      break;
  }

  return val;
}

static void rtc2_set_val(uint8_t field, uint8_t val){
  rtc2_write(field, rtc2_store_field(field, val));
}

void rtc2_set(rtc2_datetime ptr, uint8_t fields){
#if RTC2_BURST
  if(fields & RTC2_ALL_FIELDS){
    RTC2_START_TRANSMISSION(RTC2_BURST_WRITE);

    rtc2_write_byte(rtc2_store_field(RTC2_SECONDS_WRITE, ptr->seconds));
    rtc2_write_byte(rtc2_store_field(RTC2_MINUTES_WRITE, ptr->minutes));
    rtc2_write_byte(rtc2_store_field(RTC2_HOURS_WRITE, ptr->format | ptr->hours));
    rtc2_write_byte(rtc2_store_field(RTC2_DATE_WRITE, ptr->date));
    rtc2_write_byte(rtc2_store_field(RTC2_MONTH_WRITE, ptr->month));
    rtc2_write_byte(rtc2_store_field(RTC2_WDAY_WRITE, ptr->wday));
    rtc2_write_byte(rtc2_store_field(RTC2_YEAR_WRITE, ptr->year));
    rtc2_write_byte(0);

    RTC2_STOP_TRANSMISSION;
  }else{
#endif
    if(fields & RTC2_SECONDS_FIELD)
      rtc2_set_val(RTC2_SECONDS_WRITE, ptr->seconds);

    if(fields & RTC2_MINUTES_FIELD)
      rtc2_set_val(RTC2_MINUTES_WRITE, ptr->minutes);

    if(fields & RTC2_HOURS_FIELD)
      rtc2_set_val(RTC2_HOURS_WRITE, (ptr->format | ptr->hours));

    if(fields & RTC2_DATE_FIELD)
      rtc2_set_val(RTC2_DATE_WRITE, ptr->date);

    if(fields & RTC2_MONTH_FIELD)
      rtc2_set_val(RTC2_MONTH_WRITE, ptr->month);

    if(fields & RTC2_WDAY_FIELD)
      rtc2_set_val(RTC2_WDAY_WRITE, ptr->wday);

    if(fields & RTC2_YEAR_FIELD)
      rtc2_set_val(RTC2_YEAR_WRITE, ptr->year);
#if RTC2_BURST
  }
#endif
}

#endif
// }}}

// High level decoding and reading (updating) functions {{{
#if RTC2_READ

void rtc2_update(rtc2_datetime ptr){
  rtc2_get(ptr, RTC2_ALL_FIELDS);
}

static uint8_t rtc2_parse_val(uint8_t field, uint8_t val){
  switch(field){
    case RTC2_SECONDS_READ:
    case RTC2_MINUTES_READ:
      val = (val & 0x0F) + ((val & 0x70) >> 4) * 10;
      break;
    case RTC2_HOURS_READ:
      val = (val & RTC2_FORMAT_PM) | ((val & 0x0F) + ((val & 0x10) >> 4) * 10);
      break;
    case RTC2_DATE_READ:
      val = (val & 0x0F) + ((val & 0x30) >> 4) * 10;
      break;
    case RTC2_MONTH_READ:
      val = (val & 0x0F) + ((val & 0x10) >> 4) * 10;
      break;
    case RTC2_WDAY_READ:
      val &= 0x07;
      break;
    case RTC2_YEAR_READ:
      val = (val & 0x0F) + ((val & 0xF0) >> 4) * 10;
      break;
  }

  return val;
}

static uint8_t rtc2_get_val(uint8_t field){
  return rtc2_parse_val(field, rtc2_read(field));
}

void rtc2_get(rtc2_datetime ptr, uint8_t fields){
#if RTC2_BURST
  if(fields & RTC2_ALL_FIELDS){
    uint8_t tmp;

    RTC2_START_TRANSMISSION(RTC2_BURST_READ);

    ptr->seconds = rtc2_parse_val(RTC2_SECONDS_READ, rtc2_read_byte());
    ptr->minutes = rtc2_parse_val(RTC2_MINUTES_READ, rtc2_read_byte());

    tmp          = rtc2_parse_val(RTC2_HOURS_READ, rtc2_read_byte());
    ptr->format  = tmp & RTC2_FORMAT_PM;
    ptr->hours   = tmp & ~RTC2_FORMAT_PM;

    ptr->date    = rtc2_parse_val(RTC2_DATE_READ, rtc2_read_byte());
    ptr->month   = rtc2_parse_val(RTC2_MONTH_READ, rtc2_read_byte());
    ptr->wday    = rtc2_parse_val(RTC2_WDAY_READ, rtc2_read_byte());
    ptr->year    = rtc2_parse_val(RTC2_YEAR_READ, rtc2_read_byte());

    RTC2_STOP_TRANSMISSION;
  }else{
#endif
    if(fields & RTC2_SECONDS_FIELD)
      ptr->seconds = rtc2_get_val(RTC2_SECONDS_READ);

    if(fields & RTC2_MINUTES_FIELD)
      ptr->minutes = rtc2_get_val(RTC2_MINUTES_READ);

    if(fields & RTC2_HOURS_FIELD){
      uint8_t tmp = rtc2_get_val(RTC2_HOURS_READ);
      ptr->format = tmp & RTC2_FORMAT_PM;
      ptr->hours = tmp & ~RTC2_FORMAT_PM;
    }

    if(fields & RTC2_DATE_FIELD)
      ptr->date = rtc2_get_val(RTC2_DATE_READ);

    if(fields & RTC2_MONTH_FIELD)
      ptr->month = rtc2_get_val(RTC2_MONTH_READ);

    if(fields & RTC2_WDAY_FIELD)
      ptr->wday = rtc2_get_val(RTC2_WDAY_READ);

    if(fields & RTC2_YEAR_FIELD)
      ptr->year = rtc2_get_val(RTC2_YEAR_READ);
#if RTC2_BURST
  }
#endif
}

#endif
// }}}

// RAM access {{{
#if RTC2_RAM

void rtc2_mem_write_byte(uint8_t offset, uint8_t val){
  offset += RTC2_MEM_START_WRITE;

  // check for addres validity
  if(RTC2_MEM_WRITE_INVALID(offset))
    return; // kind of panic here or something

  rtc2_write(offset, val);
}

uint8_t rtc2_mem_read_byte(uint8_t offset){
  offset += RTC2_MEM_START;
  
  // check for addres validity
  if(RTC2_MEM_READ_INVALID(offset))
    return 0; // kind of panic here or something

  return rtc2_read(offset);
}

void rtc2_mem_write(uint8_t offset, size_t size, const void *buffer){
  offset += RTC2_MEM_START_WRITE;

  // check that first byte is valid *and* last byte is valid too
  // (hence whole chunk fits)
  if(RTC2_MEM_WRITE_INVALID(offset) || RTC2_MEM_WRITE_INVALID(offset + size * 2))
    return;

#if RTC2_BURST
  if(size > 2 && offset == RTC2_MEM_START_WRITE){
    RTC2_START_TRANSMISSION(RTC2_BURST_MEM_WRITE);

    for(;size > 0; offset += 2, --size, ++buffer)
      rtc2_write_byte(*(uint8_t*)buffer);

    RTC2_STOP_TRANSMISSION;
  }else{
#endif
    for(; size > 0; offset += 2, --size, ++buffer){
      rtc2_write(offset, *(uint8_t*)buffer);
    }
#if RTC2_BURST
  }
#endif
}

void rtc2_mem_read(uint8_t offset, size_t size, void *buffer){
  offset += RTC2_MEM_START;

  if(RTC2_MEM_READ_INVALID(offset) || RTC2_MEM_READ_INVALID(offset + size * 2))
    return;
 
#if RTC2_BURST
  if(size > 2 && offset == RTC2_MEM_START){
    RTC2_START_TRANSMISSION(RTC2_BURST_MEM_READ);

    for(; size > 0; offset += 2, --size, ++buffer)
      *(uint8_t*)buffer = rtc2_read_byte();

    RTC2_STOP_TRANSMISSION;
  }else{
#endif
    for(; size > 0; offset += 2, --size, ++buffer)
      *(uint8_t*)buffer = rtc2_read(offset);
#if RTC2_BURST
  }
#endif
}

// RAM string functions {{{
#if RTC2_RAM_STRINGS

void rtc2_mem_puts(uint8_t offset, const char *str){
  rtc2_mem_write(offset, strlen(str) + 1, str);
}

void rtc2_mem_gets(uint8_t offset, size_t maxlen, char *str){
  offset += RTC2_MEM_START;

  if(RTC2_MEM_READ_INVALID(offset))
    return;

  for(; maxlen > 1; --maxlen, ++str, offset += 2){
    *str = rtc2_read(offset);

    if(*str == 0)
      return;
  }

  *str = 0;
}

#endif
// }}}

#endif
// }}}

// Utility functions {{{
#if RTC2_UTILITY

void rtc2_set_charger(uint8_t flags){
  rtc2_write(RTC2_CHARGER_WRITE, flags);
}

uint8_t rtc2_get_charger(void){
  return rtc2_read(RTC2_CHARGER_READ);
}

uint8_t rtc2_halt(void){
  return rtc2_read(RTC2_SECONDS_READ) >> 7;
}

void rtc2_set_halt(uint8_t v){
  rtc2_write(RTC2_SECONDS_WRITE, v << 7);
}

uint8_t rtc2_protection(void){
  return rtc2_read(RTC2_WP_READ) >> 7;
}

void rtc2_set_protection(uint8_t v){
  rtc2_write(RTC2_WP_WRITE, v << 7);
}

#endif
// }}}
