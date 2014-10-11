//
//  main.cpp
//
//  Created by Lyle Moffitt on 7/8/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//


#include <string>
#include <iostream>
#include <fstream>
 #include <cassert>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <atomic>
#include <thread>
#include <map>
#include <deque>
#include <unistd.h>
#include <regex>

#include "function_set.h"
#include "gpio.h"

using namespace std;

deque<unsigned>         get_variadic_pins(param_t p);

void                    print(gpio * _g);

/* ************************************************************************** *\
\* ************************************************************************** */

#define PATH "/sys/class/gpio/"
#define ENV_VAR_NAME "ACTIVE_GPIO_PINS"


int main(int argc, const char * argv[])
{    
    //Collect pin numbers from environment variable
    stringstream            env_ss;
    map<unsigned,gpio*>     gpio_map;
    
    char * env_str = getenv(ENV_VAR_NAME);
    if (env_str != nullptr) {
        env_ss << env_str;
    }
    int num;
    while ( [&]{num=0; env_ss >> num; return (num>0);}() ) {
        gpio_map[num] =  new gpio(num) ;
    }
    for(auto i=gpio_map.begin(); i!=gpio_map.end(); i++){
        env_ss << " " << i->first;
    }
    setenv(ENV_VAR_NAME, env_ss.str().c_str(), 1);
    
    //Apply principal of least privilege
    const gid_t             rel_id (getuid()), eff_id (geteuid());
    seteuid(rel_id);
    
    
    //Define command line args
    function_set args_handler;
    args_handler.insert
    ({
        {"--read","<pin_list>",
            "Reads the value(s) of the GPIO(s).\n"
            "\t <pin_list>  A list of space-separated pins numbers.",        
            [&](param_t p)  
            {
                deque<unsigned> pins = get_variadic_pins(p);
                print(nullptr);
                assert(!gpio_map.empty() && "GPIOS must be first exported.\n");
                if (pins.empty()) {
                    for(auto i=gpio_map.begin(); i!=gpio_map.end(); i++){
                        print(i->second);
                    }
                }else{
                    for(auto i=pins.begin(); i!=pins.end(); i++){
                        auto _g = gpio_map.find(*i);
                        if (_g==gpio_map.end()) {
                            cout<<"\tGPIO "<<*i<<" is uninitialized.\n";
                        }else{
                            print(_g->second);
                        }
                    }
                }
                
            } 
        },
        {"--set","<attribute> <value> <pin_list>",
            "Writes the value to the attribute of the specified GPIO(s).\n"
            "\t <attribute> The feature to be set, must be one of: \n"
            "\t             \'value\'       \"1\"(Vdd) or \"0\"(Gnd)  \n"
            "\t             \'direction\'   \"in\"(Read) or \"out\"(Write)   \n"
            "\t             \'active_low\'  \"1\"(Low) or \"0\"(High)  \n"
            "\t <value>     The value to write to the attribute to (see above)\n"
            "\t <pin_list>  A list of space-separated pins numbers.",        
            [&](param_t p)   
            {
                string  attr, value;
                p.get() >>  attr >> value;
                deque<unsigned> pins = get_variadic_pins(p);
                assert(!gpio_map.empty() && "GPIOS must be first exported.\n");
                if (pins.empty()) {
                    cerr << "ERROR: No gpio pin numbers lised\n";
                }
                print(nullptr);
                for(auto i=pins.begin(); i!=pins.end(); i++){
                    auto _g = gpio_map.find(*i);
                    if (_g==gpio_map.end()) {
                        cout<<"\tGPIO "<<*i<<" is uninitialized.\n"; 
                        continue;
                    }
                    if (attr=="value") {
                        _g->second->val.set(value);
                    }else if (attr=="direction") {
                        _g->second->dir.set(value);
                    }else if (attr=="active_low"){
                        _g->second->low.set(value);
                    }else{
                        cerr<<"ERROR: Invalid argument \""<<attr<<"\". "
                        "<attribute> must be one of: \'value\', \'direction\', "
                        "or \'active_low\'.\n";
                    }
                    print(_g->second);
                }

            } 
        },
        {"--export","<pin_list>",
            "Exports and creates GPIO(s). \n"
            "\t <pin_list>  A list of space-separated pins numbers.",        
            [&](param_t p)   
            {
                deque<unsigned> pins = get_variadic_pins(p);
                seteuid(eff_id);
                fstream _export(PATH "export");
                for(auto i=pins.begin(); i!=pins.end(); i++){
                    _export << *i;
                    env_ss << " " << *i;
                    gpio_map[*i] =  new gpio( *i ) ;
                }
                setenv(ENV_VAR_NAME, env_ss.str().c_str(), 1);
                seteuid(rel_id);
            } 
        },
        {"--unexport","<pin list>",
            "Unexports and removes GPIO(s). \n"
            "\t <pin_list>  A list of space-separated pins numbers.",        
            [&](param_t p)   
            {
                deque<unsigned> pins = get_variadic_pins(p);
                assert(!gpio_map.empty() && "GPIOS must be first exported.\n");
                
                seteuid(eff_id);
                fstream _export(PATH "unexport");
                for(auto i=pins.begin(); i!=pins.end(); i++){
                    _export << *i;
                    delete gpio_map[*i];
                    gpio_map.erase(*i);
                }
                env_ss.clear();
                for(auto i=gpio_map.begin(); i!=gpio_map.end(); i++){
                    env_ss << " " << i->first;
                }
                setenv(ENV_VAR_NAME, env_ss.str().c_str(), 1);
                seteuid(rel_id);
            } 
        },
        {"--find","",
            "Find existing GPIOs and load them into " ENV_VAR_NAME " .",        
            [&](param_t p)   
            {
                char * f_tmp , *form=new char[16];
                sprintf(form, "/tmp/tmp_XXXXXX");
                f_tmp = mktemp(form);
                system(stringf("/bin/ls " PATH " > %s", f_tmp).c_str());
                fstream _fs(f_tmp);
                regex _regex("gpio([0-9]{2})",regex::ECMAScript);
                while(1){
                    string _s;
                    _fs >> _s;
                    if( _s.empty() ){   break;  }
                    std::smatch _match;
                    std::regex_search(_s, _match, _regex);
                    if( _match.length()<=0 ){  continue;  }
                    env_ss << " " << _match.str(1);
                    int index = atoi( _match.str(1).c_str() );
                    gpio_map[index] =  new gpio( index ) ;
                }
                _fs.close();
                remove(f_tmp);
                delete form;
                setenv(ENV_VAR_NAME, env_ss.str().c_str(), 1);
            }
        },
        {"--poll","<interval> <display>",
            "Periodically poll GPIOs for changes.\n"
            "\t <interval>  The delay between checks (as float) in seconds. \n"
            "\t <display>   How the results should be printed; one of:\n"
            "\t             \'clear\'       Clear screen and re-print all.  \n"
            "\t             \'stream\'      Print only those that change.   \n",
            [&](param_t p)   
            {
                using sec_fl=chrono::duration<double,chrono::seconds::period>;
                static_assert(chrono::treat_as_floating_point<sec_fl::rep>::value,
                              "Custom duration not evaluated properly.");
                double intrvl; string disp;
                p.get()>>intrvl >> disp;
                map<int, future<bool>> bit_map;
                assert(intrvl!=0 && 
                       "Invalid argument: malformed <interval> param.");
                assert(disp=="clear"||disp=="stream" && 
                       "Invalid argument: malformed <display> param.");
                system("clear");  print(nullptr);
                for( auto & i : gpio_map ){ 
                    bit_map[i.first]=i.second->poll_async(intrvl);
                    print(i.second);  
                }
                while(1)
                {
                    for (atomic_bool change(false); !change;) {
                        for( auto & i : gpio_map ){ 
                            if ( bit_map[i.first].wait_for(sec_fl(intrvl/10.0))
                                 == future_status::ready                     ){
                                change.store(true);
                                bit_map[i.first].get();
                                if(disp=="stream"){  print(i.second);  }
                                bit_map[i.first]=i.second->poll_async(intrvl);
                            }
                        }
                    }
                    if(disp=="clear"){
                        system("clear");              print(nullptr);
                        for( auto & i : gpio_map ){   print(i.second);   }
                    }
                }
            }
        }
    });
    
//    Process command-line arguments
    
    args_handler.parse(argc-1, argv+1);
    
    return 0;
}
/* ************************************************************************** *\
\* ************************************************************************** */




/* ************************************************************************** *\
    Function definitions
\* ************************************************************************** */

deque<unsigned> get_variadic_pins(param_t p)
{
    deque<unsigned> pins;
    string stmp;
    for (int t=-1; t!=0;) {
        stmp.clear();  p.get() >> stmp;
        if ((t=atoi(stmp.c_str()))>0) {     
            pins.push_back(t);
        }
    }
    for (auto i=stmp.rbegin(); i!=stmp.rend(); i++) {
        p.get().putback(*i);
    }
    return pins;
}


void print(gpio * _g)
{
    static int val_len(10), dir_len(10), low_len(10), p_len(30);
    gpio _d;

    if (_g==nullptr) 
    {
        printf("\t%-*s",    p_len,"GPIO Path Name");
        printf("\t%*s",     val_len,_d.val.attr.c_str());
        printf("\t%*s",     dir_len,_d.dir.attr.c_str());
        printf("\t%*s\n",   low_len,_d.low.attr.c_str());
    }else{
        string p_str = _g->path;
        if (_g->path.length()>=p_len) {
            p_str.replace(0,(p_str.length()-p_len>5
                             ? p_str.length()-p_len : 5),"{...}");
        }
        printf("\t%-*s",    p_len,p_str.c_str());
        printf("\t%*s",     val_len,_g->val.last().c_str());
        printf("\t%*s",     dir_len,_g->dir.last().c_str());
        printf("\t%*s\n",   low_len,_g->low.last().c_str());
    }
}




