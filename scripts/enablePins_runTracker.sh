#!/bin/bash

#Path of PWM dtb
PWM_dtb=/lib/firmware/BB-PWM0-00A*

#Check PWM dtb is already installed
if [ -e $PWM_dtb ]
then
    echo "PWM related dtb already installed"
else
    echo "Installing PWM related dtb"
    mount -o rw,remount /lib/firmware/
    apt-get update
    apt install bb-cape-overlays
    mount -o ro,remount /lib/firmware/
fi

#Enable PWM
echo BB-PWM1 > /sys/devices/platform/bone_capemgr/slots

#Enable ADC
echo BB-ADC > /sys/devices/platform/bone_capemgr/slots

#Run the trackerDemo binary
./projects/spi/arm-build/trackerDemo ./projects/spi/tracker-bbgw-demo.conf
