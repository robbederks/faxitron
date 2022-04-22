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

FX3FWROOT=../..
FX3PFWROOT=../u3p_firmware

all:compile

MODULE = cyu3lpp
LIBEXT = a

include lpp_lib.mak

SOURCE = cyu3i2s.c \
	 cyu3gpio.c \
	 cyu3gpiocomplex.c \
	 cyu3uart.c \
	 cyu3i2c.c \
	 cyu3spi.c

SOURCE_ASM=

C_OBJECT=$(SOURCE:%.c=./%.o)
A_OBJECT=$(SOURCE_ASM:%.S=./%.o)

EXES = $(MODULE).$(LIBEXT)

$(MODULE).$(LIBEXT): $(A_OBJECT) $(C_OBJECT)
	$(BDLIB)
	cp $(MODULE).$(LIBEXT) "$(FX3PFWROOT)/lib/$(CYCONFOPT)/"
	cp $(MODULE).$(LIBEXT) "$(FX3FWROOT)/fw_lib/$(CYSDKVERSION)/$(CYCONFOPT)/lib$(MODULE).$(LIBEXT)"

$(C_OBJECT) : %.o : %.c
	$(COMPILE)

$(A_OBJECT) : %.o : %.S
	$(ASSEMBLE)

clean:
	rm -f ./$(MODULE).$(LIBEXT)
	rm -f ./*.o

compile: $(C_OBJECT) $(A_OBJECT) $(EXES)

##[]

