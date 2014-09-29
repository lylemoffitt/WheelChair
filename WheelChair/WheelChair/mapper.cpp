//
//  mapper.cpp
//  WheelChair
//
//  Created by Lyle Moffitt on 9/26/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#include "mapper.h"
#include <cmath>
#include <cassert>
#include "config.h"


mapper::mapper():
trace_data(new std::deque<coordinate>()),
kill_thread(false)
{
    
}

mapper::~mapper()
{
    kill_thread = true;
    if (updater.joinable()) {   updater.join();     }   
    delete  trace_data;
}

mapper::mapper(mapper && _m)
{
    _m.mtx_pos.lock();
    kill_thread.store(_m.kill_thread);
    position    = std::move(_m.position);
    updater     = std::move(_m.updater);
    trace_data  = std::move(_m.trace_data);
    delete &_m;
}

void
mapper::reset_position()
{
    coordinate c = { .x=0, .y=0, .theta=0, };
    mtx_pos.lock();
    position = c;
    mtx_pos.unlock();
}

coordinate 
mapper::get_position()
{
    mtx_pos.lock();
    coordinate ret (position); //Save for return
    mtx_pos.unlock();
    return ret;
}


std::deque<coordinate> &
mapper::stream_trace_data()
{
    return * trace_data;
}



coordinate 
mapper::update( const short l_delta , const short r_delta )
{
    const long double //Construct all function variables
    T_r (ENCODER_TURN_RATIO*ENCODER_POSITIONS), //Encoder turn ratio
    T_1 (l_delta),                              //# of steps take by encoder_1
    T_2 (r_delta),                              //# of steps take by encoder_2
    R_w (WHEEL_DIAMETER/2.0),                   //Radius of wheel
    D_  (WHEEL_DISTANCE);                       //Distance between wheels
    
    mtx_pos.lock(); //Lock mutex to read and update 
    
    position.x      += ( R_w * cosl(position.theta) * (T_1+T_2) * PI/T_r );
    position.y      += ( R_w * sinl(position.theta) * (T_1+T_2) * PI/T_r );
    position.theta  += ( 2.0 * PI * (R_w/D_) * ((T_1-T_2)/T_r) );
    
    coordinate ret (position); //Save for return
    mtx_pos.unlock();
    
    return ret;
}

void 
mapper::update( std::deque<short> & l_deltas , std::deque<short> & r_deltas )
{
    if (&l_deltas==&r_deltas) {
        throw std::invalid_argument("Delta streams cannot be identical.");
    }
    if (updater.joinable()) {
        kill_thread = true;
        updater.join();
        trace_data->clear();
        return;
    }
    kill_thread = false;
    // Construct function for looping the update() function
    auto loop_update = [&l_deltas,&r_deltas,this]()
    {
        while (!kill_thread) 
        {
            //If source threads are empty, yield
            if (l_deltas.begin()==l_deltas.end() || 
                r_deltas.begin()==r_deltas.end()) {
                std::this_thread::yield();
            }
            //Update and publish to trace stream
            trace_data->push_back( update(l_deltas.front(),r_deltas.front()) );
            l_deltas.pop_front(); r_deltas.pop_front();
        }
    };
    
    //Start managing thread
    updater = std::thread(loop_update);
    
}

void
mapper::kill_process()
{
    kill_thread = true;
    if (updater.joinable()) {   updater.join();     }   
}

const bool
mapper::is_running() const
{   
    assert( (!kill_thread)==updater.joinable() &&
           "Kill state of thread must match running state." );
    return ( !kill_thread && updater.joinable() );
}


