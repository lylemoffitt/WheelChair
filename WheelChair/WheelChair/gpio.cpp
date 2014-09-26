//
//  gpio.cpp
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#include "GPIO.h"

#include "formatting.h"

#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <fstream>

 #include <cassert>
#include <sys/stat.h>
#include <errno.h>

#include <thread>

using namespace std;

#if defined(TESTING) && !defined(BEAGLEBONE) 
#define BASE_PATH "test_gpio/gpio"
#else
#define BASE_PATH "/sys/class/gpio/gpio"
#endif//TESTING

/* ***********************          dev             ************************* */

dev::dev(std::string     _path, std::string      _name):
 name( (_path+_name) )   
{
    assert(!name.empty() && "Device constructed with empty path string!");
    cerr<<"\tDevice ("<<name<<") constructed.\n";
}

dev::dev(dev & _d):
    name(_d.name)
{
    assert(!name.empty() && "Device constructed with empty path string!");
}


void 
dev::set(std::string  _s)
{   
    lock_guard<mutex> lock(hold);
    cerr<<"\nSET : ";
    cerr<<"\t"<<_s<<" =>"<<name<<endl;
    assert( !_s.empty() && "Attempting to set with an empty string.");
    assert( !name.empty() ); 
    f_dev.open(name, fstream::out);
//    assert(f_dev.good() && "Error opening file.");
    f_dev << _s;
    f_dev.close();
}

std::string 
dev::get()
{ 
    lock_guard<mutex> lock(hold);
    cerr<<"\nGET : ";
    ECHO(name);
    assert( !name.empty() ); 
    assert( !f_dev.is_open() );
    f_dev.open(name, fstream::in );
    assert(f_dev.good() && "Error opening file.");
    string ret(16,0);
    f_dev >> ret;
    f_dev.close();
    assert( !ret.empty() && "Nothing read from file."); 
    return  ret;
}



/* ***********************          gpio             ************************ */

gpio::gpio(u_int _number):
    path( stringf("%s%u/",BASE_PATH,_number) ),
    val(path , "value"),
    dir(path , "direction"),
    low(path , "active_low"),
    state(false)
{
    if (!mtex.try_lock()) {return;}
    dir.set( "out"  ); //Set direction outwards
    val.set( "0"    ); //Set output value to logical zero
    low.set( "1"    ); //Set gpio to active-HIGH
    cerr<<"GPIO("<<to_string(_number)<<") constructed.\n";
    mtex.unlock();
}

gpio::gpio(gpio & _g):
    path(_g.path), val(_g.val), dir(_g.dir), low(_g.low)
{
}


gpio::~gpio()
{
    if (!mtex.try_lock()) {this_thread::yield(); return;}
    dir.set( "out" );
    val.set( "0"   );
    low.set( "0"   );
    mtex.unlock();
}

void    
gpio::toggle(useconds_t u_sec)
{
    mtex.lock();
    val.set( (state=!state)? "1":"0" ); //Flip and print
#ifndef EMULATION_ONLY
    usleep( u_sec );
#endif
    val.set( (state=!state)? "1":"0" );   
    mtex.unlock();
}

bool 
gpio::set(bool _val)
{
    mtex.lock();
    state=_val;
    val.set( _val? "1":"0" );
    mtex.unlock();
    return state;
}

bool 
gpio::get()
{
    if (!mtex.try_lock()) {this_thread::yield(); return state;}
    switch( val.get()[0] )
    {
        case '1': state = true; break;
        case '0': state = false; break;
        default:
            assert( (val.get()[0]=='1' || val.get()[0]=='0') && 
                   "GPIO value read incorrectly.");
    }
    mtex.unlock();
    return state;
}


gpio::operator bool() const
{
    return state;
}


/* ***********************        gpio_bus             ************************ */



gpio_bus::gpio_bus(init_ls<int> pins)
{
    assert(pins.size()==bus.size() && "Initializer mismatch!");
    for(int i=0; i < B_WID; ++i){
        bus[i] = new gpio(pins[i]);
        val[i] = bus[i]->get();
    }
}

gpio_bus::gpio_bus(gpio_bus & g): bus(g.bus)
{
}

std::bitset<B_WID>	
gpio_bus::get()
{
    for( int i=0; i < B_WID; ++i){  val[i] = bus[i]->get();  }
    return val;
}

std::bitset<B_WID>	
gpio_bus::get_mt()
{
    std::array<thread, B_WID> t_bus;
    for( int i=0; i < B_WID; ++i){  
        t_bus[i] = thread( &gpio::get, &(*(bus[i])) );
    }
    for( int i=0; i < B_WID; ++i){  
        t_bus[i].join();  val[i] = (bool)bus[i];
    }
    return val;
}

void	
gpio_bus::set(std::bitset<B_WID> value)
{
    for( int i=0; i < B_WID; ++i){  val[i] = bus[i]->set( value[i] );  }
}







