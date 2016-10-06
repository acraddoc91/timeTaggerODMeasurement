/**********************************************************************
 *                                                                    *
 *                            SysTypes.h                              *
 *                                                                    *
 **********************************************************************/
/*
 * Project:   ttm (Time Tagging Module)
 *
 * Company:   Roithner Lasertechnik GmbH - Vienna - Austria
 * Support:   office@roithner-laser.com
 *
 * Copyright: Roithner Lasertechnik GmbH
 *            Wiedner Hauptstrasse 76
 *            1040 Vienna
 *            Austria / Europe
 *
 * Decription: */
/** @file
 *  Definition of Basic Types in a System Independent Fashion.
 *//*
 * Tab: 2 spaces
 *
 * Release 4.4.4 - 2016-01-26
 */

#ifndef SysTypes_h
#define SysTypes_h

/**********************************************************************
 *                                                                    *
 **********************************************************************/
// Use "cpp -dM" (+ Cntr-D) to show a list of all predefined macros
#if defined(__APPLE__) && !defined(__MacOSX__)
  #define __MacOSX__ 1
#endif

/**********************************************************************/

#if defined(_WIN32)
  // Note:
  //   Visual Studio 6  --> _MSC_VER = 1200
  //   Visual Studio 7  --> _MSC_VER = 1300
  //   Visual Studio 2003 (MSVC++ 7.1)  --> _MSC_VER = 1310
  //   Visual Studio 2005 (MSVC++ 8.0)  --> _MSC_VER = 1400
  //   Visual Studio 2008 (MSVC++ 9.0)  --> _MSC_VER = 1500
  //   Visual Studio 2010 (MSVC++ 10.0) --> _MSC_VER = 1600
  //   Visual Studio 2012 (MSVC++ 11.0) --> _MSC_VER = 1700
  //   Visual Studio 2013 (MSVC++ 12.0) --> _MSC_VER = 1800
  #if defined (_MSC_VER)  // Microsoft Visual Studio
    #pragma comment(lib, "ws2_32.lib")
  #endif
  #include <Winsock2.h>
  #include <ws2tcpip.h>
//  #include <sys/types.h>
  #include <sys/timeb.h>
//  #include <iostream.h>

  #ifndef _STDINT_H
    #if 1
      typedef unsigned char    uint8_t;
      typedef unsigned short   uint16_t;
      typedef unsigned long    uint32_t;
      typedef unsigned __int64 uint64_t;

      typedef signed char      int8_t;
      typedef signed short     int16_t;
      typedef signed int       int32_t;
      typedef __int64          int64_t;
    #else
      typedef unsigned char    uint8_t;
      typedef unsigned short   uint16_t;
      typedef unsigned int     uint32_t;

      typedef __int8      int8_t;
      typedef __int16     int16_t;
      typedef __int32     int32_t;
      typedef __int64     int64_t;
    #endif
  #endif

  // See /usr/include/inttypes.h for a definition of PRIX64 & Co.
  #ifndef __STDC_FORMAT_MACROS
    #if defined (_WIN32) && defined (_MSC_VER) && (_MSC_VER == 1200)
    // Microsoft Visual Studio 6 has creative names for printing 64-bit ints.
      #define PRId64 "I64d"
      #define PRIu64 "I64u"
      #define PRIX64 "I64X"
    #else
      #define PRId64 "lld"
      #define PRIu64 "llu"
      #define PRIX64 "llX"
    #endif
    #define PRId32 "ld"
    #define PRIu32 "lu"
    #define PRIX32 "lX"

    #if defined(_WIN64)
      #define PRIXPTR "012llX"
    #else
      #define PRIXPTR "08lX"
    #endif
  #endif

  // See /usr/include/stdint.h for a definition of __UINT64_C & Co.
  #ifndef __STDC_LIMIT_MACROS
    #define __INT64_C(X)  (X)
    #define __UINT64_C(X) (X)
  #endif

  #define INT64_C(X) (X)
  #define UINT64_C(X) (X)

  typedef uint32_t  in_addr_t;
  typedef uint16_t  in_port_t;

#if defined (_WIN32) && defined (_MSC_VER) && (_MSC_VER == 1200)
  typedef struct in6_addr
   {
    union
     {
      uint8_t  Byte[16];
      uint16_t Word[8];
      uint32_t Long[4];
     } u;
   } in6_addr;
  // #define in_addr6 in6addr

  typedef struct sockaddr_in6_t {
    short   sin6_family;        /* AF_INET6 */
    u_short sin6_port;          /* Transport level port number */
    u_long  sin6_flowinfo_id;   /* IPv6 flow information */
    struct  in6_addr sin6_addr; /* IPv6 address */
   } sockaddr_in6_t;

  struct sockaddr_storage
   {
    uint16_t ss_family;
    char PaddingA[14];
    union {
      in_addr  IPv4Addr;
      in6_addr IPv6Addr;
     } u;
    char PaddingB[64];
   };
#else
  typedef struct sockaddr_in6 sockaddr_in6_t;
#endif

  typedef struct in6_addr in6_addr_t;

  #define s6_addr16 u.Word

  // #define sa_family_t ADDRESS_FAMILY
  #define sa_family_t short

  typedef uint16_t  bool_t;

  typedef int socklen_t;

  #define ThreadSleep(X)      Sleep((X)*1000)
  #define ThreadMilliSleep(X) Sleep(X)
  #define ThreadMicroSleep(X) Sleep((X)/1000)

  #define NetData_t char
  #define SockOpt_t char

  #define htonll(X) __bswap_64((X))
  #define ntohll(X) __bswap_64((X))
  #define __bswap_32(X) \
            (((((uint32_t) (X)) & 0xFF000000L) >> 24) | \
             ((((uint32_t) (X)) & 0x00FF0000L) >>  8) | \
             ((((uint32_t) (X)) & 0x0000FF00L) <<  8) | \
             ((((uint32_t) (X)) & 0x000000FFL) << 24))
  #define __bswap_64(X) \
            (((((uint64_t) (X)) & __UINT64_C(0xFF00000000000000)) >> 56) | \
             ((((uint64_t) (X)) & __UINT64_C(0x00FF000000000000)) >> 40) | \
             ((((uint64_t) (X)) & __UINT64_C(0x0000FF0000000000)) >> 24) | \
             ((((uint64_t) (X)) & __UINT64_C(0x000000FF00000000)) >>  8) | \
             ((((uint64_t) (X)) & __UINT64_C(0x00000000FF000000)) <<  8) | \
             ((((uint64_t) (X)) & __UINT64_C(0x0000000000FF0000)) << 24) | \
             ((((uint64_t) (X)) & __UINT64_C(0x000000000000FF00)) << 40) | \
             ((((uint64_t) (X)) & __UINT64_C(0x00000000000000FF)) << 56))

  #define strcasecmp(X,Y) _stricmp(X,Y)

#if defined (_WIN32) && defined (_MSC_VER) && (_MSC_VER < 1800)
  #define round(X) floor((X) + 0.5)
#endif

/**********************************************************************
 *                                                                    *
 **********************************************************************/

#elif defined (__linux) || defined(__MacOSX__)

  // We want to operate on files greater that 2GiByte. Thus the FILE
  // structure (and all functions that work on it) need to be 64-bit
  // savy.
  // _LARGEFILE64_SOURCE and _FILE_OFFSET_BITS are used by <features.h>
  // to define __USE_FILE_OFFSET64 and __USE_LARGEFILE64 that cause
  // <stdio.h> to use 64-bit aware functions.
  #ifndef _LARGEFILE64_SOURCE
    #define _LARGEFILE64_SOURCE
  #endif
  #ifndef _FILE_OFFSET_BITS
    #define _FILE_OFFSET_BITS 64
  #endif

  // We want to be able to print 32- and 64-bit numbers in a compiler
  // independent fashion. Thus we shall ask the compiler to provide
  // definitions for suitable print formats...
  #ifndef __STDC_FORMAT_MACROS
    #define __STDC_FORMAT_MACROS
  #endif
  #ifndef __STDC_LIMIT_MACROS
    #define __STDC_LIMIT_MACROS
  #endif

  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>

#if 1
  #include <inttypes.h>
  #include <stdint.h>
#else
  typedef unsigned char      uint8_t;
  typedef unsigned short     uint16_t;
  typedef unsigned int       uint32_t;
  typedef unsigned long long uint64_t;

  typedef signed char      int8_t;
  typedef signed short     int16_t;
  typedef signed int       int32_t;
  typedef signed long long int64_t;
#endif

  // See /usr/include/inttypes.h for a definition of PRIX64 & Co.
  #if (defined(__cplusplus)) && !defined(__STDC_FORMAT_MACROS)
    #define PRId64 "lld"
    #define PRIX64 "llX"

    #define PRId32 "ld"
    #define PRIu32 "lu"
    #define PRIX32 "lX"
  #endif

  #ifndef FALSE
    #define FALSE 0
    #define TRUE  1
  #endif

  typedef int SOCKET;
  #define INVALID_SOCKET ((SOCKET) -1)
  typedef void  NetData_t;
  typedef char SockOpt_t;
  typedef uint16_t bool_t;

  typedef struct in6_addr in6_addr_t;
  #if defined(__MacOSX__)
    #define s6_addr16 __u6_addr.__u6_addr16
  #endif

  // See /usr/include/stdint.h for a definition of __UINT64_C & Co.
  #if !defined(__STDC_LIMIT_MACROS) || !defined(__INT64_C)
    #define __INT64_C(X)  (X ## LL)
    #define __UINT64_C(X) (X ## ULL)
  #endif

  #ifndef __STDC_CONSTANT_MACROS
  #ifndef INT64_C
    #define INT64_C(X)  (X ## LL)
  #endif
  #ifndef UINT64_C
    #define UINT64_C(X) (X ## ULL)
  #endif
  #endif

  #define closesocket(s) close(s)

  #include <unistd.h>
  #define ThreadSleep(X)      sleep(X)
  #define ThreadMilliSleep(X) usleep((X) * 1000)
  #define ThreadMicroSleep(X) usleep(X)

  #if 0
    #define __bswap_64(X) ((((uint64_t) (__bswap_32(((uint64_t) (X)) & 0x0000FFFF))) << 32) | \
                           ((uint64_t) (__bswap_32((((uint64_t) (X)) >> 32) & 0x0000FFFF))))
  #endif

  #if defined (__MacOSX__)
    #define __bswap_32(X) OSSwapConstInt32(X)
    #define __bswap_64(X) OSSwapConstInt64(X)
  #endif

  #if __BYTE_ORDER == __LITTLE_ENDIAN
    #define ntohll(x)     __bswap_64 (x)
    #define htonll(x)     __bswap_64 (x)
  #else
    #define ntohll(x)       (x)
    #define htonll(x)       (x)
  #endif

/**********************************************************************
 *                                                                    *
 **********************************************************************/
//#else
//  #error Unknown Operating System Type - Please use either Linux, Windows or MacOSX ...
#endif


#endif /* SysTypes_h */
