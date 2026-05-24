/* ==========================================
    Mock: SEGGER_RTT_Conf.h
    Host-side configuration stub for SEGGER RTT.
 ========================================== */
#ifndef SEGGER_RTT_CONF_H
#define SEGGER_RTT_CONF_H

#include <stdint.h>

/* RTT configuration defaults */
#ifndef SEGGER_RTT_MAX_NUM_UP_BUFFERS
#define SEGGER_RTT_MAX_NUM_UP_BUFFERS   2
#endif
#ifndef SEGGER_RTT_MAX_NUM_DOWN_BUFFERS
#define SEGGER_RTT_MAX_NUM_DOWN_BUFFERS 2
#endif

#endif /* SEGGER_RTT_CONF_H */
