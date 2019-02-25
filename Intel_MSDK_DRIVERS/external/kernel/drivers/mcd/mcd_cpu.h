/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef _MDM_CPU_H
#define _MDM_CPU_H

int cpu_init_gpio(void *data);
int cpu_cleanup_gpio(void *data);
int get_gpio_irq_cdump(void *data);
int get_gpio_irq_rst(void *data);
int get_gpio_mdm_state(void *data);
int get_gpio_rst(void *data);
int get_gpio_pwr(void *data);
int get_gpio_rst_usbhub(void *data);
int cpu_init_gpio_ngff(void *data);

#endif
