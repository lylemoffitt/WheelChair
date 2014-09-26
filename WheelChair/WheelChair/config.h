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

#define POLLING_PERIOD 0

/* ************************************************************************** */
//Settings for configuring the XBee TTY connection
/* ************************************************************************** */
#define TTY_PATH "/dev/tty01"

#endif
