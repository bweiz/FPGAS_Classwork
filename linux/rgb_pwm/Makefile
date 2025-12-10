ifneq ($(KERNELRELEASE),)
obj-m := rgb_pwm.o

else

KDIR ?= $(HOME)/eele467/linux-socfpga


default:
	$(MAKE) -C $(KDIR) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- M=$$PWD

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif