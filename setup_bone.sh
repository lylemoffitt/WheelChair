#!/bin/bash
SLOTS='/sys/devices/bone_capemgr.9/slots'
### This is the master setup script for the beaglebone pins.
### It must be run (with root permissions) *every* time the BB is rebooted. 

######################             MOTOR           ############################
# Export the PWM related Device Trees
pwm_ls=(                \
# Motor[0] ("LEFT")
        bone_pwm_P8_45  \
# Motor[1] ("RIGHT")
        bone_pwm_P9_31  \
# Unifying Controller
        am33xx_pwm      \
)

for pwm in ${pwm_ls[@]}; do
        echo $pwm > $SLOTS
done

######################             GPIOs           ############################
# Export the General Purpose I/O pins
gpio_ls=(       \
# 2 pins are used for Motor Left/Right Enable
        69 74   \
# The rest are used for control buttons
#        73 72 66 75 71 70
)

for pin in ${gpio_ls[@]}; do
        echo $pin > $GPIO/export
done

######################             LIDAR           ############################
# Setup the TeleTYpe device (/dev/ttyO1) 
echo BB-UART1 > $SLOTS
stty -F /dev/ttyO1 115200  raw   time 1   min 0

######################          MICROPROCESSOR          ########################
# Terminal to handle communication with HCS12 for the wheel encoders
echo BB-UART2 > $SLOTS
stty -F /dev/ttyO1 115200 raw 						\
-brkint -icrnl -imaxbel -opost 						\
-onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke
