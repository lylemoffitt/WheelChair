//
//  function_set.cpp
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#include "function_set.h"
#include "formatting.h"

#include <iostream>
#include <cassert>
#include <sstream>


//#include <boost/format.hpp>
//#include <boost/algorithm/string.hpp>

//namespace b_ = boost;
using namespace std;

/* ************************     function_sig       **************************** */

function_sig::function_sig():
    name(""), args(""), desc(""), funct_t([&](param_t p)->ret_t{return -1;})
{
}

function_sig::function_sig(string _name, string _args , string _desc, 
                     funct_t _funct):
    name(_name), args(_args), desc(_desc), funct_t(_funct)
{
}

string  
function_sig::help_short() const 
{   
//    return b_::str(b_::format("\t[ %1% %2% ]\n") %name %args );
    return stringf("\t[ %s %s ]\n",name.c_str() ,args.c_str() );
}

string  
function_sig::help_long() const 
{   
//    return b_::str(b_::format("\t[ %1% %2% ]\n\t: %3%\n") %name %args %desc );
//    return stringf("\t[ %s %s ]\n\t: %s \n", 
//                   name.c_str(), args.c_str(), desc.c_str() );
    string d = desc,  s=help_short();
    for(size_t p1=0,p2=0; p1!=string::npos; p1=d.find('\n', p1)){
        if((p1-p2)>60){ p2=d.rfind(' ',p2+60); d.replace(p2, 1, "\n "); p1=p2; }
        d.replace(p1, 1,  "\n\t: ");  p1+=4;  p2=p1;
    }
    return s+"\b"+d+"\n";
}

string 
function_sig::text()const 
{
    return name+" "+args+" "+desc; 
}

namespace std 
{   
    size_t 
    hash<function_sig>::operator()(const function_sig & x) const
    {   return hash<std::string>()( x.name );
    }
    
    
    bool operator==( const function_sig& _f1,const function_sig& _f2) 
    {   return _f1.name == _f2.name;
    }
}



/* **********************      function_set      **************************** */

bool 
function_set::has_function(std::string  _name)
{
    function_sig _f ({ "","", "", [&](param_t p)->ret_t{return 0;}});
    _f.name = _name;
    return (find(_f) != end());
}

function_sig
function_set::operator[](std::string  _name)
{
    function_sig _f ({ "","", "", [&](param_t p)->ret_t{return 0;}});
    _f.name = _name;
    auto foundit = find(_f);
    assert(foundit!=end() && "Function name not found.");
    if (foundit == end()) {    
        throw std::invalid_argument::invalid_argument
        ("Function:[\""+_name+"\"] not found in function set.");
    }
    return (*foundit);
}

int 
function_set::parse(std::string  args)
{
    std::stringstream ss(args);

    if (args.empty()){
        if (default_function){  default_function("");    }
        return -1;
    }else{
        do{
            std::string fnct_name;
            ss >> fnct_name;
            (*this)[fnct_name]( ss.str() );
        }while (!ss.str().empty()) ;
    }
    return 0;
}

int 
function_set::parse(int argc, const char * argv[])
{
    std::stringstream ss;
    for (int i =0; i<argc; ++i) {
        ss << argv[i];
    }
    return parse( ss.str() );
}

function_set::function_set() :  std::unordered_set<function_sig>
({
    {"help", "<query>" ,    
        "Print help for all functions, or for the specified one.",
        [&](param_t p)->ret_t    
        {
            if (p.empty()) {
                std::cout << "\nUsage: \n";
                for (auto i = this->begin() ; i!=this->end(); i++) {
                    cerr << i->help_long();
                }
            }else if (this->has_function(p)) {    
                (*this)[p].help_long();
            }else{
                if (this->has_function("find")) {
                    (*this)["find"](p);
                }else{
                    cout<<"Try with a null query to see a list of "
                    <<"possible options.\n";
                }
            }
            return 0;
        } 
    },
    {"options", "<query>",
        "Print the options for a specified function, or for all of them",
        [&](param_t p)->ret_t  
        {
            if (!p.empty()) {   
                (*this)[p].help_short();     return 0;
            }else {
                for (auto i = this->begin() ; i!=this->end(); i++) {
                    cerr << i->help_short();
                }
                return 0;
            }
        } 
    },
    {"find", "<query>", 
        "Find and list all matching functions.",
        [&](param_t p)->ret_t  
        {
            ostringstream out;
            for (auto i = this->begin() ; i!=this->end(); i++) {
                std::string _low, _src=(i->text());
                for (auto i : _src ) { _low += std::tolower(i) ;}
                if ( _low.find(p) != std::string::npos){
                    cout << i->help_long();
                }
            }
            if (out.tellp()>0) {
                cout<<"\nFound: \n"<<out.str();
            }else{
                cout<<"\nNo such function exists.\n";
            }
            return 0;
        } 
    },
}) 
{
    default_function = (*this)["help"];
}







