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

ifeq ($(CYFXEXOPT),cyfx_rtos_example)
    EXSUBDIRS=cyfx_rtos_example
endif

ifeq ($(CYFXEXOPT),bulklpautoenum)
    EXSUBDIRS=cyfxbulklpautoenum
endif

ifeq ($(CYFXEXOPT),cyfxbulklpotg)
    EXSUBDIRS=cyfxbulklpotg
endif

ifeq ($(CYFXEXOPT),bulksrcsink)
    EXSUBDIRS=cyfxbulksrcsink
endif

ifeq ($(CYFXEXOPT),flashprog)
    EXSUBDIRS=cyfxflashprog
endif

ifeq ($(CYFXEXOPT),cyfxgpiftousb)
    EXSUBDIRS=cyfxgpiftousb
endif

ifeq ($(CYFXEXOPT),isolpauto)
    EXSUBDIRS=cyfxisolpauto
endif

ifeq ($(CYFXEXOPT),isolpmaninout)
    EXSUBDIRS=cyfxisolpmaninout
endif

ifeq ($(CYFXEXOPT),isosrc)
    EXSUBDIRS=cyfxisosrc
endif

ifeq ($(CYFXEXOPT),isosrcsink)
    EXSUBDIRS=cyfxisosrcsink
endif

ifeq ($(CYFXEXOPT),cyfxusbdebug)
    EXSUBDIRS=cyfxusbdebug
endif

ifeq ($(CYFXEXOPT),cyfxusbhost)
    EXSUBDIRS=cyfxusbhost
endif

ifeq ($(CYFXEXOPT),cyfxusbotg)
    EXSUBDIRS=cyfxusbotg
endif

ifeq ($(CYFXEXOPT),cyfxlowpowertest)
    EXSUBDIRS=cyfxlowpowertest
endif

ifeq ($(CYFXEXOPT),) 
    EXSUBDIRS= cyfx_rtos_example \
	       cyfxbulklpautoenum \
	       cyfxbulklpotg \
	       cyfxbulksrcsink \
	       cyfxflashprog \
	       cyfxgpiftousb \
	       cyfxisolpauto \
	       cyfxisolpmaninout \
	       cyfxisosrc \
	       cyfxisosrcsink \
	       cyfxusbdebug \
	       cyfxusbhost \
	       cyfxusbotg \
	       cyfxlowpowertest
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
	cp -f $(FX3FWROOT)/fw_build/fx3_fw/cyfx_gcc_startup.S cyfxbulklpauto_cpp/.;
	cp -f $(FX3FWROOT)/fw_build/fx3_fw/cyfxtx.cpp cyfxbulklpauto_cpp/.;
	cp -f $(FX3FWROOT)/fw_build/fx3_fw/cyfxcppsyscall.cpp cyfxbulklpauto_cpp/.;
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

