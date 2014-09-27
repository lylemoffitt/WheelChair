//
//  main.cpp
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <signal.h>
#include <cassert>

#include "function_set.h"
#include "tty.h"
#include "encoder.h"
#include "mapper.h"



using namespace std;
// Global values
device_tty  * xbee;

encoder     * enc_left,   * enc_right;
deque<delta> *delta_left, *delta_right;

mapper      * gps;

int main(int argc, const char * argv[])
{

    
    function_set args_handler;
    
    funct_t     print_f;
    
    signal //Register a signal handler for SIGINT
    (SIGINT, [](int sig_num)
    { 
        delete enc_left;
        delete enc_right;
        
    }
     );
    
    args_handler.insert
    ({
        {"--enc","<pin#> <pin#> <pin#> <L|R>",
            "Constructs an encoder (on the left or right) with the supplied "
            "3 pin #'s.",
            [&](param_t p)
            {  
                int p1,p2,p3;
                p.get() >> p1 >> p2 >> p3;
                string side;
                p.get() >> side;
                
                switch (side[0]) {
                    case 'L':
                        enc_left    = new encoder({p1,p2,p3});
                        break;
                    case 'R':
                        enc_right   = new encoder({p1,p2,p3});
                        break;
                        
                    default:
                        throw std::invalid_argument("\""+side+"\"");
                        break;
                }
            }
        },
        {"--xbee","<path>",
            "Construct a tty connection at the given path.",
            [&](param_t p)
            {  
                string _s;
                p.get() >> _s;
                xbee = new device_tty(_s);
            }
        },
        {"--pipe","<xbee|stdout>",
            "Pipes to either the xbee or stdout",
            [&](param_t p)
            {  
                string _s;
                p.get() >> _s;
                if (_s=="xbee" && xbee!=nullptr) {
                    print_f = [](param_t p){
                        xbee->wr_(p.get().str());
                    };
                }else{
                    print_f = [&](param_t p){
                        cout << p.get().str();
                    };
                }
            }
        },
        {"--start","<L|R|B>",
            "Start the encoder loop for Both (default) or just the Left/Right.",
            [&](param_t p)
            {  
                string side;
                p.get() >>  side;
                switch (side[0]) 
                {
                    case 'L':
                        delta_left  = enc_left->loop();     break;
                    case 'R':
                        delta_right = enc_right->loop();    break;
                        
                    default /* B */:
                        if (enc_left!=nullptr) {
                            delta_left  = enc_left->loop();
                        }
                        if (enc_right) {
                            delta_right = enc_right->loop();
                        }
                }
            }
        }
    });
    
    // Read arguments from environment variable
    auto env_str = getenv("WHEEL_CHAIR_ARGS");
    if (env_str != nullptr) {
        std::stringstream _ss;
        _ss << env_str;
        args_handler.parse( _ss );
    }
    
    // Read arguments from main params
    args_handler.parse(argc-1, argv+1);
    
    // Make sure there is a defualt print function
    if (!print_f) 
    {   print_f = [&](param_t p)
        {
            cout << p.get().str();
        };
    }
    stringstream _ss;
    while (delta_left!=nullptr || delta_right!=nullptr) {
        if (delta_left!=nullptr) { 
            if(delta_left->begin()==delta_left->end()){break;}
            _ss<< stringf("\tL[%+02d]", delta_left->front()); 
            delta_left->pop_front();
        }
        if (delta_right!=nullptr) { 
            if(delta_right->begin()==delta_right->end()){break;}
            _ss<< stringf("\tR[%+02d]", delta_right->front()); 
            delta_right->pop_front();
        }
        _ss << "\n";
        print_f(_ss);
        assert(_ss.str().empty() && "Print buffer not emptied after print.");
    }
    
    return 0;
}

