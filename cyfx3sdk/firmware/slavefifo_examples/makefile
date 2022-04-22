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
# 

FX3FWROOT=../..
include $(FX3FWROOT)/fw_build/fx3_fw/fx3_build_config.mak

all:compile

ifeq ($(CYFXEXOPT),async)
    EXSUBDIRS=slfifoasync
endif

ifeq ($(CYFXEXOPT),sync)
    EXSUBDIRS=slfifosync
endif

ifeq ($(CYFXEXOPT),async5bit)
    EXSUBDIRS=slfifoasync5bit
endif

ifeq ($(CYFXEXOPT),sync5bit)
    EXSUBDIRS=slfifosync5bit
endif

ifeq ($(CYFXEXOPT),) 
    EXSUBDIRS= slfifoasync \
	       slfifosync \
	       slfifoasync5bit \
	       slfifosync5bit
endif

clean: 
	for subdir in $(EXSUBDIRS); do \
		$(MAKE) -C $$subdir clean; \
		done

compile: eclipse_build
	for subdir in $(EXSUBDIRS); do \
		$(MAKE) -C $$subdir $@; \
		done

eclipse_build:
	for subdir in $(EXSUBDIRS); do \
		cp -f $(FX3FWROOT)/fw_build/fx3_fw/cyfx_gcc_startup.S $$subdir/. ;\
		cp -f $(FX3FWROOT)/fw_build/fx3_fw/cyfxtx.c $$subdir/. ;\
	done

eclipse_clean:
	for subdir in $(EXSUBDIRS); do \
		rm -f $$subdir/cyfx_gcc_startup.S ;\
		rm -f $$subdir/cyfxtx.c ;\
		rm -f $$subdir/Debug/* ;\
		rmdir  $$subdir/Debug ;\
		rm -f $$subdir/Release/* ;\
		rmdir  $$subdir/Release ;\
		done

##[]

