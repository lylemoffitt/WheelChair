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

#include "function_set.h"
#include "tty.h"
#include "encoder.h"
#include "mapper.h"



using namespace std;

int main(int argc, const char * argv[])
{
    device_tty  * xbee;
    
    encoder     * enc_left,   * enc_right;
    deque<delta> *delta_left, *delta_right;
    
    mapper      * gps;
    
    function_set args_handler;
    
    args_handler.insert
    ({
        {"--enc","<pin#> <pin#> <pin#> <L|R>",
            "Constructs an encoder (on the left or right) with the supplied 3"
            " pin #'s.",
            [&](param_t p)->ret_t
            {  
                int p1,p2,p3;
                stringstream _ss (p);
                _ss >> p1 >> p2 >> p3;
                string side;
                _ss >> side;
                
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
                
                return 0;
            }
        },
        {"--xbee","<path>",
            "Construct a tty connection at the given path.",
            [&](param_t p)->ret_t
            {  
                xbee = new device_tty(p);
                return 0;
            }
        },
        {"--pipe","<xbee|stdout>",
            "Pipes stdin to either the xbee or stdout",
            [&](param_t p)->ret_t
            {  
                do{
                    xbee->wr_(cin.get());
                }while(cin.good() && !cin.eof());
                return 0;
            }
        },
        {"--read","<interval> <times> <L|R>",
            "Pipes stdin to either the xbee or stdout",
            [&](param_t p)->ret_t
            {  
                int interval, times;
                string side;
                stringstream _ss(p);
                _ss >> interval >> times >> side;
                switch (side[0]) {
                    case 'L':
                        delta_left  = enc_left->loop();
                        break;
                    case 'R':
                        delta_right = enc_right->loop();
                        break;
                        
                    default:
                        throw std::invalid_argument("\""+side+"\"");
                        break;
                }
                for (int i=0; times<0 || i<times; ++i) {
                    bitset<ENC_WIDTH> _b (en)
                    
                    printf("{ %s , %s }")
                }
                
            }
        }
    });
    
    // Read arguments from environment variable
    auto env_str = getenv("WHEEL_CHAIR_ARGS");
    if (env_str != nullptr) {
        args_handler.parse( std::string(env_str) );
    }
    
    // Read arguments from arg params
    args_handler.parse(argc-1, argv+1);
    
    
    
    return 0;
}

