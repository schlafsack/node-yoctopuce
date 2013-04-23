/*********************************************************************
 *
 * $Id: ydef.h 10864 2013-04-03 16:20:26Z mvuilleu $
 *
 * Standard definitions common to all yoctopuce projects
 *
 * - - - - - - - - - License information: - - - - - - - - -
 *
 * Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.
 *
 * 1) If you have obtained this file from www.yoctopuce.com,
 *    Yoctopuce Sarl licenses to you (hereafter Licensee) the
 *    right to use, modify, copy, and integrate this source file
 *    into your own solution for the sole purpose of interfacing
 *    a Yoctopuce product with Licensee's solution.
 *
 *    The use of this file and all relationship between Yoctopuce
 *    and Licensee are governed by Yoctopuce General Terms and
 *    Conditions.
 *
 *    THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 *    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
 *    WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS
 *    FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
 *    EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
 *    INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 *    COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR
 *    SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT
 *    LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
 *    CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
 *    BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
 *    WARRANTY, OR OTHERWISE.
 *
 * 2) If your intent is not to interface with Yoctopuce products,
 *    you are not entitled to use, read or create any derived
 *    material from this source file.
 *
 *********************************************************************/

#ifndef  YOCTO_DEF_H
#define  YOCTO_DEF_H
#ifdef  __cplusplus
extern "C" {
#endif


#if defined(_WIN32)
// Windows C compiler
#define WINDOWS_API
#ifdef _WIN64
#define __64BITS__
#define __WIN64__
#else
#define __32BITS__
#endif
#ifdef _MSC_VER
typedef unsigned char           u8;
typedef signed char             s8;
typedef unsigned short int      u16;
typedef signed short int        s16;
typedef unsigned long int       u32;
typedef signed long int         s32;
typedef unsigned long long      u64;
typedef signed long long        s64;
#define VARIABLE_SIZE

#else
typedef unsigned char           u8;
typedef signed char             s8;
typedef unsigned short int      u16;
typedef signed short int        s16;
typedef unsigned int            u32;
typedef signed int              s32;
#ifdef __BORLANDC__
typedef unsigned __int64        u64;
typedef __int64                 s64;
#else
typedef unsigned long           u64;
typedef signed long             s64;
#define VARIABLE_SIZE           0
#endif

#endif



#elif defined(__C30__)
// Microchip C30
#define MICROCHIP_API
#define __16BITS__

typedef unsigned char           u8;
typedef signed char             s8;
typedef unsigned short int      u16;
typedef signed short int        s16;
typedef unsigned long int       u32;
typedef signed long int         s32;
typedef unsigned long long      u64;
typedef signed long long        s64;
#define VARIABLE_SIZE           0

#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
//#warning IOS simulatore platform
#define IOS_API
#elif TARGET_OS_IPHONE
//#warning IOS platform
#define IOS_API
#elif TARGET_OS_MAC
#define OSX_API
#if defined(__i386__)
#define __32BITS__
#elif defined(__x86_64__)
#define OSX_API
#define __64BITS__
#else
#error Unsupported MAC OS X architecture
#endif
#else
#error Unsupported Apple target
#endif
// Mac OS X C compiler
typedef unsigned char           u8;
typedef signed char             s8;
typedef unsigned short int      u16;
typedef signed short int        s16;
typedef unsigned int            u32;
typedef signed int              s32;
typedef unsigned long           u64;
typedef signed long             s64;
#define VARIABLE_SIZE           0
#include <pthread.h>
#include <errno.h>

#elif defined(__linux__)
// gcc compiler on linux
#define LINUX_API
#if defined(__i386__)
#define __32BITS__
#elif defined(__x86_64__)
#define __64BITS__
#endif

#include <stdint.h>
typedef uint8_t                 u8;
typedef int8_t                  s8;
typedef uint16_t                u16;
typedef int16_t                 s16;
typedef uint32_t                u32;
typedef int32_t                 s32;
typedef uint64_t                u64;
typedef int64_t                 s64;
#define VARIABLE_SIZE           0
#include <pthread.h>
#include <errno.h>

#else
#warning UNSUPPORTED ARCHITECTURE, please edit yocto_def.h !
#endif

typedef u32   yTime;            /* measured in milliseconds */
typedef u32   u31;              /* shorter unsigned integers */
typedef s16   yHash;
typedef u16   yBlkHdl;          /* (yHash << 1) + [0,1] */
typedef yHash yStrRef;
typedef yHash yUrlRef;
typedef s32   YAPI_DEVICE;      /* yStrRef of serial number */
typedef s32   YAPI_FUNCTION;    /* yStrRef of serial + (ystrRef of funcId << 16) */

#define INVALID_HASH_IDX    -1  /* To use for yHash, yStrRef, yApiRef types */
#define INVALID_BLK_HDL     0   /* To use for yBlkHdl type */

#ifdef MICROCHIP_API
typedef u8              YSOCKET;
typedef s8              YYSBIO;
typedef s8              YUSBIO;
typedef s8              YUSBDEV;
#else
#if defined(WINDOWS_API) && defined(__64BITS__)
typedef unsigned __int64 YSOCKET;
#else
typedef int             YSOCKET;
#endif
typedef s32             YYSBIO;
typedef s32             YUSBIO;
typedef s32             YUSBDEV;
#endif

#define YIO_USB          1
#define YIO_TCP          2
#define YIO_YSB          3
    
#define YIO_REMOTE_CLOSE 1
#define YIO_ASYNC        2

typedef struct{
    u8      type;
    u8      flags;
    yUrlRef url;
    union {
        // make sure this union stay 32 bit, YIOHDL is used in all foreign APIs
        u32     tcpreqidx;
        YUSBIO  hdl;
        YYSBIO  ysb;
    };
} YIOHDL;


#define INVALID_YHANDLE (-1)

#define S8(x)   ((s8)(x))
#define S16(x)  ((s16)(x))
#define S32(x)  ((s32)(x))
#define S64(x)  ((s64)(x))
#define U8(x)   ((u8)(x))
#define U16(x)  ((u16)(x))
#define U32(x)  ((u32)(x))
#define U64(x)  ((u64)(x))

#define U8ADDR(x)  ((u8 *)&(x))
#define U16ADDR(x) ((u16 *)&(x))

#define ADDRESSOF(x)    (&(x))    
#define PTRVAL(x)       (*(x))    


//#define DEBUG_CRITICAL_SECTION

#ifdef DEBUG_CRITICAL_SECTION

typedef struct {
    u32                 no;
    pthread_mutex_t     *dummy_cs;
} yCRITICAL_SECTION;


void yDbgInitializeCriticalSection(const char* fileid, int lineno, yCRITICAL_SECTION *cs);
void yDbgEnterCriticalSection(const char* fileid, int lineno, yCRITICAL_SECTION *cs);
int yDbgTryEnterCriticalSection(const char* fileid, int lineno, yCRITICAL_SECTION *cs);
void yDbgLeaveCriticalSection(const char* fileid, int lineno, yCRITICAL_SECTION *cs);
void yDbgDeleteCriticalSection(const char* fileid, int lineno, yCRITICAL_SECTION *cs);

#define yInitializeCriticalSection(cs)  yDbgInitializeCriticalSection(__FILE_ID__,__LINE__,cs)
#define yEnterCriticalSection(cs)       yDbgEnterCriticalSection(__FILE_ID__,__LINE__,cs)
#define yTryEnterCriticalSection(cs)    yDbgTryEnterCriticalSection(__FILE_ID__,__LINE__,cs)
#define yLeaveCriticalSection(cs)       yDbgLeaveCriticalSection(__FILE_ID__,__LINE__,cs)
#define yDeleteCriticalSection(cs)      yDbgDeleteCriticalSection(__FILE_ID__,__LINE__,cs)

#elif defined(MICROCHIP_API)

#define yCRITICAL_SECTION               u8
#define yInitializeCriticalSection(cs)
#define yEnterCriticalSection(cs)
#define yTryEnterCriticalSection(cs)    1
#define yLeaveCriticalSection(cs)
#define yDeleteCriticalSection(cs)

#elif defined(WINDOWS_API)

#define yCRITICAL_SECTION               CRITICAL_SECTION
#define yInitializeCriticalSection(cs)  InitializeCriticalSection(cs)
#define yEnterCriticalSection(cs)       EnterCriticalSection(cs)
#define yTryEnterCriticalSection(cs)    TryEnterCriticalSection(cs)
#define yLeaveCriticalSection(cs)       LeaveCriticalSection(cs)
#define yDeleteCriticalSection(cs)      DeleteCriticalSection(cs)

#else
typedef struct {
    pthread_mutex_t     *mutex_ptr;
} yCRITICAL_SECTION;

void yInitializeCriticalSection(yCRITICAL_SECTION *cs);
void yEnterCriticalSection(yCRITICAL_SECTION *cs);
int yTryEnterCriticalSection(yCRITICAL_SECTION *cs);
void yLeaveCriticalSection(yCRITICAL_SECTION *cs);
void yDeleteCriticalSection(yCRITICAL_SECTION *cs);
#endif

typedef enum {
    YAPI_SUCCESS          = 0,      // everything worked allright
    YAPI_NOT_INITIALIZED  = -1,     // call yInitAPI() first !
    YAPI_INVALID_ARGUMENT = -2,     // one of the arguments passed to the function is invalid
    YAPI_NOT_SUPPORTED    = -3,     // the operation attempted is (currently) not supported
    YAPI_DEVICE_NOT_FOUND = -4,     // the requested device is not reachable
    YAPI_VERSION_MISMATCH = -5,     // the device firmware is incompatible with this API version
    YAPI_DEVICE_BUSY      = -6,     // the device is busy with another task and cannot answer
    YAPI_TIMEOUT          = -7,     // the device took too long to provide an answer
    YAPI_IO_ERROR         = -8,     // there was an I/O problem while talking to the device
    YAPI_NO_MORE_DATA     = -9,     // there is no more data to read from
    YAPI_EXHAUSTED        = -10,    // you have run out of a limited ressource, check the documentation
    YAPI_DOUBLE_ACCES     = -11,    // you have two process that try to acces to the same device
    YAPI_UNAUTHORIZED     = -12     // unauthorized access to password-protected device
} YRETCODE;

#define YISERR(retcode)   ((retcode) < 0)

// Yoctopuce arbitrary constants
#define YOCTO_API_VERSION_STR       "1.01"
#define YOCTO_API_VERSION_BCD       0x0101
#include "yversion.h"
#define YOCTO_DEFAULT_PORT          4444
#define YOCTO_VENDORID              0x24e0
#define YOCTO_DEVID_FACTORYBOOT     1
#define YOCTO_DEVID_BOOTLOADER      2
#define YOCTO_DEVID_HIGHEST         0xfefe

// standard buffer sizes
#define YOCTO_ERRMSG_LEN            256
#define YOCTO_MANUFACTURER_LEN      20
#define YOCTO_SERIAL_LEN            20
#define YOCTO_BASE_SERIAL_LEN        8
#define YOCTO_PRODUCTNAME_LEN       28
#define YOCTO_FIRMWARE_LEN          22
#define YOCTO_LOGICAL_LEN           20
#define YOCTO_FUNCTION_LEN          20
#define YOCTO_PUBVAL_SIZE            6 // Size of the data (can be non null terminated)
#define YOCTO_PUBVAL_LEN            16 // Temporary storage, >= YOCTO_PUBVAL_SIZE+2

// firmware description
typedef union {
    u8      asBytes[YOCTO_FIRMWARE_LEN];
    struct {
        char    buildVersion[YOCTO_FIRMWARE_LEN-2];
        u16     yfsSignature;
    } data;
} yFirmwareSt;

// device description
typedef struct {
    u16     vendorid;
    u16     deviceid;
    u16     devrelease;
    u16     nbinbterfaces;
    char    manufacturer[YOCTO_MANUFACTURER_LEN];
    char    productname[YOCTO_PRODUCTNAME_LEN];
    char    serial[YOCTO_SERIAL_LEN];
    char    logicalname[YOCTO_LOGICAL_LEN];
    char    firmware[YOCTO_FIRMWARE_LEN];
    u8      beacon;
} yDeviceSt;

// definitions for USB protocl

#ifndef C30
#define Nop()
#pragma pack(push,1)
#endif


#define USB_PKT_SIZE            64
#define YPKT_USB_VERSION_BCD    0x0205


#define YPKT_STREAM             0
#define YPKT_CONF               1


#define TO_SAFE_U16(safe,unsafe)        {(safe).low = (unsafe)&0xff; (safe).high=(unsafe)>>8;}
#define FROM_SAFE_U16(safe,unsafe)      {(unsafe) = (safe).low |((u16)((safe).high)<<8);}

typedef struct {
    u8 low;
    u8 high;
} SAFE_U16;


#define YSTREAM_EMPTY       0
#define YSTREAM_TCP         1
#define YSTREAM_TCP_CLOSE   2
#define YSTREAM_NOTICE      3


#define YPKTNOMSK   (0x7)
typedef struct {
    u8 pktno    : 3;
    u8 stream   : 5;
    u8 pkt      : 2;
    u8 size     : 6;
} YSTREAM_Head;

#define USB_CONF_RESET      0
#define USB_CONF_START      1
#define USB_CONF_RETRY      2

typedef union{
    struct{
        SAFE_U16  api;
        u8  ok;
        u8  ifaceno;
        u8  nbifaces;
    }reset;
    struct{
        u8  nbifaces;
    }start;
    struct{
        u8  pktno;
        u8  nbmissing;
    }retry;
} USB_Conf_Pkt;

#define NOTIFY_1STBYTE_MAXTINY  63  
#define NOTIFY_1STBYTE_MINSMALL 128

#define NOTIFY_PKT_NAME        0
#define NOTIFY_PKT_PRODNAME    1
#define NOTIFY_PKT_CHILD       2
#define NOTIFY_PKT_FIRMWARE    3
#define NOTIFY_PKT_FUNCNAME    4
#define NOTIFY_PKT_FUNCVAL     5
#define NOTIFY_PKT_STREAMREADY 6
#define NOTIFY_PKT_LOG         7
#define NOTIFY_PKT_FUNCNAMEYDX 8

typedef struct{
    char        serial[YOCTO_SERIAL_LEN];
    u8          type;
}Notification_header;

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4200 )
#endif

typedef struct{
    u8          funydx;
    char        pubval[VARIABLE_SIZE]; // deduce actual size from YSTREAM_head
}Notification_tiny;

typedef struct{
    u8          funydx;   // shifted by NOTIFY_1STBYTE_MINSMALL
    u8          devydx;
    char        pubval[VARIABLE_SIZE]; // deduce actual size from YSTREAM_head
}Notification_small;

#ifdef _MSC_VER
#pragma warning( pop )
#endif

typedef struct{
    char        name[YOCTO_LOGICAL_LEN];
    u8          beacon;
}Notification_name;

typedef char    Notification_product[YOCTO_PRODUCTNAME_LEN];

typedef struct {
    char        childserial[YOCTO_SERIAL_LEN];
    u8          onoff;
    u8          devydx;
}Notification_child;

typedef struct {
    char        firmware[YOCTO_FIRMWARE_LEN];
    SAFE_U16    vendorid;
    SAFE_U16    deviceid;
}Notification_firmware;

typedef struct {
    char        funcid[YOCTO_FUNCTION_LEN];
    char        funcname[YOCTO_LOGICAL_LEN];
}Notification_funcname;

typedef struct {
    char        funcid[YOCTO_FUNCTION_LEN];
    char        pubval[YOCTO_PUBVAL_SIZE];
}Notification_funcval;

typedef struct {
    char        funcid[YOCTO_FUNCTION_LEN];
    char        funcname[YOCTO_LOGICAL_LEN];
    u8          funydx;
}Notification_funcnameydx;

typedef union {
    u8                  firstByte;
    Notification_tiny   tinypubvalnot;
    Notification_small  smallpubvalnot;
    struct {
        Notification_header head;
        union {
            Notification_name           namenot;
            Notification_product        productname;
            Notification_child          childserial;
            Notification_firmware       firmwarenot;
            Notification_funcname       funcnamenot;
            Notification_funcval        pubvalnot;
            Notification_funcnameydx    funcnameydxnot;
            u8                          raw;
        };
    };
} USB_Notify_Pkt;

#define NOTIFY_NETPKT_NAME        '0'
#define NOTIFY_NETPKT_PRODNAME    '1'
#define NOTIFY_NETPKT_CHILD       '2'
#define NOTIFY_NETPKT_FIRMWARE    '3'
#define NOTIFY_NETPKT_FUNCNAME    '4'
#define NOTIFY_NETPKT_FUNCVAL     '5'
#define NOTIFY_NETPKT_STREAMREADY '6'
#define NOTIFY_NETPKT_LOG         '7'
#define NOTIFY_NETPKT_FUNCNAMEYDX '8'
#define NOTIFY_NETPKT_FUNCVALYDX  'y'
#define NOTIFY_NETPKT_NOT_SYNC    '@'

#define NOTIFY_NETPKT_VERSION   "01"
#define NOTIFY_NETPKT_START     "YN01"
#define NOTIFY_NETPKT_START_LEN 4
#define NOTIFY_NETPKT_STOP      '\n'
#define NOTIFY_NETPKT_SEP       ','
#define NOTIFY_NETPKT_MAX_LEN   (NOTIFY_NETPKT_START_LEN+1+YOCTO_SERIAL_LEN+1+YOCTO_FUNCTION_LEN+1+YOCTO_LOGICAL_LEN+1+1)

#define NOTIFY_PKT_NAME_LEN             (sizeof(Notification_header) + sizeof(Notification_name))
#define NOTIFY_PKT_PRODNAME_LEN         (sizeof(Notification_header) + sizeof(Notification_product))
#define NOTIFY_PKT_CHILD_LEN            (sizeof(Notification_header) + sizeof(Notification_child))
#define NOTIFY_PKT_FIRMWARE_LEN         (sizeof(Notification_header) + sizeof(Notification_firmware))
#define NOTIFY_PKT_STREAMREADY_LEN      (sizeof(Notification_header) + sizeof(u8))
#define NOTIFY_PKT_LOG_LEN              (sizeof(Notification_header) + sizeof(u8))
#define NOTIFY_PKT_FUNCNAME_LEN         (sizeof(Notification_header) + sizeof(Notification_funcname))
#define NOTIFY_PKT_FUNCVAL_LEN          (sizeof(Notification_header) + sizeof(Notification_funcval))
#define NOTIFY_PKT_FUNCNAMEYDX_LEN      (sizeof(Notification_header) + sizeof(Notification_funcnameydx))
#define NOTIFY_PKT_TINYVAL_MAXLEN       (sizeof(Notification_tiny) + YOCTO_PUBVAL_SIZE)

// DEFINITION OF PUBLIC FLASH STORAGE
#define USERFLASH_WORDS 11
typedef u16 UserFlash[USERFLASH_WORDS];
typedef UserFlash *UserFlashRef;

// DEFINITION OF PROGAMING PACKET AND COMMAND SENT OVER USB

#define PROG_NOP         0 // nothing to do
#define PROG_REBOOT      1 // reset the device
#define PROG_ERASE       2 // erase completely the device
#define PROG_PROG        3 // program the device
#define PROG_VERIF       4 // program the device
#define PROG_INFO        5 // get device info
#define PROG_INFO_EXT    6 // get extended device info (flash bootloader only)

#define MAX_BYTE_IN_PACKET          (60)
#define MAX_INSTR_IN_PACKET         (MAX_BYTE_IN_PACKET/3)

#define ERASE_BLOCK_SIZE_INSTR      512               // the minimal erase size in nb instr
#define PROGRAM_BLOCK_SIZE_INSTR    64                // the minimal program size in nb instr
//defins somme address in bytes too
#define ERASE_BLOCK_SIZE_BADDR      (ERASE_BLOCK_SIZE_INSTR*2)
#define PROGRAM_BLOCK_SIZE_BADDR    (PROGRAM_BLOCK_SIZE_INSTR*2)


typedef union {
    u8  raw[64];
    u16 words[32];
    struct {
        u8  size : 5;
        u8  type : 3;
        u8  addres_high;
        u16 adress_low;
        u8  data[MAX_BYTE_IN_PACKET];
    } pkt;
    struct {
        u8   size : 5;
        u8   type : 3;
        u8   pad;
        u16  pr_blk_size;
        u16  devidl;
        u16  devidh;
        u32  settings_addr;
        u32  last_addr;
        u32  config_start;
        u32  config_stop;
        u16  er_blk_size;
    } pktinfo;
    struct {
        u8   size : 5;
        u8   type : 3;
        u8   dwordpos_lo;
        u16  pageno : 14;
        u16  dwordpos_hi : 2;
        union {
        u16  npages;    // for PROG_ERASE
        u16  btsign;    // for PROG_REBOOT
        u8   data[MAX_BYTE_IN_PACKET]; // for PROG_PROG
        } opt;
    } pkt_ext;
    struct {
        u8   size : 5;
        u8   type : 3;
        u8   version;
        u16  pr_blk_size;
        u16  devidl;
        u16  devidh;
        u32  settings_addr;
        u32  last_addr;
        u32  config_start;
        u32  config_stop;
        u16  er_blk_size;
        u16  ext_jedec_id;
        u16  ext_page_size;
        u16  ext_total_pages;
        u16  first_code_page;
        u16  first_yfs3_page;
    } pktinfo_ext;
} USB_Prog_Packet;

#define START_APPLICATION_SIGN   0
#define START_BOOTLOADER_SIGN   ('b'| ('T'<<8))
#define START_AUTOFLASHER_SIGN  ('b'| ('F'<<8))

    
typedef union {
    u8              data[USB_PKT_SIZE];
    u16             data16[USB_PKT_SIZE/2];
    u32             data32[USB_PKT_SIZE/4];
    YSTREAM_Head    first_stream;
    USB_Prog_Packet prog;
    struct{
        YSTREAM_Head    head;
        USB_Conf_Pkt    conf;
    } confpkt;
} USB_Packet;

#ifndef C30
#pragma pack(pop)
#endif

//device indentifications PIC24FJ256DA210 family
#define FAMILY_PIC24FJ256DA210 0X41
#define PIC24FJ128DA206     0x08
#define PIC24FJ128DA106     0x09
#define PIC24FJ128DA210     0x0A
#define PIC24FJ128DA110     0x0B
#define PIC24FJ256DA206     0x0C
#define PIC24FJ256DA106     0x0D
#define PIC24FJ256DA210     0x0E
#define PIC24FJ256DA110     0x0F

//device indentifications PIC24FJ64GB004 family
#define FAMILY_PIC24FJ64GB004 0x42
#define PIC24FJ32GB002      0x03
#define PIC24FJ64GB002      0x07
#define PIC24FJ32GB004      0x0B
#define PIC24FJ64GB004      0x0F


#define YESC                (27u)

#ifdef  __cplusplus
}
#endif

#endif
