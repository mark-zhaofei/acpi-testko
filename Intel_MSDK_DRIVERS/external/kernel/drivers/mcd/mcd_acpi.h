/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#include <linux/mdm_ctrl_board.h>
#include <linux/mdm_ctrl.h>

#define NAME_LEN	16

#define INVALID_GPIO -1

#define MODEM_DATA_INDEX_UNSUP			0
#define MODEM_DATA_INDEX_GENERIC		1
#define MODEM_DATA_INDEX_2230			2
#define MODEM_DATA_INDEX_6260			3

/* Due to the IO-Extender on Kabylake, apparently, MCD will not be 
 * able to use the generic kernel gpio interface to control the
 * gpio pins needed for MCD. Instead, a Device Specific Method
 * is created to do the same. The following are params peratining
 * to this DSM */
#define MDM_KBL_DSM_UUID "84F9F22D-2E7E-48DB-9BB3-6704B05A3CE9"
#define MDM_KBL_DSM_REVISION 0

enum MDM_KBL_DSM_COMMANDS
   {
   MDM_KBL_DSM_GET_REVISION=0,
   MDM_KBL_DSM_CTRL_POWER_OFF=1,
   MDM_KBL_DSM_CTRL_POWER_ON=2,
   MDM_KBL_DSM_CTRL_BB_RESET_OFF=3,
   MDM_KBL_DSM_CTRL_BB_RESET_ON=4,
   MDM_KBL_DSM_CTRL_WAKE_ON_WWAN_OFF=5,
   MDM_KBL_DSM_CTRL_WAKE_ON_WWAN_ON=6,
   };


/* Retrieve modem parameters on ACPI framework */
int get_modem_acpi_data(struct platform_device *pdev);
void put_modem_acpi_data(struct platform_device *pdev);
int run_kbl_dsm (void * cpu_d, u32 command);
int cpu_data_is_kbl (void * cpu_d);

