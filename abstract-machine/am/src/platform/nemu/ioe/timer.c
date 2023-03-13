#include <am.h>
#include <nemu.h>
#include <klib.h>

#include "../../../riscv/riscv.h"

void __am_timer_init() {

}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = ((uint64_t)inl(VGACTL_ADDR + 4) << 32) | (uint64_t)inl(VGACTL_ADDR);
  // uptime->us = ((uint64_t)inl() << 32) | (uint64_t)inl(0xa111056b);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
