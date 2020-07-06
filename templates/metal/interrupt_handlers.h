/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __INTERRUPT_HANDLERS_H
#define __INTERRUPT_HANDLERS_H

#include <metal/interrupt.h>

metal_interrupt_vector_handler_t __metal_vector_table[32];

void __metal_exception_handler();
void metal_riscv_cpu_intc_default_handler();
void metal_riscv_cpu_intc_default_handler();
void metal_riscv_cpu_intc_default_handler();
void metal_riscv_cpu_intc_msip_handler();
void metal_riscv_cpu_intc_default_handler();
void metal_riscv_cpu_intc_default_handler();
void metal_riscv_cpu_intc_default_handler();
void metal_riscv_cpu_intc_mtip_handler();
{% if global_interrupts is defined %}
void metal_riscv_plic0_source_0_handler();
{% else %}
void metal_riscv_cpu_intc_meip_handler();
{% endif %}
void metal_riscv_cpu_intc_default_handler();
void metal_riscv_cpu_intc_default_handler();
void metal_riscv_cpu_intc_default_handler();
void metal_riscv_cpu_intc_default_handler();
{% if local_interrupts is defined %}
{% for irq in local_interrupts.irqs %}
void metal_{{ to_snakecase(irq.source.compatible) }}_source_{{ irq.source.id }}_handler();
{% endfor %}
{% endif %}

{% if global_interrupts is defined %}
{% for irq in global_interrupts.irqs %}
void metal_{{ to_snakecase(irq.source.compatible) }}_source_{{ irq.source.id }}_handler();
{% endfor %}
{% endif %}

#endif 