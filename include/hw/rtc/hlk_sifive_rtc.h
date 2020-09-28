
#ifndef SIFIVE_HLK_RTC_H
#define SIFIVE_HLK_RTC_H

#include "hw/sysbus.h"

/*
 *  RTC state define
 *
 */
#include "hw/sysbus.h"
typedef struct HlkRtcState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    qemu_irq irq;

    uint32_t reg[4];
    int offset;
} HlkRtcState;

#define TYPE_HLK_RTC "hlk-rtc"
#define HLK_RTC(obj) OBJECT_CHECK(HlkRtcState, (obj), TYPE_HLK_RTC)
#define SIFIVE_HLK_RTC_SIZE 0x8000

/* Address offset of the registers */
#define SIFIVE_RTC0_RTCCFG     0x40     /* RTC configuration Register */
#define SIFIVE_RTC0_RTCCOUNTLO 0x48     /* Low bits of Counter */
#define SIFIVE_RTC0_RTCCOUNTHI 0x4C     /* High bits of Counter */
//#define SIFIVE_RTC0_RTCS       80UL
#define SIFIVE_RTC0_RTCCMP0    0x60     /* RTC Compare Register */


#endif /* SIFIVE_GPIO_H */
