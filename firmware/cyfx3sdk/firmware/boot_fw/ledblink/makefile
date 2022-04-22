## Copyright Cypress Semiconductor Corporation, 2010-2018,
## All Rights Reserved
## UNPUBLISHED, LICENSED SOFTWARE.
##
## CONFIDENTIAL AND PROPRIETARY INFORMATION
## WHICH IS THE PROPERTY OF CYPRESS.
##
## Use of this file is governed
## by the license agreement included in the file
##
##      <install>/license/license.txt
##
## where <install> is the Cypress software
## installation root directory path.
##

MODULE = BootLedBlink
FX3FWROOT = ../../..

include ${FX3FWROOT}/fw_build/boot_fw/fx3_build_config.mak

ifeq ($(CYFXBUILD), arm)
APP_ASM_SOURCE = 
else
APP_ASM_SOURCE = ./cyfx_gcc_startup.S
endif

APP_SOURCE = 			\
	     main.c

APP_OBJECT=$(APP_SOURCE:%.c=./%.o)
APP_ASM_OBJECT=$(APP_ASM_SOURCE:%.S=./%.o)

EXES = $(MODULE).$(EXEEXT)

all:compile

$(APP_ASM_OBJECT) : %.o : %.S 
	$(ASSEMBLE)

$(APP_OBJECT) : %.o : %.c 
	$(COMPILE)

$(MODULE).$(EXEEXT): $(APP_OBJECT) $(APP_ASM_OBJECT) 
	$(LINK)

clean:
	rm -f ./$(MODULE).$(EXEEXT)
	rm -f ./$(MODULE).map
	rm -f ./*.o

compile: $(APP_OBJECT) $(APP_ASM_OBJECT) $(EXES)

#[]#
