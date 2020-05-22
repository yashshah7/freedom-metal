/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <metal/machine/platform.h>

#ifdef METAL_SIFIVE_TRACE

#include <metal/drivers/sifive_trace.h>
#include <metal/machine.h>

#define TRACE_REG(offset) (((unsigned long)base + (offset)))
#define TRACE_REG8(offset)                                                     \
    (__METAL_ACCESS_ONCE((__metal_io_u8 *)TRACE_REG(offset)))
#define TRACE_REG16(offset)                                                    \
    (__METAL_ACCESS_ONCE((__metal_io_u16 *)TRACE_REG(offset)))
#define TRACE_REG32(offset)                                                    \
    (__METAL_ACCESS_ONCE((__metal_io_u32 *)TRACE_REG(offset)))

static void write_itc_uint32(struct metal_uart *trace, uint32_t data) {
    long base = __metal_driver_sifive_trace_base(trace);

    TRACE_REG32(METAL_SIFIVE_TRACE_ITCSTIMULUS) = data;
}

static void write_itc_uint16(struct metal_uart *trace, uint16_t data) {
    long base = __metal_driver_sifive_trace_base(trace);

    TRACE_REG16(METAL_SIFIVE_TRACE_ITCSTIMULUS + 2) = data;
}

static void write_itc_uint8(struct metal_uart *trace, uint8_t data) {
    long base = __metal_driver_sifive_trace_base(trace);

    TRACE_REG8(METAL_SIFIVE_TRACE_ITCSTIMULUS + 3) = data;
}

int __metal_driver_sifive_trace_putc(struct metal_uart *trace, int c) {
    static uint32_t buffer = 0;
    static int bytes_in_buffer = 0;

    buffer |= (((uint32_t)c) << (bytes_in_buffer * 8));

    bytes_in_buffer += 1;

    if (bytes_in_buffer >= 4) {
        write_itc_uint32(trace, buffer);

        buffer = 0;
        bytes_in_buffer = 0;
    } else if (((char)c == '\n') || ((char)c == '\r')) { // partial write
        switch (bytes_in_buffer) {
        case 3: // do a full word write
            write_itc_uint16(trace, (uint16_t)(buffer));
            write_itc_uint8(trace, (uint8_t)(buffer >> 16));
            break;
        case 2: // do a 16 bit write
            write_itc_uint16(trace, (uint16_t)buffer);
            break;
        case 1: // do a 1 byte write
            write_itc_uint8(trace, (uint8_t)buffer);
            break;
        }

        buffer = 0;
        bytes_in_buffer = 0;
    }

    return c;
}

void __metal_driver_sifive_trace_init(struct metal_uart *trace, int baud_rate) {
    // The only init we do here is to make sure ITC 0 is enabled. It is up to
    // Freedom Studio or other mechanisms to make sure tracing is enabled. If we
    // try to enable tracing here, it will likely conflict with Freedom Studio,
    // and they will just fight with each other.

    long base = __metal_driver_sifive_trace_base(trace);

    TRACE_REG32(METAL_SIFIVE_TRACE_ITCTRACEENABLE) |= 0x00000001;
}

__METAL_DEFINE_VTABLE(__metal_driver_vtable_sifive_trace) = {
    .uart.init = __metal_driver_sifive_trace_init,
    .uart.putc = __metal_driver_sifive_trace_putc,
    .uart.getc = NULL,

    .uart.get_baud_rate = NULL,
    .uart.set_baud_rate = NULL,

    .uart.controller_interrupt = NULL,
    .uart.get_interrupt_id = NULL,
};

#ifdef METAL_STDOUT_SIFIVE_TRACE
#if defined(__METAL_DT_STDOUT_UART_HANDLE)

METAL_CONSTRUCTOR(metal_tty_init) {
    metal_uart_init(__METAL_DT_STDOUT_UART_HANDLE, __METAL_DT_STDOUT_UART_BAUD);
}

int metal_tty_putc(int c) {
    return metal_uart_putc(__METAL_DT_STDOUT_UART_HANDLE, c);
}

int metal_tty_getc(int *c) {
    do {
        metal_uart_getc(__METAL_DT_STDOUT_UART_HANDLE, c);
        /* -1 means no key pressed, getc waits */
    } while (-1 == *c);
    return 0;
}

#ifndef __METAL_DT_STDOUT_UART_BAUD
#define __METAL_DT_STDOUT_UART_BAUD 115200
#endif

#endif /* __METAL_DT_STDOUT_UART_HANDLE */
#endif /* METAL_STDOUT_SIFIVE_TRACE */

#endif /* METAL_SIFIVE_TRACE */

typedef int no_empty_translation_units;
