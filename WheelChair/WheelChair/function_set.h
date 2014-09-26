//
//  function_set.h
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef __CNC__function_map__
#define __CNC__function_map__


#include <string>
//#include <set>
#include <functional>
#include <unordered_set>



/// The return type of the function binding.
typedef int             ret_t;     
/// The paramaeter type of the function binding.
typedef std::string     param_t;   

/** A function binding with a signature of the form:
 @code
 ret_t anonymous_function(param_t);
 @endcode
 This function is intended to mirror that of \c main() .
 It can be called in any manner analagous to:
 @code
 auto  f = funct_t( [&](param_t p)->ret_t{return p.length();} );
 ret_t r = f( "parameter string" );
 @endcode
 In this example a lambda function is used to construct the \c funct_t
 object. Primarily this is what the type is used for. The typedefs 
 \c ret_t and \c param_t were chosen to emphasize the structure of a lambda
 and its similarity to a normal function.
 */
typedef std::function< ret_t(param_t) > funct_t;

typedef std::pair<funct_t, param_t> funct_call;



/** A function signature; stores related data.
 
 The primary benefit of this structure is that it also holds all the related 
 metadata of the function it stores. This means that the functions can be 
 be uniformly searched when the applicable function is unknown, and that the 
 usage syntax for a function can be printed via a uniform interface.
 @see help_short()  @see help_long()
 @note
    Since it inherits from \c std::function<> , it also posesses the call
 operator and can therefore be used like any other functor (function object).
 */
struct function_sig : funct_t
{
    std::string name, args, desc;
    
    /// Defualt constructor makes empty strings and useless functor.
    function_sig();
    
    /** Constructor of the function signature structure.
     @param     _name   
        The name of the function.
     @param     _args   
        The template argument string; suggests their order and meaning.
     @param     _desc
        A description of what the function does.
     @param     _funct
        The function that will be executed upon use of the call operator.
     */
    function_sig(std::string _name, std::string _args , std::string _desc, 
              funct_t _funct);
    
    /** Returns the basic usage of the function.
     @return    String of the form: "\\t[ name args ]\\n"
     */
    std::string         help_short()    const ;
    
    /** Returns the full description of the function.
     @return    String of the form: "\\t[ name args ]\\n\\t: desc \\n"
     @note  Description \c desc will be formatted such that it is soft-wrapped 
     to 60 columns or hard-wrapped at each new-line.
     */
    std::string         help_long()     const ;
    
    /** Concatenates all contained strings interleaved with spaces.
     @note Intended for use by search function.
     */
    std::string         text()          const ;

};

namespace std 
{   
    /** Hash functor specialization for \c function_sig() class.
     @details 
        Required because \c std::unordered_set<> (which is secretly a hash_map)
     lacks can not infer the template correctly. (SFINAE) 
     */
    template <> struct hash<function_sig>
    {   size_t operator()(const function_sig & x) const;
    };
    
    /** Equivalence operator for \c function_sig() class.
     */
    bool operator==( const function_sig& _f1,  const function_sig& _f2) ;
}



/** A set of function signatures with a few helper functions added.
 */
struct function_set :  std::unordered_set<function_sig>
{
    /** Default constructor of function set. 
     @note Comes with three helper functions for using the set. They are:
     @code
     help    <query> // Calls ::help_long() on specified or all functions. 
     options <query> // Calls ::help_short() on specified or all functions.
     find    <query> // Calls ::help_long() on all functions with text() 
                     //  which contain <query>.
     @endcode
     @sa \c defualt_function is initialized.
     */
    function_set() ;
    
    /** Explicit constructor; contains no helper functions.
     @warning   Comes with @b no helper functions.
     @sa \c defualt_function is uninitialized.
     */
    function_set(std::unordered_set<function_sig> set):
        std::unordered_set<function_sig>(set)
    {
    }
    
    /** Access operator
     @param _name    The name of the function.
     @pre   The function must exist.
     @post  If the function does not exist, calls abort();
     */
    inline
    function_sig    operator[](std::string  _name);
    
    /** The defualt function to call if \c parse() is called on an empty or 
     malformed string.
     */
    function_sig    default_function;
    
    
    int             parse(std::string args);
    
    int             parse(int argc, const char * argv[]);
    
    inline 
    bool            has_function(std::string _name);
    
};






#endif /* defined(__CNC__function_map__) */
