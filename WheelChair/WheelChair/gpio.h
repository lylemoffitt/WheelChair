//
//  GPIO.h
//  CNC
//
//  Created by Lyle Moffitt on 6/1/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef __CNC__GPIO__
#define __CNC__GPIO__

#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <array>

#include <mutex>
#include <atomic>
#include "formatting.h"

//The usual system headers for read() and write()

#if defined(TESTING) && !defined(BEAGLEBONE) 
#define BASE_PATH "test_gpio/gpio"
#else
#define BASE_PATH "/sys/class/gpio/gpio"
#endif//TESTING
#define BUFF_SIZE 8

/// A single file within a device
class dev
{
    /// The name (full path) of the device
    const std::string       name;
    /// The actual device
    std::fstream            f_dev;
    
    std::mutex              hold;
    
public:
    /// Constructor
    dev(std::string     _path, std::string      _name);
    /// Copy constructor
    dev(dev & _d);
    
    /// Write to the device
    void                    set(std::string  _s);
    /// Read from the device
    std::string             get();
    
    
    
};

/// A collection of devices used to emulate a single GPIO pin
class gpio
{
    /// The full path to the directory that contains the devices.
    const std::string       path;
    /// The associated devices.
    dev                     val, dir, low;
    /// The last value written to or read from the device.
    std::atomic_bool        state;
    
    std::mutex              mtex;
    
public:
    /// Constructor
    gpio(u_int _number);
    /// Copy constructor
    gpio(gpio & _g);
    
    /// Destructor
    ~gpio();
    
    /** Flip pin state for specified duration.
     * @param u_sec The duration of the toggle in microseconds.
     */
    void toggle(useconds_t   u_sec);
    /// Assignment
    bool set(bool _val);
    /// Read the pin and set the state.
    bool get();
    
};


#define B_WID 8
class gpio_bus : std::bitset<B_WID>    
{
    std::array<gpio*,B_WID>     bus;
    
public:
	gpio_bus(init_ls<int> pins );
    
    gpio_bus(gpio_bus & g);
    
    
    std::bitset<B_WID>          get();
    
	void                        set(std::bitset<B_WID> value);
    
};



#endif /* defined(__CNC__GPIO__) */
