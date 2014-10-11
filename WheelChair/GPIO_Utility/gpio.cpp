//
//  gpio.cpp
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#include "gpio.h"

//#include "formatting.h"

#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <fstream>

 #include <cassert>
#include <sys/stat.h>
#include <errno.h>

#include <thread>
#include <functional>
using namespace std;

#if defined(TESTING) && !defined(BEAGLEBONE) 
#include <sys/stat.h>
#define BASE_PATH "/tmp/gpio"
#else
#define BASE_PATH "/sys/class/gpio/gpio"
#endif//TESTING

using sec_float=chrono::duration<double,chrono::seconds::period>;
static_assert(chrono::treat_as_floating_point<sec_float::rep>::value,
              "Custom duration not evaluated properly.");

/* ***********************          dev             ************************* */
dev::dev(std::string _attr):name(),attr(_attr)
{
}

dev::dev(std::string     _path, std::string      _name):
    name( (_path+_name) ) , attr(_name)  
{
#ifdef TESTING
    mkdir(_path.c_str(), 0777);
#endif
    assert(!name.empty() && "Device constructed with empty path string!");
#ifdef ECHO_DEBUG
    cerr<<"\tDevice ("<<name<<") \tconstructed.\n";
#endif
}

dev::dev(dev & _d):
    name(_d.name)
{
#ifdef ECHO_DEBUG
    assert(!name.empty() && "Device constructed with empty path string!");
#endif
}


void 
dev::set(std::string  _s)
{   
    lock_guard<mutex> lock(hold);

    assert( !_s.empty() && "Attempting to set with an empty string.");
    assert( !name.empty() ); 
    f_dev.open(name, fstream::out);
//    assert(f_dev.good() && "Error opening file.");
    f_dev << _s;
    f_dev.close();
    state=_s;
}

std::string 
dev::get()
{ 
    lock_guard<mutex> lock(hold);
    
    assert( !name.empty() ); 
    assert( !f_dev.is_open() );
    f_dev.open(name, fstream::in );
    assert(f_dev.good() && "Error opening file.");
    string ret(16,0);
    f_dev >> ret;
    f_dev.close();
    assert( !ret.empty() && "Nothing read from file."); 
    state = ret;
    return  ret;
}


std::string
dev::last()
{
    if(state.empty()) get();
    return state;
}


/* ***********************          gpio             ************************ */
gpio::gpio():
path(),
val("value"),
dir("direction"),
low("active_low"),
state(false)
{
    if (!mtex.try_lock()) {return;}
    mtex.unlock();
}


gpio::gpio(unsigned number):
    path( stringf("%s%u/",BASE_PATH,number) ),
    val(path , "value"),
    dir(path , "direction"),
    low(path , "active_low"),
    state(false)
{
    if (!mtex.try_lock()) {return;}
#ifdef TESTING
    dir.set("out");
    val.set("0");
    low.set("0");
#endif
#ifdef ECHO_DEBUG
    printf("GPIO(%02u) constructed.\n",number);
#endif
    mtex.unlock();
}

gpio::gpio(gpio & _g):
    path(_g.path), val(_g.val), dir(_g.dir), low(_g.low),
    state(bool(_g.state))
{
}


gpio::~gpio()
{
    if (!mtex.try_lock()) {this_thread::yield(); return;}
#ifdef ECHO_DEBUG
    if (!path.empty()) {
        cerr<<"GPIO("
        <<path.substr(path.length()-3,path.length()-1)
        <<") destroyed.\n";
    }
#endif
    mtex.unlock();
}

void    
gpio::toggle(double  delay)
{
    mtex.lock();
    val.set( (state=!state)? "1":"0" ); //Flip and print
#ifndef EMULATION_ONLY
    this_thread::sleep_for(sec_float(delay));
#endif
    val.set( (state=!state)? "1":"0" );   
    mtex.unlock();
}

bool 
gpio::set(bool _val)
{
    mtex.lock();
    state=_val;
    if (dir.last()=="in") {  dir.set("out");  }
    val.set( _val? "1":"0" );
    mtex.unlock();
    return state;
}

bool 
gpio::get()
{
    if (!mtex.try_lock()) {this_thread::yield(); return state;}
    if (dir.last()=="out") {  dir.set("in");  }
    switch( val.get()[0] )
    {
        case '1': state = true; break;
        case '0': state = false; break;
        default:
            assert( (val.last()[0]=='1' || val.last()[0]=='0') && 
                   "GPIO value read incorrectly.");
    }
    mtex.unlock();
    return state;
}

bool
gpio::poll(double interval)
{
    bool tmp(state);   
    while (true) {
        bool curr = get();
        if (curr!=tmp) {
            return curr;
        }else{
            this_thread::sleep_for(sec_float(interval));
        }
    }
}

std::future<bool> 
gpio::poll_async(double interval)
{
    packaged_task<bool()> task( bind(&gpio::poll,&(*this),interval) );
    thread t(std::move(task));  t.detach();
    return task.get_future();
}


gpio::operator bool() const
{
    return state;
}

const std::atomic_bool* 
gpio::get_state() const
{
    return &state;
}






