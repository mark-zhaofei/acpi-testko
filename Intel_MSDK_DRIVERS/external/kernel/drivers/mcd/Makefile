#
# Copyright (C) 2018 Intel Corporation
#
# SPDX-License-Identifier: GPL-2.0-only
#
DESTDIR=../../out/ModemSDK
PREFIX_LIB=/usr/local/lib

EXTRA_CFLAGS += -w -I$(PWD)/../include -I$(PWD)/../../include/uapi/ -DCONFIG_MDM_CTRL_DEV_NAME=\"mdm_ctrl\"

obj-m += mcd_driver.o

mcd_driver-objs := mdm_util.o mdm_ctrl.o mcd_mdm.o mcd_cpu.o mcd_pmic.o mcd_acpi.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

