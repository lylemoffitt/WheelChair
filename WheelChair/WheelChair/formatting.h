//
//  formatting.h
//  WheelChair
//
//  Created by Lyle Moffitt on 9/25/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef CNC_formatting_h
#define CNC_formatting_h

#include <string>
#include <cstring>

template<class ... var_t> 
inline 
std::string
stringf( const char * format, var_t ... f_args )
{
    using namespace std;
    size_t _sz = (sizeof...(f_args)) * sizeof(size_t) * string(format).size();
    char buf [_sz];
    memset(&buf, 0, _sz);
    sprintf(buf, format, f_args... );
    return std::string( buf );
}



#include <vector>
template<typename _type>
struct init_ls : public std::vector<_type>
{
    
    init_ls( std::initializer_list<_type> _ls):     
    std::vector<_type>(_ls)
    {
    }
};




/** Concatenate two things together.
 * @def CAT( a,b )
 * @param a,b Any two things. 
 * @return Those two things concatenated together. NOT a string.
 */
#define CAT( a,b )			\
a ## b

/** Division with (long double) casting and DIV0 checking.
 *
 * This is a super handy macro. There are so many times in C/C++ that
 * a division operation gets messed up. This avoids all the technical 
 * complications and simple divides as ccurately. 
 * @note <tt> long double </tt> Is a **quadrouple** precision float.   
 * @def DIV( a,b )
 * @param 	a,b 	Any two things that evaluate to numbers.
 * @return  Division of \c a over \c b , or just \c a if \c b is zero.
 */
#define DIV( a,b )		    \
(	((double) a ) /	\
    ((double) NZ( b ))	\
)

/** Make sure it's Not Zero.
 * @def NZ( a )
 * @param 	a 	Anything that evaluates to a number.
 * @return If \c a is not zero, then \a is returned. 
 * Otherwise 1.0 is returned.
 */
#define NZ( a )				\
(	((a) != 0)	? (a) : 1	\
)

/** Converts directly to a string, does not evaluate.
 * @def STR( str )
 * @param 	str 	Something that you like to be literally converted 
 * to a sring without evaluation.
 * @return The _literal_ C-string of what you put in.
 */
#define STR( str )			\
#str

/** Returns the greater of the two inputs
 * @def MAX( a,b )
 * @param 	a,b 	Any two thing that evaluate to numbers.
 * @return  The greater of the two.
 */
#define MAX( a,b )			\
(	(a) < (b) ? (b) : (a)	\
)

/** Returns the lesser of the two inputs
 * @def MIN( a,b )
 * @param 	a,b 	Any two thing that evaluate to numbers.
 * @return  The lesser of the two.
 */
#define MIN( a,b )			\
(	(a) > (b) ? (b) : (a)	\
)

/** Return the absolute value of the input
 * @def ABS( a )
 * @param 	a 	Anything that evaluate to a number.
 * @return  The number or its negation, whichever is not less than 0
 */
#define ABS( a )			\
(	(a) < 0 ? -(a) : (a)	\
)

/** A basic "No Op" style command
 * @def NO_OP
 */
#define NO_OP               \
do{                         \
}while(0)	

/** Used to define where the LOG outputs are directed
 * @def LOUT
 * A shortcut to std::cout , std::cerr,  or where ever you 
 * want the LOG(N) outputs directed.
 */
#define LOUT std::cout <<

/** Print command to LOUT and then print its evaluated result to LOUT
 * @def ECHO( a )
 * @param 	a Any evaluatable expression 
 * @return  LOUT "\t" STR( a ) " = " << a << "\t"
 */
#define ECHO( a )			\
LOUT STR( a ) " = " << a    \





#endif //CNC_formatting_h
