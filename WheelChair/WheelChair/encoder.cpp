//
//  encoder.cpp
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#include "encoder.h"
#include <cstdlib>
#include <algorithm>
#include <cassert>

#define MULTITHREAD_GPIO //Use to enable multi-threaded reading from the GPIOs

#ifdef MULTITHREAD_GPIO
    #define GET( _gpio_ ) ( _gpio_.get_mt() ) 
#else
    #define GET( _gpio_ ) ( _gpio_.get() ) 
#endif

encoder::encoder(init_ls<int> pin_ls):pins(pin_ls)
{
    assert(pin_ls.size() == ENC_WIDTH && "Pin # mismatch!");
    char  _pins[(2<<ENC_WIDTH)][ENC_WIDTH+1] = ENCODER_SEQUENCE;
    for (uint8_t i=0; i<(2<<ENC_WIDTH); ++i) {
        grey_ls[i] = std::bitset<ENC_WIDTH>(_pins[i]);
    }
    auto curr = pins.get();
    for (uint8_t i=0; i<grey_ls.size(); ++i) {
        if (curr == grey_ls[i]){  pos=i; break; }
    }
}

encoder::encoder( encoder &_enc):
    runner(std::move(_enc.runner)),
    pins(_enc.pins),
    delta_ls(_enc.delta_ls),
    grey_ls(_enc.grey_ls)
{
    pos.store(_enc.pos);
}

encoder::~encoder()
{
    delta_ls->clear();
    delete delta_ls;
    if( runner.joinable() ){  runner.join();  }
}

const short 
encoder::poll(const size_t number)
{
    std::bitset<B_WID>	curr = GET(pins);
    for (size_t i=0; i<number; ++i) {
        auto tmp = GET(pins);
        if (tmp != curr) {  curr=tmp; break; }
    }
    
    for (uint i=0; i<=(1<<ENC_WIDTH); ++i) {
        if (curr == grey_ls[(pos+i)%(2<<ENC_WIDTH)]) {  
            pos = (pos+i)%(2<<ENC_WIDTH);  return 0+i;  
        }
        if (curr == grey_ls[(pos-i)%(2<<ENC_WIDTH)]) {  
            pos = (pos-i)%(2<<ENC_WIDTH);  return 0-i;
        }
    }
    return pos;
}

std::deque<short>  * 
encoder::loop(const encoder::loop_opt option)
{   //Kill it if it's currently running.
    if (delta_ls != nullptr && runner.joinable()) {
        switch (option) {
            case loop_opt::clear    :
                delta_ls->clear();  break;
            case loop_opt::wait     :
                while (delta_ls->begin()!=delta_ls->end()) {
                    std::this_thread::yield();
                }   break;
        }
        delete delta_ls;
        runner.join(); 
        return nullptr;
    }
    delta_ls = new std::deque<short>();
    
    auto loop_f = [this]//lambda function to run in thread
    {
        std::bitset<B_WID>	_bits;
        while (delta_ls!=nullptr) 
        {   
            delta_ls->push_back( poll( POLLING_PERIOD ) );
        }
        return;
    };
    
    runner = std::thread( loop_f );
    return delta_ls;
}

const bool
encoder::is_running() const
{
    return ( delta_ls!=nullptr && runner.joinable() );
}


std::deque<delta>  * 
encoder::get_list() const
{
    return delta_ls;
}




