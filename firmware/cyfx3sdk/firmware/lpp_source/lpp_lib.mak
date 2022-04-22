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

# Default option for Config is fx3_debug
ifeq ($(CYCONFOPT),)
        CYCONFOPT=fx3_debug
endif

# Default build option is gcc
CYFXBUILD ?= gcc

# Default SDK version is 1.3.4
CYSDKVERSION ?= 1_3_4

# now include the compile specific build options
include $(FX3FWROOT)/fw_build/fx3_fw/fx3_build_config.mak

##[]

