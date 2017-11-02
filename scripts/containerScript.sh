#!/bin/bash

# Path of PWM dtb
PWM0_dtb=/lib/firmware/BB-PWM0-00A0.dtbo
PWM1_dtb=/lib/firmware/BB-PWM1-00A0.dtbo
PWM2_dtb=/lib/firmware/BB-PWM2-00A0.dtbo

# Check PWM dtb is already installed
if [[ -e $PWM0_dtb && -e $PWM1_dtb && -e $PWM2_dtb ]]
then
    echo "PWM related dtb already installed"
else
    echo "Installing PWM related dtb"
    mount -o rw,remount /lib/firmware/
    apt-get update
    apt install bb-cape-overlays
    mount -o ro,remount /lib/firmware/
fi

# Enable PWM
echo BB-PWM1 > /sys/devices/platform/bone_capemgr/slots

# Enable ADC
echo BB-ADC > /sys/devices/platform/bone_capemgr/slots

# Mosquitto Library Build
MOSQUITTO_LIB=/usr/local/lib/libmosquitto.so
MOSQUITTO_INC=/usr/local/include/mosquitto.h

if [[ -e $MOSQUITTO_LIB && -e $MOSQUITTO_LIB.1 && -e $MOSQUITTO_INC ]]
then
    echo "Mosquttio Library already exist"
else
    cd /tracker-demo/Common/lib/mosquitto/
    make clean; make all

    # Install Mosquitto library
    install -d /usr/local/lib/
    install -s --strip-program=strip lib/libmosquitto.so.1 $MOSQUITTO_LIB.1
    ln -sf $MOSQUITTO_LIB.1 $MOSQUITTO_LIB

    # Install Mosquitto header file
    install -d /usr/local/include/
    install lib/mosquitto.h $MOSQUITTO_INC
    ldconfig
fi

# JSMN Library Build
JSMN_LIB=/usr/local/lib/libjsmn.a
JSMN_INC=/usr/local/include/jsmn.h

if [[ -e $JSMN_LIB && -e $JSMN_INC ]]
then
    echo "JSMN Library already exist"
else
    cd /tracker-demo/Common/lib/jsmn/
    make clean;make all

    # Install JSMN library
    install -d /usr/local/lib/
    install libjsmn.a $JSMN_LIB

    # Install JSMN header file
    install -d /usr/local/include/
    install jsmn.h $JSMN_INC
    ldconfig
fi

# Build trackerDemo
cd /tracker-demo

gcc -std=c11 -DPRINT_LOG_MSG -I/usr/local/include -I./Common -I./hw -I./projects/spi/include -O0 -g3 -Wall -fmessage-length=0 -o projects/spi/arm-build/trackerDemo projects/spi/src/trackerDemo.c Common/utilities/utilities.c hw/BBGW/GPIO/GPIO.c projects/spi/include/*.c Common/mosquitto/mosquittoClient.c -lpthread -L/usr/local/lib -lmosquitto -ljsmn

# Run the trackerDemo binary
./projects/spi/arm-build/trackerDemo ./projects/spi/tracker-bbgw-demo.conf
