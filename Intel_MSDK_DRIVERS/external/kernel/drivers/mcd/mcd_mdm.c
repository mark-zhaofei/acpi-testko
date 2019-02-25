/*
 * Copyright (C) 2018 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#include "mdm_util.h"
#include "mcd_mdm.h"

#include <linux/mdm_ctrl.h>
#include "mcd_acpi.h"


/* NGFF specific timings */
#define HUB_RST_PULSE_WIDTH 1000
#define HUB_RST_CONFIG_DELAY 120000
#define PWR_ON_DELAY_NGFF 10000
#define POWER_OFF_DELAY_NGFF 1000000

/* 2230 specific timings */
#define PWR_ON_DELAY_2230 100000
#define ON_KEY_DELAY 1000000
#define ON_KEY_PULSE_WIDTH 150000

/*****************************************************************************
 *
 * Modem Power/Reset functions
 *
 ****************************************************************************/

int mcd_mdm_init(void *data)
{
	return 0;
}

void mcd_mdm_set_gpio (struct mcd_base_info * pdata, int is_kbl, int type, int gpio, int value)
{
    void *cpu_data = pdata->cpu_data;
    if (is_kbl)
    {
        int s = run_kbl_dsm (cpu_data, type);
        pr_info ("%s: Cmd: %d, Value: %d, Status:%d",__func__,type, value, s); 
    }
    else
        gpio_set_value(gpio, value);
}

void mcd_mdm_set_gpio_cansleep (struct mcd_base_info * pdata, int is_kbl, int type, int gpio, int value)
{
    void *cpu_data = pdata->cpu_data;
    if (is_kbl)
    {
        int s = run_kbl_dsm (cpu_data, type);
        pr_info ("%s: Cmd: %d, Value: %d, Status:%d",__func__,type, value, s); 
    }
    else
        gpio_set_value_cansleep (gpio, value);
}

/**
 *  mcd_mdm_cold_boot - Perform a modem cold boot sequence
 *  @drv: Reference to the driver structure
 *
 *  - Set to HIGH the PWRDWN_N to switch ON the modem
 *  - Set to HIGH the RESET_BB_N
 *  - Do a pulse on ON1
 */
int mcd_mdm_cold_boot(void *data)
{
	struct mdm_info *mdm = data;

	struct cpu_ops *cpu = &mdm->pdata->cpu;
	struct pmic_ops *pmic = &mdm->pdata->pmic;

	struct mdm_ctrl_mdm_data *mdm_data = mdm->pdata->modem_data;
	void *cpu_data = mdm->pdata->cpu_data;
	void *pmic_data = mdm->pdata->pmic_data;

	int ret = 0;
	int rst = cpu->get_gpio_rst(cpu_data);
	int pwr_on = cpu->get_gpio_pwr(cpu_data);
	int pwr_on_ctrl = mdm->pdata->pwr_on_ctrl;
        int is_kbl =0;
        is_kbl =cpu_data_is_kbl (cpu_data); 

	if (pwr_on_ctrl == POWER_ON_PMIC) {
	  	if ((rst != INVALID_GPIO)|| is_kbl)
			/* Toggle the RESET_BB_N */
			mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_ON, rst, 1);

		/* Toggle modem ctrl signals using only PMIC */
		if (pmic->power_on_mdm) {
			ret = pmic->power_on_mdm(pmic_data);
			if (ret) {
				pr_err(DRVNAME ": Error PMIC power-ON.");
				goto end;
			}
		}
	} else if (pwr_on_ctrl == POWER_ON_PMIC_GPIO) {
		/* Toggle power_on signal using PMIC */
		if (pmic->power_on_mdm) {
			ret = pmic->power_on_mdm(pmic_data);
			if (ret) {
				pr_err(DRVNAME ": Error PMIC power-ON.");
				goto end;
			}
		}

		if (((rst != INVALID_GPIO) && (pwr_on != INVALID_GPIO))||
                       is_kbl) {
			/* Toggle the RESET_BB_N */
                        mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_ON, rst, 1);

			/* Wait before doing the pulse on ON1 */
			usleep_range(mdm_data->pre_on_delay,
				mdm_data->pre_on_delay + 1);

			/* Do a pulse on ON1 */
                        mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_POWER_ON, pwr_on, 1);

			usleep_range(mdm_data->on_duration,
						 mdm_data->on_duration + 1);
                        mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_POWER_OFF, pwr_on, 0);
		}
	} else {
		pr_err(DRVNAME ": Power on method not supported");
		ret = -EINVAL;
	}

end :
	return ret;
}

/**
 *  mdm_ctrl_silent_warm_reset_7x6x - Perform a silent modem warm reset
 *				      sequence
 *  @drv: Reference to the driver structure
 *
 *  - Do a pulse on the RESET_BB_N
 *  - No struct modification
 *  - debug purpose only
 */
int mcd_mdm_warm_reset(void *data, int rst)
{
	struct mdm_info *mdm = data;
	struct mdm_ctrl_mdm_data *mdm_data = mdm->pdata->modem_data;
	void *cpu_data = mdm->pdata->cpu_data;
        int is_kbl =  cpu_data_is_kbl (cpu_data);

	if (rst != INVALID_GPIO|| is_kbl) {
                mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_OFF, rst, 0);
		usleep_range(mdm_data->warm_rst_duration,
				mdm_data->warm_rst_duration + 1);
                mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_ON, rst, 1);
	}
	return 0;
}

/**
 *  mcd_mdm_power_off - Perform the modem switch OFF sequence
 *  @drv: Reference to the driver structure
 *
 *  - Set to low the ON1
 *  - Write the PMIC reg
 */
int mcd_mdm_power_off(void *data)
{
	struct mdm_info *mdm = data;

	struct cpu_ops *cpu = &mdm->pdata->cpu;
	struct pmic_ops *pmic = &mdm->pdata->pmic;

	struct mdm_ctrl_mdm_data *mdm_data = mdm->pdata->modem_data;
	void *cpu_data = mdm->pdata->cpu_data;
	void *pmic_data = mdm->pdata->pmic_data;
        int is_kbl =  cpu_data_is_kbl (cpu_data);

	int ret = 0;
	int rst = cpu->get_gpio_rst(cpu_data);
	int pwr_on_ctrl = mdm->pdata->pwr_on_ctrl;

	if (pwr_on_ctrl == POWER_ON_PMIC_GPIO) {
		if (rst != INVALID_GPIO||is_kbl) {
			/* Set the RESET_BB_N to 0 */
                        mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_OFF, rst, 0);

			/* Wait before doing the pulse on ON1 */
			usleep_range(mdm_data->pre_pwr_down_delay,
				mdm_data->pre_pwr_down_delay + 1);
			if (pmic->power_off_mdm) {
				ret = pmic->power_off_mdm(pmic_data);
				if (ret)
					pr_err(DRVNAME ": Error PMIC power-OFF.");
			}
		}
	} else if (pwr_on_ctrl == POWER_ON_PMIC) {
		if (pmic->power_off_mdm) {
			ret = pmic->power_off_mdm(pmic_data);
			if (ret)
				pr_err(DRVNAME ": Error PMIC power-OFF.");
		}

		if (rst != INVALID_GPIO || is_kbl)
			/* Set the RESET_BB_N to 0 */
			mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_OFF, rst, 0);
	} else {
		pr_err(DRVNAME ": Power on method not supported");
		ret = -EINVAL;
	}
	return ret;
}

int mcd_mdm_get_cflash_delay(void *data)
{
	struct mdm_ctrl_mdm_data *mdm_data = data;
	return mdm_data->pre_cflash_delay;
}

int mcd_mdm_get_wflash_delay(void *data)
{
	struct mdm_ctrl_mdm_data *mdm_data = data;
	return mdm_data->pre_wflash_delay;
}

int mcd_mdm_cleanup(void *data)
{
	return 0;
}

/**
 *  mcd_mdm_cold_boot_ngff - Perform a NGFF modem cold boot sequence
 *  @drv: Reference to the driver structure
 *
 *  - Reset USB hub if needed
 *  - Set to HIGH the RESET_BB_N
 *  - Set to HIGH the POWER_ON_OFF using PMIC
 */
int mcd_mdm_cold_boot_ngff(void *data)
{
	struct mdm_info *mdm = data;

	struct cpu_ops *cpu = &mdm->pdata->cpu;
	void *cpu_data = mdm->pdata->cpu_data;

	struct pmic_ops *pmic = &mdm->pdata->pmic;
	void *pmic_data = mdm->pdata->pmic_data;

	int ret = 0;
	int rst = cpu->get_gpio_rst(cpu_data);
	int pwr_on = cpu->get_gpio_pwr(cpu_data);
	int rst_usbhub = cpu->get_gpio_rst_usbhub(cpu_data);
	int pwr_on_ctrl = mdm->pdata->pwr_on_ctrl;
        int is_kbl =  cpu_data_is_kbl (cpu_data);

	/* Reset the USB hub if needed */
	if ((mdm->pdata->usb_hub_ctrl) &&
		(rst_usbhub != INVALID_GPIO)) {
			gpio_set_value(rst_usbhub, 0);
			usleep_range(HUB_RST_PULSE_WIDTH,
						 HUB_RST_PULSE_WIDTH + 1);
			gpio_set_value(rst_usbhub, 1);
			usleep_range(HUB_RST_CONFIG_DELAY,
						 HUB_RST_CONFIG_DELAY + 1);
	}

	if (((rst != INVALID_GPIO) && (pwr_on != INVALID_GPIO))||
             is_kbl) {
		/* Toggle the RESET_BB_N */
                mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_ON, rst, 1);

		/* Wait before toggling POWER_ON_OFF,
		 * NGFF specific timing */
		usleep_range(PWR_ON_DELAY_NGFF,
					 PWR_ON_DELAY_NGFF + 1);

		/* Toggle POWER_ON_OFF using PMIC or GPIO */
		if (pwr_on_ctrl == POWER_ON_PMIC_GPIO) {
			if (pmic->power_on_mdm) {
				ret = pmic->power_on_mdm(pmic_data);
				if (ret)
					pr_err(DRVNAME ": Error PMIC power-ON.");
			}
		} else if (pwr_on_ctrl == POWER_ON_GPIO)
                        mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_POWER_ON, pwr_on, 1);
		else {
			pr_err(DRVNAME ": Power on method not supported on NGFF");
			ret = -EINVAL;
		}
	}
	return ret;
}


int mcd_mdm_cold_boot_2230(void *data)
{
	struct mdm_info *mdm = data;

	struct cpu_ops *cpu = &mdm->pdata->cpu;
	void *cpu_data = mdm->pdata->cpu_data;

	struct mdm_ctrl_mdm_data *mdm_data = mdm->pdata->modem_data;

	int rst = cpu->get_gpio_rst(cpu_data);
	int pwr_on = cpu->get_gpio_pwr(cpu_data);
        int is_kbl = cpu_data_is_kbl (cpu_data); 

	if (((rst != INVALID_GPIO) && (pwr_on != INVALID_GPIO))|| is_kbl) {
		/* Toggle the RESET_BB_N */
		mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_OFF, rst, 0);

		/* Toggle the POWER_ON */
		usleep_range(mdm_data->pre_on_delay,
					 mdm_data->pre_on_delay + 1);
		mcd_mdm_set_gpio_cansleep (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_POWER_OFF, pwr_on, 0);

		/* Toggle RESET_BB_N */
		usleep_range(mdm_data->on_duration, mdm_data->on_duration + 1);
		mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_ON, rst, 1);

		/* Toggle POWER_ON */
		usleep_range(PWR_ON_DELAY_2230, PWR_ON_DELAY_2230 + 1);
		mcd_mdm_set_gpio_cansleep (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_POWER_ON, pwr_on, 1);
	}
	return 0;
}

/**
 *  mcd_mdm_power_off_2230 - Perform a power off for modem 2230
 *  @drv: Reference to the driver structure
 *
 *  - Set to LOW the PWRDWN_N
 *  - Set to LOW the PWR ON pin
 */
int mcd_mdm_power_off_2230(void *data)
{
	struct mdm_info *mdm = data;

	struct cpu_ops *cpu = &mdm->pdata->cpu;

	struct mdm_ctrl_mdm_data *mdm_data = mdm->pdata->modem_data;
	void *cpu_data = mdm->pdata->cpu_data;

	int rst = cpu->get_gpio_rst(cpu_data);
	int pwr_on = cpu->get_gpio_pwr(cpu_data);
        int is_kbl =  cpu_data_is_kbl (cpu_data);

	if (((rst != INVALID_GPIO) && (pwr_on != INVALID_GPIO))||is_kbl) {
		/* Set the RESET_BB_N to 0 */
		mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_OFF, rst, 0);

		/* Wait before doing the pull down battery on */
		usleep_range(mdm_data->pre_pwr_down_delay,
			mdm_data->pre_pwr_down_delay + 1);

		mcd_mdm_set_gpio_cansleep (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_POWER_OFF, pwr_on, 0);
	}
	return 0;
}

/**
 *  mcd_mdm_power_off - Perform the NGFF modem switch OFF sequence
 *  @drv: Reference to the driver structure
 *
 *  - Set to low the RESET pin
 *  - Write the PMIC reg or the GPIO
 */
int mcd_mdm_power_off_ngff(void *data)
{
	struct mdm_info *mdm = data;

	struct cpu_ops *cpu = &mdm->pdata->cpu;
	struct pmic_ops *pmic = &mdm->pdata->pmic;

	struct mdm_ctrl_mdm_data *mdm_data = mdm->pdata->modem_data;
	void *cpu_data = mdm->pdata->cpu_data;
	void *pmic_data = mdm->pdata->pmic_data;
        int is_kbl =  cpu_data_is_kbl (cpu_data);


	int ret = 0;
	int rst = cpu->get_gpio_rst(cpu_data);
	int pwr_on = cpu->get_gpio_pwr(cpu_data);
	int pwr_on_ctrl = mdm->pdata->pwr_on_ctrl;

	if (((rst != INVALID_GPIO) && (pwr_on != INVALID_GPIO))||
             is_kbl) {
		/* Set the RESET_BB_N to 0 */
                mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_BB_RESET_OFF, rst, 0);

		/* Wait before doing the pulse on ON1 */
		usleep_range(mdm_data->pre_pwr_down_delay,
				mdm_data->pre_pwr_down_delay + 1);

		/* Toggle POWER_ON_OFF using PMIC or GPIO */
		if (pwr_on_ctrl == POWER_ON_PMIC_GPIO) {
			if (pmic->power_off_mdm) {
				ret = pmic->power_off_mdm(pmic_data);
				if (ret)
					pr_err(DRVNAME ": Error PMIC power-ON.");
			}
		} else if (pwr_on_ctrl == POWER_ON_GPIO)
                        mcd_mdm_set_gpio (mdm->pdata, is_kbl, MDM_KBL_DSM_CTRL_POWER_OFF, pwr_on, 0);
		else {
			pr_err(DRVNAME ": Error unkown power_on method");
			ret = -EINVAL;
		}

		/* Wait after power off to meet
		 * the ngff modem settle down timings
		 */
		usleep_range(POWER_OFF_DELAY_NGFF, POWER_OFF_DELAY_NGFF + 1);
	}
	return ret;
}
