//
//  tty.cpp
//  micromouse2014
//
//  Created by Lyle Moffitt on 3/23/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#include "tty.h"

using namespace std;

/********************    Teletype Style Device        **********************/

uint8_t * 
device_tty::sleepy_read(size_t num_bytes)
{
    uint8_t * tmp = new uint8_t[num_bytes+1];
    size_t n_rcvd,tmp_index=0;
    while(num_bytes > 0)
    {
        n_rcvd = read( fd, &(tmp[tmp_index]), num_bytes);
        if (n_rcvd == 0){   
            usleep(useconds_t(tty_config::pkt_lag * num_bytes)); 
        }else{  
            num_bytes -= n_rcvd;
            tmp_index += n_rcvd;
        }
    }
    return tmp;
}

device_tty::device_tty(string _path):
tty_path (_path)
{
    fd = open( tty_path.c_str(), O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK );
    if(fd == -1)
    {   cerr << "Invalid: "<<_path<<"\n";  
        fd=2; 
        tty_path = "/dev/fd/2";
    }
}

uint8_t*
device_tty::rd_(size_t num)
{
    uint8_t* _ret = new uint8_t[num+1];
    for(size_t len =0; num > len;len += read( fd, &(_ret[len]), (num-len)) ); 
    
    //cout<<"[DBG] "<< tty_path<<" == >> "<< _ret <<endl;
    return _ret;
}

uint8_t
device_tty::rd_(){ return rd_(1)[0];    }



