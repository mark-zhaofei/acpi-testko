/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/delay.h>
#include <linux/mfd/intel_soc_pmic.h>
#include <linux/mdm_ctrl_board.h>
#include "mcd_pmic.h"

int pmic_io_init(void *data)
{
	return 0;
}

int pmic_io_power_on_mdm(void *data)
{
	struct mdm_ctrl_pmic_data *pmic_data = data;
	int ret = 0;
	u16 addr = pmic_data->chipctrl;
	u8 def_value = 0x00;
	u8 iodata;

#ifdef HAVE_INTEL_SOC_PMIC
	def_value = intel_soc_pmic_readb(addr);
	if (def_value < 0) {
		pr_err(DRVNAME ": intel_soc_pmic_readb(MDM_CTRL)  failed (err: %d)\n",
			   def_value);
		return -1;
	}
	
	/* force the SDWN to be in open drain mode */
	iodata = def_value & ~PMIC_MODEMCTRL_REG_SDWN_OPENDRAIN;
	ret = intel_soc_pmic_writeb(addr, iodata);
	if (ret) {
		pr_err(DRVNAME ": intel_soc_pmic_writeb(SDWN) failed (err: %d)\n",
			   ret);
		return -1;
	}
	    
	/* Write the new register value (SDWN_ON) */
	iodata = iodata |
		(pmic_data->chipctrlon << PMIC_MODEMCTRL_REG_SDWN_SHIFT);
	ret = intel_soc_pmic_writeb(addr, iodata);
	if (ret) {
		pr_err(DRVNAME ": intel_soc_pmic_writeb(SDWN) failed (err: %d)\n",
			   ret);
		return -1;
	}

	usleep_range(TSDWN2ON, TSDWN2ON+1);

	/* Write the new register value (POWER_ON) */
	iodata = iodata |
		(pmic_data->chipctrlon << PMIC_MODEMCTRL_REG_MODEMOFF_SHIFT);
	ret = intel_soc_pmic_writeb(addr, iodata);
	if (ret) {
		pr_err(DRVNAME ": intel_soc_pmic_writeb(ON) failed (err: %d)\n",
			   ret);
		return -1;
	}
#endif

	return 0;
}

int pmic_io_power_off_mdm(void *data)
{
	struct mdm_ctrl_pmic_data *pmic_data = data;
	int ret = 0;
	u16 addr = pmic_data->chipctrl;
	u8 iodata;
	u8 def_value = 0x00;

#ifdef HAVE_INTEL_SOC_PMIC
	def_value = intel_soc_pmic_readb(addr);
	if (def_value < 0) {
		pr_err(DRVNAME ": intel_soc_pmic_readb(MDM_CTRL)  failed (err: %d)\n",
			def_value);
		return -1;
	}

	/* force the SDWN to be in open drain mode */
	iodata = def_value & ~PMIC_MODEMCTRL_REG_SDWN_OPENDRAIN;
	ret = intel_soc_pmic_writeb(addr, iodata);
	if (ret) {
		pr_err(DRVNAME ": intel_soc_pmic_writeb(SDWN) failed (err: %d)\n",
			   ret);
		return -1;
	}
	
	/* Write the new register value (SDWN_OFF) */
	iodata = iodata &
		~(pmic_data->chipctrlon << PMIC_MODEMCTRL_REG_SDWN_SHIFT);
	ret = intel_soc_pmic_writeb(addr, iodata);
	if (ret) {
		pr_err(DRVNAME ": intel_soc_pmic_writeb(SDWN) failed (err: %d)\n",
			   ret);
		return -1;
	}

	usleep_range(TSDWN2OFF, TSDWN2OFF+1);

	/* Write the new register value (POWER_OFF) */
	iodata = iodata &
		~(pmic_data->chipctrlon << PMIC_MODEMCTRL_REG_MODEMOFF_SHIFT);
	ret = intel_soc_pmic_writeb(addr, iodata);
	if (ret) {
		pr_err(DRVNAME ": intel_soc_pmic_writeb(ON) failed (err: %d)\n",
		   ret);
		return -1;
	}

	/* Safety sleep. Avoid to directly call power on. */
	usleep_range(pmic_data->pwr_down_duration,
				 pmic_data->pwr_down_duration);

#endif
	return 0;
}

int pmic_io_cleanup(void *data)
{
	return 0;
}

int pmic_io_get_early_pwr_on(void *data)
{
	struct mdm_ctrl_pmic_data *pmic_data = data;
	return pmic_data->early_pwr_on;
}

int pmic_io_get_early_pwr_off(void *data)
{
	struct mdm_ctrl_pmic_data *pmic_data = data;
	return pmic_data->early_pwr_off;
}
