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
device_tty      * xbee;

encoder         * enc_left,   
                * enc_right;

deque<delta>    * delta_left, 
                * delta_right;

mapper          * gps;

int main(int argc, const char * argv[])
{
    function_set    args_handler;
    
    funct_t         print_f, write_f;
    
    string          format_str;
    
    long int        iteration_limit=0;
    
    signal (SIGINT, [](int sig_num)//Register a signal handler for SIGINT
    { 
        if (enc_left!=nullptr)  {     enc_left->kill_process();     }
        if (enc_right!=nullptr) {     enc_right->kill_process();    }
        if (gps!=nullptr)       {     gps->kill_process();          }
    });

/* ************************************************************************** *\
    Begin command-line argument definitions
\* ************************************************************************** */
    args_handler.insert
    ({
        {"--new-encoder","<pin#> <pin#> <pin#> <L|R>",
            "Constructs an encoder (on the left or right) with the supplied "
            "3 pin #'s.",
            [&](param_t p)
            {  
                int pin1(0), pin2(0), pin3(0);
                p.get() >> pin1 >> pin2 >> pin3;
                string side;
                p.get() >> side;
                
                assert(pin1&&pin2&&pin3 && "Must supply valid pin numbers.");
                
                switch (side[0]) {
                    case 'L':
                        enc_left    = new encoder({pin1,pin2,pin3});
                        break;
                    case 'R':
                        enc_right   = new encoder({pin1,pin2,pin3});
                        break;
                        
                    default:
                        throw std::invalid_argument("\""+side+"\"");
                        break;
                }
            }
        },
        {"--new-xbee","<path>",
            "Construct a tty connection at the given path.",
            [&](param_t p)
            {  
                string _s;
                p.get() >> _s;
                xbee = new device_tty(_s);
            }
        },
        {"--print","<xbee|stdout>",
            "Prints to either the xbee or stdout",
            [&](param_t p)
            {  
                string _s;
                p.get() >> _s;
                if (_s=="xbee" && xbee!=nullptr) {
                    print_f = [](param_t p){
                        xbee->wr_(p.get().str());
                    };
                }else{
                    assert(_s=="stdout" && "Invalid default argument.");
                    print_f = [&](param_t p){
                        cout << p.get().str();
                    };
                }
            }
        },
        {"--start-encoder","<L|R|B>",
            "Start the encoder loop for just the Left/Right or Both (default).",
            [&](param_t p)
            {  
                string arg;
                p.get() >>  arg;
                 assert( arg=="L"||arg=="R"||arg=="B" && 
                        "Argument must be one of {L,R,B}.");
                switch (arg[0]) 
                {
                    case 'L':
                        assert(enc_left && "Encoder must be constructed first.");
                        delta_left  = enc_left->loop();     break;
                    case 'R':
                        assert(enc_right && "Encoder must be constructed first.");
                        delta_right = enc_right->loop();    break;
                        
                    case 'B':
                    default: 
                        assert(enc_left && "Encoder must be constructed first.");
                        delta_left  = enc_left->loop();
                        assert(enc_right && "Encoder must be constructed first.");
                        delta_right = enc_right->loop();
                }
            }
        },
        {"--write","<delta|position>",
            "Write formatted from either the delta or position data streams.",
            [&](param_t p)
            {  
                string _s;
                p.get() >> _s;
                
                if (_s=="delta") 
                {
                    format_str = "\tL[%+02d]\tR[%+02d]\n";
                    write_f = [&](param_t p)
                    {   decltype(iteration_limit) i=0;
                        assert((enc_left!=nullptr || enc_right!=nullptr) && 
                               "An encoder must be constructed to write");
                        assert((delta_left!=nullptr || delta_right!=nullptr) && 
                               "A delta list must exist to write from.");
                        bool //Precheck some flags 
                            _eL(enc_left!=nullptr),
                            _eR(enc_right!=nullptr),
                            _dL(delta_left!=nullptr),
                            _dR(delta_right!=nullptr);
                        
                        while ( (_eL?enc_left->is_running() :false) ||
                                (_eR?enc_right->is_running():false)  ) 
                        {   if(iteration_limit<0?0: ++i>iteration_limit){break;}
                            delta d_left(88),d_right(88);
                            if(_eL && _dL)
                                if(delta_left->begin()!=delta_left->end())
                                {
                                    d_left=delta_left->front();
                                    delta_left->pop_front();
                                }
                            if(_eR && _dR)
                               if(delta_right->begin()!=delta_right->end())
                                {
                                    d_right=delta_right->front();
                                    delta_right->pop_front();
                                }
                            p.get()<<stringf(format_str.c_str(),d_left,d_right);
                            print_f( p.get() );
                            assert( p.get().str().empty() && 
                                   "Print buffer not emptied after print.");
                        }
                    };
                }

                if (_s=="position") 
                {
                    format_str = "\tX[%+010.3lf]\tY%+010.3lf]\tA%+010.3lf]\n";
                    write_f = [&](param_t p)
                    {   decltype(iteration_limit) i=0;
                        assert(gps!=nullptr && "Mapper must be constructed.");
                        while ( enc_left->is_running()  && 
                                enc_right->is_running() &&
                                gps->is_running()        ) 
                        {   if(iteration_limit<0?0: ++i>iteration_limit){break;}
                            
                            if( gps->stream_trace_data().begin()    ==
                                gps->stream_trace_data().end()      ){  
                                continue; 
                            }
                            auto data = gps->stream_trace_data().front();
                            gps->stream_trace_data().pop_front();
                            
                            p.get() << stringf( format_str.c_str(),
                                                data.x, data.y, data.theta );
                            print_f( p.get() );
                            assert( p.get().str().empty() && 
                                   "Print buffer not emptied after print." );
                        }
                    };
                }
            }
        },
        {"--set-format"," ^string^ ",
            "Specify the carat-deliminated, 'printf' style format string to use.",
            [&](param_t p)
            {  
                format_str.clear();
                string arg;
                p.get() >>  arg; 
                assert(!arg.empty() && "Format cannot be empty");
                format_str=arg;
                return ;
                
                char _c;
                while ((_c = p.get().get())!='^');
                assert(_c=='^' && "Format string must begin with \'^\'");
                while ((_c = p.get().get())!='^'){  format_str+=_c;  }
                assert(_c=='^' && "Format string must end with \'^\'");
            }
        },
        {"--kill-after","<number>",
            "Kill after a given number of iterations.",
            [&](param_t p)
            {  
                p.get() >> iteration_limit;
            }
        },
        {"--start-mapping","",
            "Start the mapping loop.",
            [&](param_t p)
            {  
                gps = new mapper();
                assert(delta_left!=nullptr && delta_right!=nullptr &&
                       "Cannot map from null delta lists.");
                assert(enc_left!=nullptr && enc_right!=nullptr &&
                       "Cannot map without constructed encoders.");
                gps->update(*delta_left, *delta_right);
            }
        }
    });
    
/* ************************************************************************** *\
    Process command-line arguments
\* ************************************************************************** */
    
    // Read arguments from environment variable
    auto env_str = getenv("WHEEL_CHAIR_ARGS");
    if (env_str != nullptr) {
        std::stringstream _ss;
        _ss << env_str;
        args_handler.parse( _ss );
    }
    // Read arguments from main params
    args_handler.parse(argc-1, argv+1);
    
    
/* ************************************************************************** *\
    Check and assign defaults
\* ************************************************************************** */
    stringstream _ss;
    if (!print_f)               //Assign default print function
    {   _ss << " --print stdout ";
    }
    
    if (iteration_limit==0)     //Assign defualt iteration limit
    {  _ss << " --kill-after 32 ";
    }
    
    if (!write_f)               //Assign default writing function
    {  _ss << " --write delta "; 
    }
    /*
    if (format_str.empty())     //Assign default format string
    {   _ss << " --set-format ^[%lf][%lf][%lf]^ ";
    }
    */
    if (!_ss.str().empty())     //Interpret the above assignments
    {   args_handler.parse( _ss );    
    }
    
    
/* ************************************************************************** *\
    Execute
\* ************************************************************************** */
    write_f(_ss);
    
    return 0;
}










