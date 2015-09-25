#ifndef TYPES_DEF_H_
#define TYPES_DEF_H_


#define IN
#define OUT
#define INOUT

#ifndef PACKED 
#define PACKED  __packed//__attribute__ ((packed))
#endif //PACKED

#ifndef NULL
#define NULL                0
#endif

#ifndef INLINE
#ifdef __GNUC__
/** inline directive */
#define    INLINE    inline
#else
/** inline directive */
#define    INLINE    __inline
#endif
#endif

typedef signed char         INT8;
typedef unsigned char       UINT8;
typedef signed short int    INT16;
typedef unsigned short int  UINT16;
typedef signed int          INT32;
typedef unsigned int        UINT32;
typedef volatile unsigned long    VUINT32;
typedef volatile unsigned long  const VUC32;  /* Read Only */
typedef volatile UINT16         VUINT16;
typedef volatile UINT16  const VUC16;  /* Read Only */

typedef unsigned char *       PUINT8;
typedef unsigned short int *    PUINT16;
typedef unsigned int *       PUINT32;

#ifndef BOOL_T
typedef unsigned char       BOOL_T;
#endif

#ifndef VOID
#define     VOID              void  
//typedef void                VOID;
typedef void *                PVOID;
#endif

typedef char *                PSTRING;
typedef char    STRING;

#define NST_FALSE                              0
#define NST_TRUE                               1

typedef __int64        UINT64;

typedef union _LARGE_INTEGER
{
    struct 
    {
        UINT32 LowPart;
        UINT32 HighPart;
    } u;
    __int64 QuadPart;
} LARGE_INTEGER;


#endif /*TYPES_DEF_H_*/
