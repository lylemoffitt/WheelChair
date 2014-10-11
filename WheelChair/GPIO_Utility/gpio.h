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
#include <unistd.h>
#include <thread>
#include <future>
#include <cassert>

#include "formatting.h"
//#include "config.h"

//The usual system headers for read() and write()

class gpio;

/// A single file within a device
class dev
{
    /// The actual device
    std::fstream            f_dev;
    
    std::mutex              hold;
    std::string             state;
    
    friend class gpio;
    dev(std::string _attr);
    
public:
    /// The name (full path) of the device
    const std::string       name, attr;
    
    
    /// Constructor
    dev(std::string     _path, std::string      _name);
    /// Copy constructor
    dev(dev & _d);
    
    /// Write to the device
    void                    set(std::string  _s);
    /// Read from the device
    std::string             get();
    
    std::string             last();
};

/// A collection of devices used to emulate a single GPIO pin
class gpio
{
    /// The last value written to or read from the device.
    std::atomic_bool        state;
    
    std::mutex              mtex;
public:
    /// The full path to the directory that contains the devices.
    const std::string       path;
    /// The associated devices.
    dev                     val, dir, low;
    
    /// Dummy constructor
    gpio();
    /// Constructor
    gpio(unsigned number);
    /// Copy constructor
    gpio(gpio & _g);
    
    /// Destructor
    ~gpio();
    
    /** Flip pin state for specified duration.
     \param     delay       The delay in seconds (expressed as float).
     */
    void toggle(double  delay = 0);
    /// Assignment
    bool set(bool _val);
    /// Read the pin and set the state.
    bool get();
    
    /** Block until the value has changed
     \param     interval    The delay in seconds (expressed as float).
     */
    bool poll(double interval = 0);
    
    /** Returns future immediately and runs asynchronously
     \param     interval    The delay in seconds (expressed as float).
     */
    std::future<bool> poll_async(double interval = 0);
    
    //Return state via cast to bool
    inline explicit operator bool() const;
    
    inline const std::atomic_bool* get_state() const;
    
};


template< size_t B_WID >
class gpio_bus
{
    std::array<gpio*,B_WID>     bus;
    std::bitset<B_WID>          val;
    
public:
	gpio_bus(init_ls<int> pins )
    {
        assert(pins.size()==bus.size() && "Initializer mismatch!");
        for(int i=0; i < B_WID; ++i){
            bus[i] = new gpio(pins[i]);
            val[i] = bus[i]->get();
        }
    }
    
    gpio_bus(gpio_bus & g): bus(g.bus)
    {
    }
    
    
    std::bitset<B_WID>          get()
    {
        for( int i=0; i < B_WID; ++i){  val[i] = bus[i]->get();  }
        return val;
    }

    std::bitset<B_WID>          get_mt()
    {
        std::array<std::thread, B_WID> t_bus;
        for( int i=0; i < B_WID; ++i){  
            t_bus[i] = std::thread( &gpio::get, &(*(bus[i])) );
        }
        for( int i=0; i < B_WID; ++i){  
            t_bus[i].join();  val[i] = (bool)bus[i];
        }
        return val;
    }
    
	void                        set(std::bitset<B_WID> value)
    {
        for( int i=0; i < B_WID; ++i){  val[i] = bus[i]->set( value[i] );  }
    }
    
};



#endif /* defined(__WheelChair__GPIO__) */
