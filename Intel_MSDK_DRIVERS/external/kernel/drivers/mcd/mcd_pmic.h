/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ifndef _MDM_PMIC_H
#define _MDM_PMIC_H

#define PMIC_MODEMCTRL_REG_SDWN_OPENDRAIN 8
#define PMIC_MODEMCTRL_REG_SDWN_SHIFT 2
#define PMIC_MODEMCTRL_REG_MODEMOFF_SHIFT 0

#define TSDWN2OFF	1000
#define TSDWN2ON	50

int pmic_io_init(void *data);
int pmic_io_power_on_mdm(void *data);
int pmic_io_power_on_ctp_mdm(void *data);
int pmic_io_power_off_mdm(void *data);
int pmic_io_cleanup(void *data);
int pmic_io_get_early_pwr_on(void *data);
int pmic_io_get_early_pwr_off(void *data);
#endif
