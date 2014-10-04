//
//  mapper.h
//  WheelChair
//
//  Created by Lyle Moffitt on 9/26/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef __WheelChair__mapper__
#define __WheelChair__mapper__

#include <iostream>
#include <deque>
#include <map>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <sstream>

//#include "formatting.h"

struct coordinate { long double x,y, theta; };


class mapper
{
    /// Mutex to protect the position data if being edited/read by mutliple threads
    std::mutex                          mtx_pos;
    /// Data holding the current kinematic state
    coordinate                          position;
    
    /// Stream containing raw data as it changes
    std::deque<coordinate>            * trace_data;
    
    /// Thread that handles the alteration and publishing of data
    std::thread                         updater;
    std::atomic_bool                    kill_thread;
    
    
protected:
    
    /// Special method for resetting the position to zero (initial state)
    void 
    reset_position();
    
    
public:
    /// Default constructor
    mapper();
    
    /// Move constructor
    mapper(mapper && _m);
    
    /// Default destructor
    ~mapper();
    
    /// Fetch the most current position
    coordinate 
    get_position();
    
    /// Fetch the raw data stream
    std::deque<coordinate> &
    stream_trace_data();
    
    
    /** Updates the mapper's position
     * \sa http://en.wikipedia.org/wiki/Dead_reckoning
     * \return position
     */
    coordinate 
    update( const short l_delta , const short r_delta );
    
    /** Macro function for handling the input stream
     * \sa encoder::loop()
     * Call once to enable; call again to kill.
     * \param   l_deltas    Stream of position changes for LEFT encoder.
     * \param   r_deltas    Stream of position changes for RIGHT encoder.
     */
    void 
    update( std::deque<short> & l_deltas , std::deque<short> & r_deltas );
    
    
    void
    kill_process();
    
    /// Check if the mapping thread is currently running
    const bool    
    is_running() const;
    
    
    
    
    
    
};

#endif /* defined(__WheelChair__mapper__) */
