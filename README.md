# AVR DS1302 Library

This library provides access to all functionality of [DS1302](http://www.maximintegrated.com/datasheet/index.mvp/id/2685) real time clock IC.

## Notes

Library is very raw, I've tested basic functionality (clock itself, burst mode, memory) in conjunction with ATmega168P microcontroller.

No support for multiple DS1302 (because of defines) and no support for interrupt-based I/O.

Library provides a lot and has pretty big size (2.7K with AVR-GCC 4.8.2), in future I plan to refactor functionality so one could select only required functions. Currently removing unnecessary functionality to save some space is up to user.

To make included example run get `USART.h` and `USART.c` from [hexagon5un](https://github.com/hexagon5un/AVR-Programming.git).

Slightly modified `Makefile` is also his.

**NOTE:** Do not copy example as is. It utilizes `printf()` function
which is quiet heavy.

## Reference

For reference look into `rtc2.h`;

### Enabling / disabling features

To save some space you can enable or disable functionality. See
`rtc2_config.h` file for details.

## TODO

* More testing;
* Testing with other models of AVR and maybe other compilers;
