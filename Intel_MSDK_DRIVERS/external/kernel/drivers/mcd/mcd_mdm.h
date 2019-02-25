/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef _MDM_IMC_H
#define _MDM_IMC_H

int mcd_mdm_init(void *data);
int mcd_mdm_cold_boot(void *data);
int mcd_mdm_warm_reset(void *data, int rst);
int mcd_mdm_power_off(void *data);
int mcd_mdm_get_cflash_delay(void *data);
int mcd_mdm_get_wflash_delay(void *data);
int mcd_mdm_cleanup(void *data);
int mcd_mdm_cold_boot_ngff(void *data);
int mcd_mdm_power_off_ngff(void *data);
int mcd_mdm_cold_boot_2230(void *data);
int mcd_mdm_power_off_2230(void *data);

#endif				/* _MDM_IMC_H */
