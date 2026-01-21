#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=${DISTDIR}/Dodona.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=${DISTDIR}/Dodona.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../../common/aeneas/src/main.c ../../common/aeneas/src/adc.c ../../common/aeneas/src/burn.c ../../common/aeneas/src/clocktest.c ../../common/aeneas/src/commandmanager.c ../../common/aeneas/src/crc.c ../../common/aeneas/src/debug.c ../../common/aeneas/src/downlinkmessage.c ../../common/aeneas/src/eps.c ../../common/aeneas/src/errors.c ../../common/aeneas/src/flashmanager.c ../../common/aeneas/src/flash_AT25DF641.c ../../common/aeneas/src/fredtime.c ../../common/aeneas/src/FSW.c ../../common/aeneas/src/geomag.c ../../common/aeneas/src/GomSpace/crc32.c ../../common/aeneas/src/GomSpace/cspHeader.c ../../common/aeneas/src/GomSpace/frame.c ../../common/aeneas/src/GomSpace/kiss.c ../../common/aeneas/src/gyro.c ../../common/aeneas/src/hyperterm.c ../../common/aeneas/src/i2c.c ../../common/aeneas/src/imi100.c ../../common/aeneas/src/inet.c ../../common/aeneas/src/itag.c ../../common/aeneas/src/labview.c ../../common/aeneas/src/linkmanager.c ../../common/aeneas/src/loader_pcd.c ../../common/aeneas/src/lower_tx.c ../../common/aeneas/src/macros.c ../../common/aeneas/src/maestro.c ../../common/aeneas/src/maestro_flash.c ../../common/aeneas/src/mag.c ../../common/aeneas/src/mystack.c ../../common/aeneas/src/Neon.c ../../common/aeneas/src/nonblock.c ../../common/aeneas/src/orbit.c ../../common/aeneas/src/payloadmanager.c ../../common/aeneas/src/payload_smartsat.c ../../common/aeneas/src/ports.c ../../common/aeneas/src/schedule.c ../../common/aeneas/src/spi.c ../../common/aeneas/src/stensat.c ../../common/aeneas/src/sunsensor.c ../../common/aeneas/src/telemetry.c ../../common/aeneas/src/time.c ../../common/aeneas/src/transceiver_gomspace.c ../../common/aeneas/src/tx_manager.c ../../common/aeneas/src/uplinkmessage.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/885629990/main.o ${OBJECTDIR}/_ext/885629990/adc.o ${OBJECTDIR}/_ext/885629990/burn.o ${OBJECTDIR}/_ext/885629990/clocktest.o ${OBJECTDIR}/_ext/885629990/commandmanager.o ${OBJECTDIR}/_ext/885629990/crc.o ${OBJECTDIR}/_ext/885629990/debug.o ${OBJECTDIR}/_ext/885629990/downlinkmessage.o ${OBJECTDIR}/_ext/885629990/eps.o ${OBJECTDIR}/_ext/885629990/errors.o ${OBJECTDIR}/_ext/885629990/flashmanager.o ${OBJECTDIR}/_ext/885629990/flash_AT25DF641.o ${OBJECTDIR}/_ext/885629990/fredtime.o ${OBJECTDIR}/_ext/885629990/FSW.o ${OBJECTDIR}/_ext/885629990/geomag.o ${OBJECTDIR}/_ext/592668522/crc32.o ${OBJECTDIR}/_ext/592668522/cspHeader.o ${OBJECTDIR}/_ext/592668522/frame.o ${OBJECTDIR}/_ext/592668522/kiss.o ${OBJECTDIR}/_ext/885629990/gyro.o ${OBJECTDIR}/_ext/885629990/hyperterm.o ${OBJECTDIR}/_ext/885629990/i2c.o ${OBJECTDIR}/_ext/885629990/imi100.o ${OBJECTDIR}/_ext/885629990/inet.o ${OBJECTDIR}/_ext/885629990/itag.o ${OBJECTDIR}/_ext/885629990/labview.o ${OBJECTDIR}/_ext/885629990/linkmanager.o ${OBJECTDIR}/_ext/885629990/loader_pcd.o ${OBJECTDIR}/_ext/885629990/lower_tx.o ${OBJECTDIR}/_ext/885629990/macros.o ${OBJECTDIR}/_ext/885629990/maestro.o ${OBJECTDIR}/_ext/885629990/maestro_flash.o ${OBJECTDIR}/_ext/885629990/mag.o ${OBJECTDIR}/_ext/885629990/mystack.o ${OBJECTDIR}/_ext/885629990/Neon.o ${OBJECTDIR}/_ext/885629990/nonblock.o ${OBJECTDIR}/_ext/885629990/orbit.o ${OBJECTDIR}/_ext/885629990/payloadmanager.o ${OBJECTDIR}/_ext/885629990/payload_smartsat.o ${OBJECTDIR}/_ext/885629990/ports.o ${OBJECTDIR}/_ext/885629990/schedule.o ${OBJECTDIR}/_ext/885629990/spi.o ${OBJECTDIR}/_ext/885629990/stensat.o ${OBJECTDIR}/_ext/885629990/sunsensor.o ${OBJECTDIR}/_ext/885629990/telemetry.o ${OBJECTDIR}/_ext/885629990/time.o ${OBJECTDIR}/_ext/885629990/transceiver_gomspace.o ${OBJECTDIR}/_ext/885629990/tx_manager.o ${OBJECTDIR}/_ext/885629990/uplinkmessage.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/885629990/main.o.d ${OBJECTDIR}/_ext/885629990/adc.o.d ${OBJECTDIR}/_ext/885629990/burn.o.d ${OBJECTDIR}/_ext/885629990/clocktest.o.d ${OBJECTDIR}/_ext/885629990/commandmanager.o.d ${OBJECTDIR}/_ext/885629990/crc.o.d ${OBJECTDIR}/_ext/885629990/debug.o.d ${OBJECTDIR}/_ext/885629990/downlinkmessage.o.d ${OBJECTDIR}/_ext/885629990/eps.o.d ${OBJECTDIR}/_ext/885629990/errors.o.d ${OBJECTDIR}/_ext/885629990/flashmanager.o.d ${OBJECTDIR}/_ext/885629990/flash_AT25DF641.o.d ${OBJECTDIR}/_ext/885629990/fredtime.o.d ${OBJECTDIR}/_ext/885629990/FSW.o.d ${OBJECTDIR}/_ext/885629990/geomag.o.d ${OBJECTDIR}/_ext/592668522/crc32.o.d ${OBJECTDIR}/_ext/592668522/cspHeader.o.d ${OBJECTDIR}/_ext/592668522/frame.o.d ${OBJECTDIR}/_ext/592668522/kiss.o.d ${OBJECTDIR}/_ext/885629990/gyro.o.d ${OBJECTDIR}/_ext/885629990/hyperterm.o.d ${OBJECTDIR}/_ext/885629990/i2c.o.d ${OBJECTDIR}/_ext/885629990/imi100.o.d ${OBJECTDIR}/_ext/885629990/inet.o.d ${OBJECTDIR}/_ext/885629990/itag.o.d ${OBJECTDIR}/_ext/885629990/labview.o.d ${OBJECTDIR}/_ext/885629990/linkmanager.o.d ${OBJECTDIR}/_ext/885629990/loader_pcd.o.d ${OBJECTDIR}/_ext/885629990/lower_tx.o.d ${OBJECTDIR}/_ext/885629990/macros.o.d ${OBJECTDIR}/_ext/885629990/maestro.o.d ${OBJECTDIR}/_ext/885629990/maestro_flash.o.d ${OBJECTDIR}/_ext/885629990/mag.o.d ${OBJECTDIR}/_ext/885629990/mystack.o.d ${OBJECTDIR}/_ext/885629990/Neon.o.d ${OBJECTDIR}/_ext/885629990/nonblock.o.d ${OBJECTDIR}/_ext/885629990/orbit.o.d ${OBJECTDIR}/_ext/885629990/payloadmanager.o.d ${OBJECTDIR}/_ext/885629990/payload_smartsat.o.d ${OBJECTDIR}/_ext/885629990/ports.o.d ${OBJECTDIR}/_ext/885629990/schedule.o.d ${OBJECTDIR}/_ext/885629990/spi.o.d ${OBJECTDIR}/_ext/885629990/stensat.o.d ${OBJECTDIR}/_ext/885629990/sunsensor.o.d ${OBJECTDIR}/_ext/885629990/telemetry.o.d ${OBJECTDIR}/_ext/885629990/time.o.d ${OBJECTDIR}/_ext/885629990/transceiver_gomspace.o.d ${OBJECTDIR}/_ext/885629990/tx_manager.o.d ${OBJECTDIR}/_ext/885629990/uplinkmessage.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/885629990/main.o ${OBJECTDIR}/_ext/885629990/adc.o ${OBJECTDIR}/_ext/885629990/burn.o ${OBJECTDIR}/_ext/885629990/clocktest.o ${OBJECTDIR}/_ext/885629990/commandmanager.o ${OBJECTDIR}/_ext/885629990/crc.o ${OBJECTDIR}/_ext/885629990/debug.o ${OBJECTDIR}/_ext/885629990/downlinkmessage.o ${OBJECTDIR}/_ext/885629990/eps.o ${OBJECTDIR}/_ext/885629990/errors.o ${OBJECTDIR}/_ext/885629990/flashmanager.o ${OBJECTDIR}/_ext/885629990/flash_AT25DF641.o ${OBJECTDIR}/_ext/885629990/fredtime.o ${OBJECTDIR}/_ext/885629990/FSW.o ${OBJECTDIR}/_ext/885629990/geomag.o ${OBJECTDIR}/_ext/592668522/crc32.o ${OBJECTDIR}/_ext/592668522/cspHeader.o ${OBJECTDIR}/_ext/592668522/frame.o ${OBJECTDIR}/_ext/592668522/kiss.o ${OBJECTDIR}/_ext/885629990/gyro.o ${OBJECTDIR}/_ext/885629990/hyperterm.o ${OBJECTDIR}/_ext/885629990/i2c.o ${OBJECTDIR}/_ext/885629990/imi100.o ${OBJECTDIR}/_ext/885629990/inet.o ${OBJECTDIR}/_ext/885629990/itag.o ${OBJECTDIR}/_ext/885629990/labview.o ${OBJECTDIR}/_ext/885629990/linkmanager.o ${OBJECTDIR}/_ext/885629990/loader_pcd.o ${OBJECTDIR}/_ext/885629990/lower_tx.o ${OBJECTDIR}/_ext/885629990/macros.o ${OBJECTDIR}/_ext/885629990/maestro.o ${OBJECTDIR}/_ext/885629990/maestro_flash.o ${OBJECTDIR}/_ext/885629990/mag.o ${OBJECTDIR}/_ext/885629990/mystack.o ${OBJECTDIR}/_ext/885629990/Neon.o ${OBJECTDIR}/_ext/885629990/nonblock.o ${OBJECTDIR}/_ext/885629990/orbit.o ${OBJECTDIR}/_ext/885629990/payloadmanager.o ${OBJECTDIR}/_ext/885629990/payload_smartsat.o ${OBJECTDIR}/_ext/885629990/ports.o ${OBJECTDIR}/_ext/885629990/schedule.o ${OBJECTDIR}/_ext/885629990/spi.o ${OBJECTDIR}/_ext/885629990/stensat.o ${OBJECTDIR}/_ext/885629990/sunsensor.o ${OBJECTDIR}/_ext/885629990/telemetry.o ${OBJECTDIR}/_ext/885629990/time.o ${OBJECTDIR}/_ext/885629990/transceiver_gomspace.o ${OBJECTDIR}/_ext/885629990/tx_manager.o ${OBJECTDIR}/_ext/885629990/uplinkmessage.o

# Source Files
SOURCEFILES=../../common/aeneas/src/main.c ../../common/aeneas/src/adc.c ../../common/aeneas/src/burn.c ../../common/aeneas/src/clocktest.c ../../common/aeneas/src/commandmanager.c ../../common/aeneas/src/crc.c ../../common/aeneas/src/debug.c ../../common/aeneas/src/downlinkmessage.c ../../common/aeneas/src/eps.c ../../common/aeneas/src/errors.c ../../common/aeneas/src/flashmanager.c ../../common/aeneas/src/flash_AT25DF641.c ../../common/aeneas/src/fredtime.c ../../common/aeneas/src/FSW.c ../../common/aeneas/src/geomag.c ../../common/aeneas/src/GomSpace/crc32.c ../../common/aeneas/src/GomSpace/cspHeader.c ../../common/aeneas/src/GomSpace/frame.c ../../common/aeneas/src/GomSpace/kiss.c ../../common/aeneas/src/gyro.c ../../common/aeneas/src/hyperterm.c ../../common/aeneas/src/i2c.c ../../common/aeneas/src/imi100.c ../../common/aeneas/src/inet.c ../../common/aeneas/src/itag.c ../../common/aeneas/src/labview.c ../../common/aeneas/src/linkmanager.c ../../common/aeneas/src/loader_pcd.c ../../common/aeneas/src/lower_tx.c ../../common/aeneas/src/macros.c ../../common/aeneas/src/maestro.c ../../common/aeneas/src/maestro_flash.c ../../common/aeneas/src/mag.c ../../common/aeneas/src/mystack.c ../../common/aeneas/src/Neon.c ../../common/aeneas/src/nonblock.c ../../common/aeneas/src/orbit.c ../../common/aeneas/src/payloadmanager.c ../../common/aeneas/src/payload_smartsat.c ../../common/aeneas/src/ports.c ../../common/aeneas/src/schedule.c ../../common/aeneas/src/spi.c ../../common/aeneas/src/stensat.c ../../common/aeneas/src/sunsensor.c ../../common/aeneas/src/telemetry.c ../../common/aeneas/src/time.c ../../common/aeneas/src/transceiver_gomspace.c ../../common/aeneas/src/tx_manager.c ../../common/aeneas/src/uplinkmessage.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/Dodona.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_CC="C:\Program Files (x86)\PICC\CCSCON.exe"
MP_LD="C:\Program Files (x86)\PICC\CCSCON.exe"
sourceline.device=sourceline="\#device PIC24FJ256GA110"
sourceline.xccompat=sourceline="\#device ANSI" sourceline="\#device PASS_STRINGS=IN_RAM" sourceline="\#device CONST=READ_ONLY" sourceline="\#case" sourceline="\#TYPE SIGNED" sourceline="\#type INT=16, LONG=32"
sourceline.gcccompat=sourceline="\#device ANSI" sourceline="\#device PASS_STRINGS=IN_RAM" sourceline="\#device CONST=READ_ONLY" sourceline="\#case" sourceline="\#TYPE SIGNED"
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/885629990/main.o: ../../common/aeneas/src/main.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/main.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/main.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/main.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/main.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/adc.o: ../../common/aeneas/src/adc.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/adc.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/adc.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/adc.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/adc.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/burn.o: ../../common/aeneas/src/burn.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/burn.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/burn.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/burn.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/burn.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/clocktest.o: ../../common/aeneas/src/clocktest.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/clocktest.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/clocktest.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/clocktest.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/clocktest.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/commandmanager.o: ../../common/aeneas/src/commandmanager.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/commandmanager.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/commandmanager.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/commandmanager.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/commandmanager.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/crc.o: ../../common/aeneas/src/crc.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/crc.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/crc.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/crc.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/crc.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/debug.o: ../../common/aeneas/src/debug.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/debug.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/debug.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/debug.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/debug.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/downlinkmessage.o: ../../common/aeneas/src/downlinkmessage.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/downlinkmessage.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/downlinkmessage.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/downlinkmessage.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/downlinkmessage.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/eps.o: ../../common/aeneas/src/eps.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/eps.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/eps.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/eps.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/eps.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/errors.o: ../../common/aeneas/src/errors.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/errors.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/errors.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/errors.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/errors.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/flashmanager.o: ../../common/aeneas/src/flashmanager.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/flashmanager.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/flashmanager.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/flashmanager.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/flashmanager.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/flash_AT25DF641.o: ../../common/aeneas/src/flash_AT25DF641.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/flash_AT25DF641.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/flash_AT25DF641.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/flash_AT25DF641.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/flash_AT25DF641.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/fredtime.o: ../../common/aeneas/src/fredtime.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/fredtime.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/fredtime.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/fredtime.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/fredtime.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/FSW.o: ../../common/aeneas/src/FSW.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/FSW.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/FSW.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/FSW.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/FSW.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/geomag.o: ../../common/aeneas/src/geomag.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/geomag.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/geomag.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/geomag.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/geomag.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/592668522/crc32.o: ../../common/aeneas/src/GomSpace/crc32.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/592668522 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\592668522"  ../../common/aeneas/src/GomSpace/crc32.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/592668522/crc32.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/592668522/crc32.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\592668522"" ../../common/aeneas/src/GomSpace/crc32.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/592668522"  
	
endif 
	
${OBJECTDIR}/_ext/592668522/cspHeader.o: ../../common/aeneas/src/GomSpace/cspHeader.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/592668522 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\592668522"  ../../common/aeneas/src/GomSpace/cspHeader.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/592668522/cspHeader.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/592668522/cspHeader.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\592668522"" ../../common/aeneas/src/GomSpace/cspHeader.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/592668522"  
	
endif 
	
${OBJECTDIR}/_ext/592668522/frame.o: ../../common/aeneas/src/GomSpace/frame.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/592668522 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\592668522"  ../../common/aeneas/src/GomSpace/frame.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/592668522/frame.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/592668522/frame.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\592668522"" ../../common/aeneas/src/GomSpace/frame.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/592668522"  
	
endif 
	
${OBJECTDIR}/_ext/592668522/kiss.o: ../../common/aeneas/src/GomSpace/kiss.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/592668522 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\592668522"  ../../common/aeneas/src/GomSpace/kiss.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/592668522/kiss.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/592668522/kiss.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\592668522"" ../../common/aeneas/src/GomSpace/kiss.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/592668522"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/gyro.o: ../../common/aeneas/src/gyro.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/gyro.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/gyro.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/gyro.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/gyro.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/hyperterm.o: ../../common/aeneas/src/hyperterm.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/hyperterm.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/hyperterm.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/hyperterm.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/hyperterm.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/i2c.o: ../../common/aeneas/src/i2c.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/i2c.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/i2c.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/i2c.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/i2c.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/imi100.o: ../../common/aeneas/src/imi100.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/imi100.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/imi100.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/imi100.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/imi100.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/inet.o: ../../common/aeneas/src/inet.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/inet.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/inet.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/inet.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/inet.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/itag.o: ../../common/aeneas/src/itag.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/itag.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/itag.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/itag.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/itag.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/labview.o: ../../common/aeneas/src/labview.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/labview.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/labview.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/labview.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/labview.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/linkmanager.o: ../../common/aeneas/src/linkmanager.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/linkmanager.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/linkmanager.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/linkmanager.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/linkmanager.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/loader_pcd.o: ../../common/aeneas/src/loader_pcd.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/loader_pcd.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/loader_pcd.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/loader_pcd.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/loader_pcd.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/lower_tx.o: ../../common/aeneas/src/lower_tx.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/lower_tx.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/lower_tx.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/lower_tx.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/lower_tx.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/macros.o: ../../common/aeneas/src/macros.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/macros.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/macros.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/macros.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/macros.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/maestro.o: ../../common/aeneas/src/maestro.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/maestro.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/maestro.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/maestro.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/maestro.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/maestro_flash.o: ../../common/aeneas/src/maestro_flash.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/maestro_flash.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/maestro_flash.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/maestro_flash.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/maestro_flash.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/mag.o: ../../common/aeneas/src/mag.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/mag.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/mag.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/mag.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/mag.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/mystack.o: ../../common/aeneas/src/mystack.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/mystack.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/mystack.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/mystack.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/mystack.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/Neon.o: ../../common/aeneas/src/Neon.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/Neon.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/Neon.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/Neon.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/Neon.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/nonblock.o: ../../common/aeneas/src/nonblock.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/nonblock.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/nonblock.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/nonblock.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/nonblock.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/orbit.o: ../../common/aeneas/src/orbit.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/orbit.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/orbit.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/orbit.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/orbit.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/payloadmanager.o: ../../common/aeneas/src/payloadmanager.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/payloadmanager.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/payloadmanager.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/payloadmanager.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/payloadmanager.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/payload_smartsat.o: ../../common/aeneas/src/payload_smartsat.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/payload_smartsat.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/payload_smartsat.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/payload_smartsat.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/payload_smartsat.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/ports.o: ../../common/aeneas/src/ports.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/ports.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/ports.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/ports.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/ports.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/schedule.o: ../../common/aeneas/src/schedule.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/schedule.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/schedule.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/schedule.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/schedule.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/spi.o: ../../common/aeneas/src/spi.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/spi.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/spi.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/spi.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/spi.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/stensat.o: ../../common/aeneas/src/stensat.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/stensat.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/stensat.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/stensat.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/stensat.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/sunsensor.o: ../../common/aeneas/src/sunsensor.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/sunsensor.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/sunsensor.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/sunsensor.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/sunsensor.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/telemetry.o: ../../common/aeneas/src/telemetry.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/telemetry.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/telemetry.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/telemetry.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/telemetry.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/time.o: ../../common/aeneas/src/time.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/time.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/time.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/time.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/time.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/transceiver_gomspace.o: ../../common/aeneas/src/transceiver_gomspace.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/transceiver_gomspace.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/transceiver_gomspace.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/transceiver_gomspace.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/transceiver_gomspace.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/tx_manager.o: ../../common/aeneas/src/tx_manager.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/tx_manager.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/tx_manager.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/tx_manager.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/tx_manager.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/uplinkmessage.o: ../../common/aeneas/src/uplinkmessage.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/uplinkmessage.c +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/uplinkmessage.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/uplinkmessage.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/uplinkmessage.c +EXPORT +FD +DF +CC #__DEBUG=1 +ICD +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
else
${OBJECTDIR}/_ext/885629990/main.o: ../../common/aeneas/src/main.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/main.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/main.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/main.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/main.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/adc.o: ../../common/aeneas/src/adc.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/adc.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/adc.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/adc.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/adc.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/burn.o: ../../common/aeneas/src/burn.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/burn.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/burn.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/burn.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/burn.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/clocktest.o: ../../common/aeneas/src/clocktest.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/clocktest.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/clocktest.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/clocktest.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/clocktest.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/commandmanager.o: ../../common/aeneas/src/commandmanager.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/commandmanager.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/commandmanager.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/commandmanager.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/commandmanager.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/crc.o: ../../common/aeneas/src/crc.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/crc.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/crc.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/crc.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/crc.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/debug.o: ../../common/aeneas/src/debug.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/debug.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/debug.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/debug.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/debug.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/downlinkmessage.o: ../../common/aeneas/src/downlinkmessage.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/downlinkmessage.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/downlinkmessage.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/downlinkmessage.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/downlinkmessage.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/eps.o: ../../common/aeneas/src/eps.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/eps.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/eps.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/eps.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/eps.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/errors.o: ../../common/aeneas/src/errors.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/errors.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/errors.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/errors.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/errors.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/flashmanager.o: ../../common/aeneas/src/flashmanager.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/flashmanager.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/flashmanager.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/flashmanager.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/flashmanager.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/flash_AT25DF641.o: ../../common/aeneas/src/flash_AT25DF641.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/flash_AT25DF641.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/flash_AT25DF641.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/flash_AT25DF641.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/flash_AT25DF641.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/fredtime.o: ../../common/aeneas/src/fredtime.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/fredtime.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/fredtime.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/fredtime.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/fredtime.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/FSW.o: ../../common/aeneas/src/FSW.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/FSW.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/FSW.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/FSW.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/FSW.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/geomag.o: ../../common/aeneas/src/geomag.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/geomag.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/geomag.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/geomag.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/geomag.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/592668522/crc32.o: ../../common/aeneas/src/GomSpace/crc32.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/592668522 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\592668522"  ../../common/aeneas/src/GomSpace/crc32.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/592668522/crc32.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/592668522/crc32.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\592668522"" ../../common/aeneas/src/GomSpace/crc32.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/592668522"  
	
endif 
	
${OBJECTDIR}/_ext/592668522/cspHeader.o: ../../common/aeneas/src/GomSpace/cspHeader.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/592668522 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\592668522"  ../../common/aeneas/src/GomSpace/cspHeader.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/592668522/cspHeader.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/592668522/cspHeader.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\592668522"" ../../common/aeneas/src/GomSpace/cspHeader.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/592668522"  
	
endif 
	
${OBJECTDIR}/_ext/592668522/frame.o: ../../common/aeneas/src/GomSpace/frame.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/592668522 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\592668522"  ../../common/aeneas/src/GomSpace/frame.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/592668522/frame.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/592668522/frame.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\592668522"" ../../common/aeneas/src/GomSpace/frame.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/592668522"  
	
endif 
	
${OBJECTDIR}/_ext/592668522/kiss.o: ../../common/aeneas/src/GomSpace/kiss.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/592668522 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\592668522"  ../../common/aeneas/src/GomSpace/kiss.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/592668522/kiss.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/592668522/kiss.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\592668522"" ../../common/aeneas/src/GomSpace/kiss.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/592668522"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/gyro.o: ../../common/aeneas/src/gyro.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/gyro.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/gyro.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/gyro.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/gyro.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/hyperterm.o: ../../common/aeneas/src/hyperterm.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/hyperterm.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/hyperterm.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/hyperterm.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/hyperterm.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/i2c.o: ../../common/aeneas/src/i2c.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/i2c.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/i2c.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/i2c.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/i2c.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/imi100.o: ../../common/aeneas/src/imi100.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/imi100.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/imi100.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/imi100.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/imi100.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/inet.o: ../../common/aeneas/src/inet.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/inet.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/inet.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/inet.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/inet.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/itag.o: ../../common/aeneas/src/itag.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/itag.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/itag.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/itag.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/itag.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/labview.o: ../../common/aeneas/src/labview.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/labview.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/labview.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/labview.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/labview.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/linkmanager.o: ../../common/aeneas/src/linkmanager.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/linkmanager.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/linkmanager.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/linkmanager.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/linkmanager.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/loader_pcd.o: ../../common/aeneas/src/loader_pcd.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/loader_pcd.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/loader_pcd.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/loader_pcd.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/loader_pcd.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/lower_tx.o: ../../common/aeneas/src/lower_tx.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/lower_tx.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/lower_tx.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/lower_tx.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/lower_tx.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/macros.o: ../../common/aeneas/src/macros.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/macros.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/macros.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/macros.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/macros.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/maestro.o: ../../common/aeneas/src/maestro.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/maestro.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/maestro.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/maestro.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/maestro.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/maestro_flash.o: ../../common/aeneas/src/maestro_flash.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/maestro_flash.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/maestro_flash.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/maestro_flash.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/maestro_flash.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/mag.o: ../../common/aeneas/src/mag.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/mag.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/mag.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/mag.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/mag.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/mystack.o: ../../common/aeneas/src/mystack.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/mystack.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/mystack.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/mystack.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/mystack.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/Neon.o: ../../common/aeneas/src/Neon.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/Neon.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/Neon.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/Neon.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/Neon.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/nonblock.o: ../../common/aeneas/src/nonblock.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/nonblock.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/nonblock.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/nonblock.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/nonblock.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/orbit.o: ../../common/aeneas/src/orbit.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/orbit.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/orbit.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/orbit.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/orbit.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/payloadmanager.o: ../../common/aeneas/src/payloadmanager.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/payloadmanager.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/payloadmanager.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/payloadmanager.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/payloadmanager.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/payload_smartsat.o: ../../common/aeneas/src/payload_smartsat.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/payload_smartsat.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/payload_smartsat.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/payload_smartsat.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/payload_smartsat.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/ports.o: ../../common/aeneas/src/ports.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/ports.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/ports.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/ports.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/ports.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/schedule.o: ../../common/aeneas/src/schedule.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/schedule.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/schedule.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/schedule.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/schedule.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/spi.o: ../../common/aeneas/src/spi.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/spi.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/spi.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/spi.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/spi.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/stensat.o: ../../common/aeneas/src/stensat.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/stensat.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/stensat.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/stensat.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/stensat.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/sunsensor.o: ../../common/aeneas/src/sunsensor.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/sunsensor.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/sunsensor.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/sunsensor.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/sunsensor.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/telemetry.o: ../../common/aeneas/src/telemetry.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/telemetry.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/telemetry.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/telemetry.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/telemetry.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/time.o: ../../common/aeneas/src/time.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/time.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/time.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/time.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/time.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/transceiver_gomspace.o: ../../common/aeneas/src/transceiver_gomspace.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/transceiver_gomspace.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/transceiver_gomspace.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/transceiver_gomspace.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/transceiver_gomspace.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/tx_manager.o: ../../common/aeneas/src/tx_manager.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/tx_manager.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/tx_manager.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/tx_manager.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/tx_manager.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
${OBJECTDIR}/_ext/885629990/uplinkmessage.o: ../../common/aeneas/src/uplinkmessage.c  nbproject/Makefile-${CND_CONF}.mk 
	${MKDIR} ${OBJECTDIR}/_ext/885629990 
ifeq (50,1) 
	${MKDIR} ${DISTDIR} 
	${MP_CC}  out="${OBJECTDIR}\_ext\885629990"  ../../common/aeneas/src/uplinkmessage.c +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 
	@mv ${OBJECTDIR}/_ext/885629990/uplinkmessage.cof "${DISTDIR}/Dodona.${IMAGE_TYPE}.cof" 
	@mv ${OBJECTDIR}/_ext/885629990/uplinkmessage.hex "${DISTDIR}/Dodona.${IMAGE_TYPE}.hex"
else 
	${MP_CC}  out=""${OBJECTDIR}\_ext\885629990"" ../../common/aeneas/src/uplinkmessage.c +EXPORT +FD +DF +CC +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P #__PIC24FJ256GA110__=1 +EXPORTD="${OBJECTDIR}/_ext/885629990"  
	
endif 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/Dodona.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	${MKDIR} ${DISTDIR} 
ifeq (50,1) 
	
else 
	${MP_LD}   out="${DISTDIR}"  +FD +DF +CC LINK=Dodona.${IMAGE_TYPE}.hex=${OBJECTDIR}/_ext/885629990/main.o,${OBJECTDIR}/_ext/885629990/adc.o,${OBJECTDIR}/_ext/885629990/burn.o,${OBJECTDIR}/_ext/885629990/clocktest.o,${OBJECTDIR}/_ext/885629990/commandmanager.o,${OBJECTDIR}/_ext/885629990/crc.o,${OBJECTDIR}/_ext/885629990/debug.o,${OBJECTDIR}/_ext/885629990/downlinkmessage.o,${OBJECTDIR}/_ext/885629990/eps.o,${OBJECTDIR}/_ext/885629990/errors.o,${OBJECTDIR}/_ext/885629990/flashmanager.o,${OBJECTDIR}/_ext/885629990/flash_AT25DF641.o,${OBJECTDIR}/_ext/885629990/fredtime.o,${OBJECTDIR}/_ext/885629990/FSW.o,${OBJECTDIR}/_ext/885629990/geomag.o,${OBJECTDIR}/_ext/592668522/crc32.o,${OBJECTDIR}/_ext/592668522/cspHeader.o,${OBJECTDIR}/_ext/592668522/frame.o,${OBJECTDIR}/_ext/592668522/kiss.o,${OBJECTDIR}/_ext/885629990/gyro.o,${OBJECTDIR}/_ext/885629990/hyperterm.o,${OBJECTDIR}/_ext/885629990/i2c.o,${OBJECTDIR}/_ext/885629990/imi100.o,${OBJECTDIR}/_ext/885629990/inet.o,${OBJECTDIR}/_ext/885629990/itag.o,${OBJECTDIR}/_ext/885629990/labview.o,${OBJECTDIR}/_ext/885629990/linkmanager.o,${OBJECTDIR}/_ext/885629990/loader_pcd.o,${OBJECTDIR}/_ext/885629990/lower_tx.o,${OBJECTDIR}/_ext/885629990/macros.o,${OBJECTDIR}/_ext/885629990/maestro.o,${OBJECTDIR}/_ext/885629990/maestro_flash.o,${OBJECTDIR}/_ext/885629990/mag.o,${OBJECTDIR}/_ext/885629990/mystack.o,${OBJECTDIR}/_ext/885629990/Neon.o,${OBJECTDIR}/_ext/885629990/nonblock.o,${OBJECTDIR}/_ext/885629990/orbit.o,${OBJECTDIR}/_ext/885629990/payloadmanager.o,${OBJECTDIR}/_ext/885629990/payload_smartsat.o,${OBJECTDIR}/_ext/885629990/ports.o,${OBJECTDIR}/_ext/885629990/schedule.o,${OBJECTDIR}/_ext/885629990/spi.o,${OBJECTDIR}/_ext/885629990/stensat.o,${OBJECTDIR}/_ext/885629990/sunsensor.o,${OBJECTDIR}/_ext/885629990/telemetry.o,${OBJECTDIR}/_ext/885629990/time.o,${OBJECTDIR}/_ext/885629990/transceiver_gomspace.o,${OBJECTDIR}/_ext/885629990/tx_manager.o,${OBJECTDIR}/_ext/885629990/uplinkmessage.o +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P
	
endif 
	
else
${DISTDIR}/Dodona.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	${MKDIR} ${DISTDIR} 
ifeq (50,1) 
	
else 
	${MP_LD}   out="${DISTDIR}"  +FD +DF +CC LINK=Dodona.${IMAGE_TYPE}.hex=${OBJECTDIR}/_ext/885629990/main.o,${OBJECTDIR}/_ext/885629990/adc.o,${OBJECTDIR}/_ext/885629990/burn.o,${OBJECTDIR}/_ext/885629990/clocktest.o,${OBJECTDIR}/_ext/885629990/commandmanager.o,${OBJECTDIR}/_ext/885629990/crc.o,${OBJECTDIR}/_ext/885629990/debug.o,${OBJECTDIR}/_ext/885629990/downlinkmessage.o,${OBJECTDIR}/_ext/885629990/eps.o,${OBJECTDIR}/_ext/885629990/errors.o,${OBJECTDIR}/_ext/885629990/flashmanager.o,${OBJECTDIR}/_ext/885629990/flash_AT25DF641.o,${OBJECTDIR}/_ext/885629990/fredtime.o,${OBJECTDIR}/_ext/885629990/FSW.o,${OBJECTDIR}/_ext/885629990/geomag.o,${OBJECTDIR}/_ext/592668522/crc32.o,${OBJECTDIR}/_ext/592668522/cspHeader.o,${OBJECTDIR}/_ext/592668522/frame.o,${OBJECTDIR}/_ext/592668522/kiss.o,${OBJECTDIR}/_ext/885629990/gyro.o,${OBJECTDIR}/_ext/885629990/hyperterm.o,${OBJECTDIR}/_ext/885629990/i2c.o,${OBJECTDIR}/_ext/885629990/imi100.o,${OBJECTDIR}/_ext/885629990/inet.o,${OBJECTDIR}/_ext/885629990/itag.o,${OBJECTDIR}/_ext/885629990/labview.o,${OBJECTDIR}/_ext/885629990/linkmanager.o,${OBJECTDIR}/_ext/885629990/loader_pcd.o,${OBJECTDIR}/_ext/885629990/lower_tx.o,${OBJECTDIR}/_ext/885629990/macros.o,${OBJECTDIR}/_ext/885629990/maestro.o,${OBJECTDIR}/_ext/885629990/maestro_flash.o,${OBJECTDIR}/_ext/885629990/mag.o,${OBJECTDIR}/_ext/885629990/mystack.o,${OBJECTDIR}/_ext/885629990/Neon.o,${OBJECTDIR}/_ext/885629990/nonblock.o,${OBJECTDIR}/_ext/885629990/orbit.o,${OBJECTDIR}/_ext/885629990/payloadmanager.o,${OBJECTDIR}/_ext/885629990/payload_smartsat.o,${OBJECTDIR}/_ext/885629990/ports.o,${OBJECTDIR}/_ext/885629990/schedule.o,${OBJECTDIR}/_ext/885629990/spi.o,${OBJECTDIR}/_ext/885629990/stensat.o,${OBJECTDIR}/_ext/885629990/sunsensor.o,${OBJECTDIR}/_ext/885629990/telemetry.o,${OBJECTDIR}/_ext/885629990/time.o,${OBJECTDIR}/_ext/885629990/transceiver_gomspace.o,${OBJECTDIR}/_ext/885629990/tx_manager.o,${OBJECTDIR}/_ext/885629990/uplinkmessage.o +Y=9 +EA I+="C:\Program Files (x86)\PICC\Drivers" I+="C:\Program Files (x86)\PICC\Devices" I+="..\..\common\aeneas" +DF +LN +T +A +M +J +Z -P
	
endif 
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(wildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
