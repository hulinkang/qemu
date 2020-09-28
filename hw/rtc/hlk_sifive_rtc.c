/*---------------------------------------------------------------------------
 * Reference file: 
 *      hw/riscv/sifive_gpio.c
 *      hw/rtc/aspeed_rtc.c
 * By linkang.hu 2020-09-22
 *----------------------------------------------------------------------------
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/rtc/hlk_sifive_rtc.h"
#include "migration/vmstate.h"
#include "trace.h"


static inline void trace_hlk_rtc_read(uint64_t addr, uint64_t value)
{
    struct timeval _now;
    gettimeofday(&_now, NULL);
    qemu_log("%d@%zu.%06zu:hlk_rtc_read " "addr 0x%02" PRIx64 " value 0x%08" PRIx64 "\n",
                qemu_get_thread_id(),
                (size_t)_now.tv_sec, (size_t)_now.tv_usec
                , addr, value);
}

static inline void trace_hlk_rtc_write(uint64_t addr, uint64_t value)
{
    struct timeval _now;
    gettimeofday(&_now, NULL);
    qemu_log("%d@%zu.%06zu:hlk_rtc_write " "addr 0x%02" PRIx64 " value 0x%08" PRIx64 "\n",
                qemu_get_thread_id(),
                (size_t)_now.tv_sec, (size_t)_now.tv_usec
                , addr, value);

}

/* Read */
static uint64_t hlk_rtc_read(void *opaque, hwaddr offset,
                                unsigned size)
{
    HlkRtcState *rtc = opaque;
    uint64_t ret = 0;
    uint32_t r = offset;

    switch (r) {
    case SIFIVE_RTC0_RTCCFG:
        ret = rtc->reg[0];
        break;
    case SIFIVE_RTC0_RTCCOUNTLO:
        ret = rtc->reg[1];
        break;
    case SIFIVE_RTC0_RTCCOUNTHI:
        ret = rtc->reg[2];
        break;
    case SIFIVE_RTC0_RTCCMP0:
        ret = rtc->reg[3];
        break;
    default:
        qemu_log_mask(LOG_UNIMP, "%s: 0x%" HWADDR_PRIx "\n", __func__, offset);
        return 0;
    }

    //trace_hlk_rtc_read(offset, ret);

    return ret;
}

/* Write */
static void hlk_rtc_write(void *opaque, hwaddr offset,
                             uint64_t val, unsigned size)
{
    HlkRtcState *rtc = opaque;
    uint32_t r = offset;

    switch (r) {
    case SIFIVE_RTC0_RTCCFG:
        rtc->reg[0] = val;
        break;
    case SIFIVE_RTC0_RTCCOUNTLO:
        rtc->reg[1] = val;
        break;
    case SIFIVE_RTC0_RTCCOUNTHI:
        rtc->reg[2] = val;
        break;
    case SIFIVE_RTC0_RTCCMP0:
        rtc->reg[3] = val;
        break;
    default:
        qemu_log_mask(LOG_UNIMP, "%s: 0x%" HWADDR_PRIx "\n", __func__, offset);
        break;
    }
    //trace_hlk_rtc_write(offset, val);
}

/* Reset the RTC*/
static void hlk_rtc_reset(DeviceState *d)
{
    HlkRtcState *rtc = HLK_RTC(d);

    rtc->offset = 0;
    memset(rtc->reg, 0, sizeof(rtc->reg));
}

/* Operations to the RTC device*/
static const MemoryRegionOps hlk_rtc_ops = {
    .read = hlk_rtc_read,
    .write = hlk_rtc_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

/* realize the rtc*/
static void hlk_rtc_realize(DeviceState *dev, Error **errp)
{   
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
    HlkRtcState *s = HLK_RTC(dev);      //dynamic cast "DeviceState" to "HlkRtcState" 

    sysbus_init_irq(sbd, &s->irq);
    memory_region_init_io(&s->iomem, OBJECT(dev), &hlk_rtc_ops, s,   //1: init mmio，指定读写函数、大小
                          TYPE_HLK_RTC, SIFIVE_HLK_RTC_SIZE);
    sysbus_init_mmio(sbd, &s->iomem);                                //2: 把mmio传给设备
}

/* initialize the rtc class */
static void hlk_rtc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = hlk_rtc_realize;
    dc->reset = &hlk_rtc_reset;
}

static const TypeInfo hlk_rtc_info = {
    .name          = TYPE_HLK_RTC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(HlkRtcState),
    .class_init    = hlk_rtc_class_init,
};

static void hlk_rtc_register_types(void)
{
    type_register_static(&hlk_rtc_info);
}

type_init(hlk_rtc_register_types)