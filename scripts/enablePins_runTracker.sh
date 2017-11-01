#!/bin/bash

#Install PWM dtb
mount -o rw,remount /lib/firmware/
apt-get update
apt install bb-cape-overlays

echo BB-PWM1 > /sys/devices/platform/bone_capemgr/slots
mount -o ro,remount /lib/firmware/

#Enable ADC
echo BB-ADC > /sys/devices/platform/bone_capemgr/slots

./projects/spi/arm-build/trackerDemo ./projects/spi/tracker-bbgw-demo.conf
