#ifndef __USER_JSCOPE_TRANSMIT_H__
#define __USER_JSCOPE_TRANSMIT_H__

#include <stdint.h>


#pragma pack(push, 1)
typedef struct {
    uint32_t timestamp;
    float val_1;
    float val_2;
    float val_3;
} JScope_Transmit_t;
#pragma pack(pop)

#endif //__USER_JSCOPE_TRANSMIT_H__