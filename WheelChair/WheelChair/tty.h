//
//  tty.h
//  micromouse2014
//
//  Created by Lyle Moffitt on 3/23/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef __micromouse2014__tty__
#define __micromouse2014__tty__

#include <iostream>
#include <strstream>
#include <string>


#ifndef _WIN32

#include <termios.h>
#include <unistd.h> // UNIX standard function definitions
#include <fcntl.h> // File control definitions

#endif //_WIN32

namespace tty_config
{
	constexpr size_t BAUD =115200;
	constexpr size_t pkt_lag = std::micro::den / float(BAUD);
}


/********************    Teletype Style Device        **********************/
class device_tty
{
    // The full path to the device, including its name
    std::string tty_path;// == "/dev/ttyO1"
    
    // File handle for the specific tty
	int     fd;
protected:
    device_tty():tty_path(""){ fd=-1; }
    
public:
    
	device_tty(std::string _path);
    
    device_tty(const char* _path):device_tty(std::string(_path))
    {}
    ~device_tty(){ close(fd); }
    
    uint8_t *   sleepy_read(size_t num_bytes);
   
    uint8_t*    rd_(size_t num);
    
    uint8_t     rd_();
    
    template <class _type>
    void        wr_(_type val)
    {
        std::strstream _ss;
        _ss << val;
        std::string _s (_ss.str());
        write(fd,_s.c_str(),_s.length());
        //cout<<"[DBG] "<< tty_path<<" << == "<<_s <<endl;
    }
    
};




#endif /* defined(__micromouse2014__tty__) */