/* ==========================================
    Mock: SEGGER_RTT.h
    Description:
    Host-side stub for the SEGGER RTT debug output
    library. Redirects to stdout on the host PC.
 ========================================== */

#ifndef SEGGER_RTT_H
#define SEGGER_RTT_H

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

/* ==========================================
 * RTT configuration (defaults)
 * ========================================== */
#ifndef SEGGER_RTT_MAX_NUM_UP_BUFFERS
#define SEGGER_RTT_MAX_NUM_UP_BUFFERS   2
#endif
#ifndef SEGGER_RTT_MAX_NUM_DOWN_BUFFERS
#define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS 2
#endif
#ifndef BUFFER_SIZE_UP
#define BUFFER_SIZE_UP 1024
#endif

/* ==========================================
 * RTT buffer descriptors
 * ========================================== */
typedef struct {
    const char *sName;
    char *pBuffer;
    unsigned SizeOfBuffer;
    unsigned WrOff;
    volatile unsigned RdOff;
    unsigned Flags;
} SEGGER_RTT_BUFFER_UP;

typedef struct {
    const char *sName;
    char *pBuffer;
    unsigned SizeOfBuffer;
    volatile unsigned WrOff;
    unsigned RdOff;
    unsigned Flags;
} SEGGER_RTT_BUFFER_DOWN;

typedef struct {
    char acID[16];
    int MaxNumUpBuffers;
    int MaxNumDownBuffers;
    SEGGER_RTT_BUFFER_UP  aUp[SEGGER_RTT_MAX_NUM_UP_BUFFERS];
    SEGGER_RTT_BUFFER_DOWN aDown[SEGGER_RTT_MAX_NUM_DOWN_BUFFERS];
} SEGGER_RTT_CB;

/* ==========================================
 * RTT API stubs
 * ========================================== */
int          SEGGER_RTT_AllocDownBuffer(const char *sName, void *pBuffer,
                                        unsigned BufferSize, unsigned Flags);
int          SEGGER_RTT_AllocUpBuffer(const char *sName, void *pBuffer,
                                       unsigned BufferSize, unsigned Flags);
int          SEGGER_RTT_ConfigUpBuffer(unsigned BufferIndex, const char *sName,
                                        void *pBuffer, unsigned BufferSize,
                                        unsigned Flags);
int          SEGGER_RTT_ConfigDownBuffer(unsigned BufferIndex, const char *sName,
                                          void *pBuffer, unsigned BufferSize,
                                          unsigned Flags);
int          SEGGER_RTT_GetKey(void);
unsigned     SEGGER_RTT_HasData(unsigned BufferIndex);
int          SEGGER_RTT_HasKey(void);
unsigned     SEGGER_RTT_HasDataUp(unsigned BufferIndex);
void         SEGGER_RTT_Init(void);
unsigned     SEGGER_RTT_Read(unsigned BufferIndex, void *pBuffer,
                              unsigned BufferSize);
unsigned     SEGGER_RTT_ReadNoLock(unsigned BufferIndex, void *pData,
                                    unsigned BufferSize);
int          SEGGER_RTT_SetNameDownBuffer(unsigned BufferIndex,
                                           const char *sName);
int          SEGGER_RTT_SetNameUpBuffer(unsigned BufferIndex, const char *sName);
int          SEGGER_RTT_SetFlagsDownBuffer(unsigned BufferIndex, unsigned Flags);
int          SEGGER_RTT_SetFlagsUpBuffer(unsigned BufferIndex, unsigned Flags);
int          SEGGER_RTT_WaitKey(void);
unsigned     SEGGER_RTT_Write(unsigned BufferIndex, const void *pBuffer,
                               unsigned NumBytes);
unsigned     SEGGER_RTT_WriteNoLock(unsigned BufferIndex, const void *pBuffer,
                                     unsigned NumBytes);
unsigned     SEGGER_RTT_WriteSkipNoLock(unsigned BufferIndex, const void *pBuffer,
                                         unsigned NumBytes);
unsigned     SEGGER_RTT_ASM_WriteSkipNoLock(unsigned BufferIndex,
                                             const void *pBuffer,
                                             unsigned NumBytes);
unsigned     SEGGER_RTT_WriteString(unsigned BufferIndex, const char *s);
void         SEGGER_RTT_WriteWithOverwriteNoLock(unsigned BufferIndex,
                                                   const void *pBuffer,
                                                   unsigned NumBytes);
unsigned     SEGGER_RTT_PutChar(unsigned BufferIndex, char c);
unsigned     SEGGER_RTT_PutCharSkip(unsigned BufferIndex, char c);
unsigned     SEGGER_RTT_PutCharSkipNoLock(unsigned BufferIndex, char c);
unsigned     SEGGER_RTT_GetAvailWriteSpace(unsigned BufferIndex);
unsigned     SEGGER_RTT_GetBytesInBuffer(unsigned BufferIndex);
unsigned     SEGGER_RTT_ReadUpBuffer(unsigned BufferIndex, void *pBuffer,
                                       unsigned BufferSize);
unsigned     SEGGER_RTT_ReadUpBufferNoLock(unsigned BufferIndex, void *pData,
                                            unsigned BufferSize);
unsigned     SEGGER_RTT_WriteDownBuffer(unsigned BufferIndex,
                                         const void *pBuffer,
                                         unsigned NumBytes);
unsigned     SEGGER_RTT_WriteDownBufferNoLock(unsigned BufferIndex,
                                               const void *pBuffer,
                                               unsigned NumBytes);

int          SEGGER_RTT_SetTerminal(unsigned char TerminalId);
int          SEGGER_RTT_TerminalOut(unsigned char TerminalId, const char *s);

int SEGGER_RTT_printf(unsigned BufferIndex, const char *sFormat, ...);
int SEGGER_RTT_vprintf(unsigned BufferIndex, const char *sFormat,
                        va_list *pParamList);

/* ==========================================
 * RTT macros - no-op on host
 * ========================================== */
#define RTT_CTRL_RESET        ""
#define RTT_CTRL_CLEAR        ""

/* Host-side printf replacement for RTT */
#ifdef __cplusplus
extern "C" {
#endif

/* Redefine RTT macros to stdout equivalents on host */
#define SEGGER_RTT_HASDATA(n)           0
#define SEGGER_RTT_HASDATA_UP(n)         0

#ifdef __cplusplus
}
#endif

#endif /* SEGGER_RTT_H */
