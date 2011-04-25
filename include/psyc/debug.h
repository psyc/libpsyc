#ifndef DEBUG_H
# define DEBUG_H

#ifdef DEBUG
# include <stdio.h>
# define PP(args) printf args;
#else
# define PP(args)
#endif

#ifdef TEST
# define PT(MSG)	PP(MSG);
# define DT(CODE)	CODE
#else
# define PT(MSG)
# define DT(CODE)
#endif

/* simplified form of conditional compilation */

#ifndef DEBUG_FLAGS
# ifdef DEBUG
#  if DEBUG == 1
#   define DEBUG_FLAGS 0x03
#  else
#   if DEBUG == 2
#    define DEBUG_FLAGS 0x07
#   else
#    if DEBUG == 3
#     define DEBUG_FLAGS 0x0f
#    else
#     if DEBUG == 4
#      define DEBUG_FLAGS 0x1f
#     else
#      define DEBUG_FLAGS 0x01
#     endif
#    endif
#   endif
#  endif
# else
#  define DEBUG_FLAGS 0x00 /* no debugging */
# endif
#endif

#if DEBUG_FLAGS & 0x01
# define D0(CODE)	CODE
# define P0(MSG)	PP(MSG);
#else
# define D0(CODE)
# define P0(MSG)
#endif

#if DEBUG_FLAGS & 0x02
# define D1(CODE)	CODE
# define P1(MSG)	PP(MSG);
#else
# define D1(CODE)
# define P1(MSG)
#endif

#if DEBUG_FLAGS & 0x04
# define D2(CODE)	CODE
# define P2(MSG)	PP(MSG);
#else
# define D2(CODE)
# define P2(MSG)
#endif

#if DEBUG_FLAGS & 0x08
# define D3(CODE)	CODE
# define P3(MSG)	PP(MSG);
#else
# define D3(CODE)
# define P3(MSG)
#endif

#if DEBUG_FLAGS & 0x10
# define D4(CODE)	CODE
# define P4(MSG)	PP(MSG);
#else
# define D4(CODE)
# define P4(MSG)
#endif

#endif // DEBUG_H
