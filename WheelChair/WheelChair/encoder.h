//
//  encoder.h
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef __WheelChair__encoder__
#define __WheelChair__encoder__

#include <deque>
#include <thread>
#include <atomic>


#include "gpio.h"
#include "config.h"



class encoder 
{
    /** 4-bit grey code sequence produced by the encoder
     * http://www.ctscorp.com/components/datasheets/288.pdf
     */
    std::array< std::bitset<ENC_WIDTH>,(2<<ENC_WIDTH) >     grey_ls;
    
    /** Keeps track the current encoder position
     * \note Must be assigned with \c (...)%(2<<ENC_WIDTH)
     */
    std::atomic_uint_fast8_t pos;
    
    /** List of changes between each poll of the encoder
     * \note Equals \c nullptr if the thread is not running. 
     */
    std::deque<delta>   *   delta_ls;
    
    /// The thread handling the continuous read-update loop
    std::thread             runner;
    std::atomic_bool        kill_runner;
    
    /// The pins to which the encoder is connected
    gpio_bus                pins;
    
    /// The number of times to read from the bus before we accept the new number
    size_t                  polling_period;
public:
    /// Constructor -- Requires that you supply the pins of the encoder
    encoder(init_ls<int> pin_ls);
    
    /// Copy Constructor -- Moves \c runner if thread is running. 
    encoder(encoder & _enc);
    
    /// Destructor
    ~encoder();
    
    /** Check if the encoder positon has changed
     * \param   number  
        The number of times to check again if the encoder hasn't moved yet.
     */
    const short   
    poll(const size_t number = POLLING_PERIOD);
    
    
    /// Sets what action to take when re-entering \c loop()
    enum loop_opt
    {   clear,  ///< Clear the delta list [default]
        wait    ///< Wait (yield) until the delta list is empty
    };
    /** Start/Stop the managing thread to read the encoder
     * \return      nullptr     The thread has been killed
     */
    std::deque<delta> * 
    loop(const loop_opt option = clear);
    
    /// Check if the encoder thread is currently running
    const bool    
    is_running() const;
    
    /// Fetches the encoder's delta list
    inline std::deque<delta> * 
    get_list() const;
    
    /// Kill and join the running thread
    void
    kill_process();
    
    
    
};

#endif /* defined(__WheelChair__encoder__) */
