//
//  config.h
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef WheelChair_config_h
#define WheelChair_config_h

/* ************************************************************************** */
//Settings for configuring the encoder
/* ************************************************************************** */

/// The amount of change between polls of the encoder
typedef short delta;

/// The pins for the left wheel's encoder
const int left_pins[]   = {69, 74, 73, 72};
/// The pins for the right wheel's encoder
const int right_pins[]  = {66, 75, 71, 70};

#define ENC_WIDTH 4

#define ENCODER_SEQUENCE         \
{                                \
    "0000",/*01*/                \
    "0001",/*02*/                \
    "0011",/*03*/                \
    "0010",/*04*/                \
    "1010",/*05*/                \
    "1011",/*06*/                \
    "1111",/*07*/                \
    "1110",/*08*/                \
    "0110",/*09*/                \
    "0111",/*10*/                \
    "0101",/*11*/                \
    "0100",/*12*/                \
    "1100",/*13*/                \
    "1101",/*14*/                \
    "1001",/*15*/                \
    "1000",/*16*/                \
}                                \

#define ENCODER_POSITIONS 16

#define POLLING_PERIOD 0

#define ENCODER_TURN_RATIO 53.0




/* ************************************************************************** */
//Settings for configuring the XBee TTY connection
/* ************************************************************************** */
#define TTY_PATH "/dev/tty01"




/* ************************************************************************** */
//Settings for configuring the XBee TTY connection
/* ************************************************************************** */
#define WHEEL_DISTANCE ( 20.0+(5.0/8.0) )

#define WHEEL_DIAMETER 24.0

#define PI 3.14159265359





#endif
