#
# Copyright (C) 2018 Intel Corporation
#
# SPDX-License-Identifier: GPL-2.0-only
#
.PHONY: all clean install


DESTDIR=${msdk_build_path}/../out/ModemSDK
PREFIX=/etc/telephony/msdk/iosm
PREFIX_BIN=/usr/local/bin
IOSM_DRIVER_KO_PATH=${msdk_build_path}/../../external/kernel/drivers/iosm
MCD_DRIVER_KO_PATH=${msdk_build_path}/../../external/kernel/drivers/mcd
MUX_DRIVER_KO_PATH=${msdk_build_path}/../../external/kernel/drivers/gsm_mux

IOSM_DRIVER=imc_ipc.ko
MCD_DRIVER=mcd_driver.ko
GSM_MUX_DRIVER=gsm_mux.ko

all: 	

clean:
	rm -f *.o 

install:
	mkdir -p $(DESTDIR)$(PREFIX)
	mkdir -p $(DESTDIR)$(PREFIX_BIN)
	cp -f ${IOSM_DRIVER_KO_PATH}/*imc_start* $(DESTDIR)$(PREFIX)
	cp -f ${IOSM_DRIVER_KO_PATH}/*ipc_config* $(DESTDIR)$(PREFIX)
	cp -f ${IOSM_DRIVER_KO_PATH}/*wwan_* $(DESTDIR)$(PREFIX)
	cp -f ${IOSM_DRIVER_KO_PATH}/${IOSM_DRIVER}  $(DESTDIR)$(PREFIX_BIN)
	cp -f ${MCD_DRIVER_KO_PATH}/${MCD_DRIVER}  $(DESTDIR)$(PREFIX_BIN)
	cp -f ${MUX_DRIVER_KO_PATH}/${GSM_MUX_DRIVER}  $(DESTDIR)$(PREFIX_BIN)





