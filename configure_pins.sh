#!/bin/bash

#### if debugging use: 
# set -xvB

### This is the master setup script for the beaglebone pins.
### It must be run (with root permissions) *every* time the BB is rebooted. 

######################             GPIOs           ############################
# Export the General Purpose I/O pins
gpio_ls=( 
################################################################################
####                            P8 HEADER                                   ####
################################################################################
# 38  # 3  GPIO1_6      1[6]    ##  Note:     
# 39  # 4  GPIO1_7      1[7]    ##  Note:     
# 34  # 5  GPIO1_2      1[2]    ##  Note:     
# 35  # 6  GPIO1_3      1[3]    ##  Note:     
      # 7  TIMER4       2[2]    ##  Note:     
      # 8  TIMER7       2[3]    ##  Note:     
      # 9  TIMER5       2[5]    ##  Note:     
      # 10 TIMER6       2[4]    ##  Note:     
# 45  # 11 GPIO1_13     1[13]   ##  Note:    
# 44  # 12 GPIO1_12     1[12]   ##  Note:    
      # 13 EHRPWM2B     0[23]   ##  Note:    
# 26  # 14 GPIO0_26     0[26]   ##  Note:    
  47  # 15 GPIO1_15     1[15]   ##  Note:    
  46  # 16 GPIO1_14     1[14]   ##  Note:    
  27  # 17 GPIO0_27     0[27]   ##  Note:    
  65  # 18 GPIO2_1      2[1]    ##  Note:     
      # 19 EHRPWM2A     0[22]   ##  Note:    
# 63  # 20 GPIO1_31     1[31]   ##  Note:    
# 62  # 21 GPIO1_30     1[30]   ##  Note:    
# 37  # 22 GPIO1_5      1[5]    ##  Note: Pulled-up,    
# 36  # 23 GPIO1_4      1[4]    ##  Note: Pulled-up,    
# 33  # 24 GPIO1_1      1[1]    ##  Note:     
# 32  # 25 GPIO1_0      1[0]    ##  Note:     
# 61  # 26 GPIO1_29     1[29]   ##  Note: Pulled-up,   
  86  # 27 GPIO2_22     2[22]   ##  Note:    
  88  # 28 GPIO2_24     2[24]   ##  Note:    
  87  # 29 GPIO2_23     2[23]   ##  Note:    
  89  # 30 GPIO2_25     2[25]   ##  Note:    
      # 31 UART5_CTSN   0[10]   ##  Note:    
      # 32 UART5_RTSN   0[11]   ##  Note:    
      # 33 UART4_RTSN   0[9]    ##  Note:     
      # 34 UART3_RTSN   2[17]   ##  Note:    
      # 35 UART4_CTSN   0[8]    ##  Note:     
      # 36 UART3_CTSN   2[16]   ##  Note:    
      # 37 UART5_TXD    2[14]   ##  Note:    
      # 38 UART5_RXD    2[15]   ##  Note:    
      # 39 GPIO2_12     2[12]   ##  Note:    
      # 40 GPIO2_13     2[13]   ##  Note:    
      # 41 GPIO2_10     2[10]   ##  Note:    
      # 42 GPIO2_11     2[11]   ##  Note:    
      # 43 GPIO2_8      2[8]    ##  Note:     
      # 44 GPIO2_9      2[9]    ##  Note:     
      # 45 GPIO2_6      2[6]    ##  Note:     
      # 46 GPIO2_7      2[7]    ##  Note:     
        


)  
        
for pin in ${gpio_ls[@]}; do
        echo;echo "   Creating device:  gpio"$pin
        #### Creates the specified GPIO
        echo $pin > /sys/class/gpio/export
        
        #### Sets each GPIO as active high
        #echo 0 > /sys/class/gpio/gpio${pin}/active_low
        #### Sets each GPIO as "out"
        #echo "out" > /sys/class/gpio/gpio${pin}/direction
        #### Sets each GPIO as logical 0
        #echo 0 > /sys/class/gpio/gpio${pin}/value
        
        #### Shows that the gpio now exists.
        cat /sys/class/gpio/gpio${pin}
done

# ######################             LIDAR           ###########################
# # Setup the TeleTYpe device (/dev/ttyO1)
# echo BB-UART1 > $SLOTS
# stty -F /dev/ttyO1 115200  raw   time 1   min 0
#
# ######################          MICROPROCESSOR          ######################
# # Terminal to handle communication with HCS12 for the wheel encoders
# echo BB-UART2 > $SLOTS
# stty -F /dev/ttyO1 115200 raw                                                 \
# -brkint -icrnl -imaxbel -opost                                                 \
# -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke
