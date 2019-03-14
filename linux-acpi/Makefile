obj-m := testko.o
KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

#CFLAGS   := -Werror=incompatible-pointer-types -Werror=designated-init

all:
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD)  modules
clean:
	rm *.o *.ko *.mod.c

.PHONY:clean
