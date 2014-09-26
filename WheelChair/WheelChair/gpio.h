//
//  gpio.h
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef __WheelChair__GPIO__
#define __WheelChair__GPIO__

#include <string>
#include <fstream>
#include <bitset>
#include <array>

#include <mutex>
#include <atomic>


#include "formatting.h"
#include "config.h"

//The usual system headers for read() and write()



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
    
    
    inline explicit operator bool() const;
    
};

#define B_WID ENC_WIDTH

class gpio_bus
{
    std::array<gpio*,B_WID>     bus;
    std::bitset<B_WID>          val;
    
public:
	gpio_bus(init_ls<int> pins );
    
    gpio_bus(gpio_bus & g);
    
    
    std::bitset<B_WID>          get();
    std::bitset<B_WID>          get_mt();
    
	void                        set(std::bitset<B_WID> value);
    
};



#endif /* defined(__WheelChair__GPIO__) */
