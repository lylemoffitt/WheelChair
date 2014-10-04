//
//  tty.h
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef __WheelChair__tty__
#define __WheelChair__tty__

#include <iostream>
#include <sstream>
#include <string>
#include <ratio>


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
    /// The full path to the device, including its name
    std::string tty_path;// == "/dev/ttyO1"
    
    /// File handle for the specific tty
	int     fd;
protected:
    /// Default constructor is unusable
    device_tty():tty_path(""){ fd=-1; }
    
public:
    /// Construct tty from device path
	device_tty(std::string _path);
    
    /// Construct tty from device path
    device_tty(const char* _path):device_tty(std::string(_path))
    {}
    
    /// Destroy the tty
    ~device_tty(){ close(fd); }
    
    
    /** Read a specific number of bytes and hold while amount received is less
     \param     num_bytes   The number of bytes to read.
     \return    uint8_t*    Pointer to a \c new array of bytes that was read.
     The reading is 'sleepy' because the function sleeps a calculated duration 
     based on \c tty_config::pkt_lag and number of bytes remaining. At most, it 
     should take \c (pkt_lag*num_bytes) to read.
     */
    uint8_t *   sleepy_read(size_t num_bytes);
    
    /** Continually read until \c num bytes have been received.
     \return    uint8_t*    An \c new array of [num] bytes as read. 
     */
    uint8_t*    rd_(size_t num);
    
    /// Read 1 byte and return it.
    uint8_t     rd_();
    
    
    /** Write to the tty.
     \param     val     The value to write; must have \c operator<<() defined.
     */
    template <class _type>
    void        wr_(_type val)
    {
        std::stringstream _ss;
        _ss << val;
        std::string _s (_ss.str());
        write(fd,_s.c_str(),_s.length());
        //cout<<"[DBG] "<< tty_path<<" << == "<<_s <<endl;
    }
    
};




#endif /* defined(__micromouse2014__tty__) */