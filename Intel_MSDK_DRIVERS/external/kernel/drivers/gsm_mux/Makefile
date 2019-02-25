#
# Copyright (C) 2018 Intel Corporation
#
# SPDX-License-Identifier: GPL-2.0-only
#
DESTDIR=../../out/ModemSDK
PREFIX_LIB=/usr/local/lib

EXTRA_CFLAGS += -w -I$(PWD)/../include -I$(PWD)/../../include/uapi/linux -DMODULE_COMPILE

obj-m += gsm_mux.o

gsm_mux-objs := n_gsm.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

