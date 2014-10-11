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

using namespace std;

/* ************************     function_sig       **************************** */

function_sig::function_sig():
    name(""), args(""), desc(""), funct_t([&](param_t p)->ret_t{return;})
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
    return stringf("\n\t[ %s %s ]",name.c_str() ,args.c_str() );
}

string  
function_sig::help_long() const 
{   
    string d = desc,  s=help_short();
    for(size_t p1=0,p2=0; (p1=d.find('\n', p1))!=string::npos; ){
        if((p1-p2)>WRAP_MARGIN){ 
            p2=d.rfind(' ',p2+WRAP_MARGIN); d.replace(p2, 1, "\n "); p1=p2; 
        }
        d.replace(p1, 1,  "\n\t: ");  p1+=4;  p2=p1;
    }
    return s+"\n\t: "+d+"\n";
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
    function_sig _f ({ "","", "", [&](param_t p)->ret_t{}});
    _f.name = _name;
    return (find(_f) != end());
}

function_sig
function_set::operator[](std::string  _name)
{
    function_sig _f ({ "","", "", [&](param_t p)->ret_t{}});
    _f.name = _name;
    auto foundit = find(_f);
    assert(foundit!=end() && "Function name not found; try \'help\'.");
    if (foundit == end()) {    
        throw std::invalid_argument
        ("Function:[\""+_name+"\"] not found in function set.");
    }
    return (*foundit);
}

int 
function_set::parse(std::stringstream & args)
{
    for( size_t len = args.str().length(); len>0 ; --len){
        args.put(tolower(args.get()));
    }
    
    if (args.str().empty()){
        if (default_function){  default_function( args );  }
        return -1;
    }else{
        std::string fnct_name;
        std::stringstream bad_args;
        args >> fnct_name;
        while (!fnct_name.empty()){
            if (has_function(fnct_name)) {
                (*this)[fnct_name]( std::ref(args) );   
                fnct_name.clear();
            }else{
                bad_args << fnct_name;
            }
            args >> fnct_name;
        }
        if (bad_args.tellp()>0) {
            default_function(bad_args);
        }
        
    }
    return 0;
}

int 
function_set::parse(int argc, const char * argv[])
{
    std::stringstream ss;
    for (int i =0; i<argc; ++i) {   
        ss.put(' ');
        for (int j =0; argv[i][j]; j++) {
            ss.put( tolower(argv[i][j]) );
        }
    }
    return parse( ss );
}

function_set::function_set() :  std::unordered_set<function_sig>
({
    {"help", "<query/s>" ,    
        "Print help for all functions, or for the specified one.",
        [&](param_t p)->ret_t    
        {
            if (p.get().str().empty()) {
                std::cout << "\nUsage: \n";
                for ( auto &i : (*this) ) {  cerr << i.help_short(); }
                cout << endl;  
                return;
            }
            unordered_set<string> n_set;
            stringstream out;
            for( string _s(" "); !_s.empty(); _s.clear(),p.get()>>_s )
            {
                if (this->has_function(_s)) {    
                     (*this)[_s].help_long();  continue;
                }
                for (auto i = this->begin() ; i!=this->end(); i++) 
                {
                    std::string _low, _src(i->text());
                    for (auto j : _src ) { _low += std::tolower(j) ;}
                    
                    if ( _low.find(_s)==_low.npos ){  
                        out << _s<<", ";  continue;
                    }
                    if ( n_set.find(i->name)==n_set.end() ){
                        n_set.insert(i->name);
                    }
                }
            }
            if (out.tellp()>0) {
                cerr << "Could not find any matches to : " ;
                cerr << out.str()<<"\b\b\n";
            }
            if (!n_set.empty()) {
                cout<< "Found the following matches : \n";
                for ( auto &i : n_set ){  cout<<(*this)[i].help_long();  }
            }
            cout << endl;
        } 
    }
}) 
{
    
    default_function = (*this)["help"];
}







