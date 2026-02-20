/* 包含头文件 ----------------------------------------------------------------*/
#include "../user_ring_buffe.h"

/* 私有函数声明 --------------------------------------------------------------*/
static void RingBuffer_AddReadIndex(RING_BUFFER *buffer, uint16_t length);
static uint8_t RingBuffer_Read(const RING_BUFFER *buffer, uint16_t index);
static uint16_t RingBuffer_GetRemain(const RING_BUFFER *buffer);

/* 私有函数 ------------------------------------------------------------------*/

/**
* @brief 增加读索引
* @param buffer 环形缓冲区指针
* @param length 要增加的长度
*/
static void RingBuffer_AddReadIndex(RING_BUFFER *buffer, const uint16_t length) {
    buffer->read_index = (length + buffer->read_index) % RING_BUFFER_SIZE;
}

/**
* @brief 读取第 i 位数据（超过缓冲区长度自动循环）
* @param buffer 环形缓冲区指针
* @param index  要读取的数据索引
* @return 读取的数据
*/
static uint8_t RingBuffer_Read(const RING_BUFFER *buffer, const uint16_t index) {
    return buffer->buffer[index % RING_BUFFER_SIZE];
}

/**
* @brief 计算缓冲区剩余空间
* @param buffer 环形缓冲区指针
* @return 剩余空间大小
*/
static uint16_t RingBuffer_GetRemain(const RING_BUFFER *buffer) {
    return RING_BUFFER_SIZE - RingBuffer_GetLength(buffer);
}

/* 函数体 --------------------------------------------------------------------*/

/**
* @brief 计算未处理的数据长度
* @param buffer 环形缓冲区指针
* @return 未处理的数据长度
*/
uint16_t RingBuffer_GetLength(const RING_BUFFER *buffer) {
    return (buffer->write_index + RING_BUFFER_SIZE - buffer->read_index) % RING_BUFFER_SIZE;
}


/**
* @brief 向缓冲区写入数据
* @param buffer 环形缓冲区指针
* @param data   要写入的数据指针
* @param length 要写入的数据长度
* @return 写入的数据长度
* @retval 0 缓冲区空间不足，未写入
* @retval length 成功写入的数据长度
*/
uint16_t RingBuffer_Put(RING_BUFFER *buffer, const uint8_t *data, const uint16_t length) {
    // 如果缓冲区空间不足，则不写入数据
    if (RingBuffer_GetRemain(buffer) < length)
        return 0;
    
    // 将数据写入缓冲区
    if (buffer->write_index + length < RING_BUFFER_SIZE) {
        memcpy(buffer->buffer + buffer->write_index, data, length);
        buffer->write_index += length;
    } else {
        const uint16_t first_len = RING_BUFFER_SIZE - buffer->write_index;
        
        memcpy(buffer->buffer + buffer->write_index, data, first_len);
        memcpy(buffer->buffer, data + first_len, length - first_len);
        buffer->write_index = length - first_len;
    }
    return length;
}

/**
* @brief 通过帧头和帧尾获取数据帧
* @param buffer  环形缓冲区指针
* @param data    存储获取数据帧的缓冲区
* @param head    帧头字符串
* @param tail    帧尾字符串
* @return 获取的数据帧长度
* @retval 0 没有获取到数据帧
*/
uint16_t RingBuffer_GetWithHT(RING_BUFFER *buffer, uint8_t *data, const char *head, const char *tail) {
    const uint16_t buff_len = RingBuffer_GetLength(buffer);
    const uint16_t head_len = (uint16_t)strlen(head);
    const uint16_t tail_len = (uint16_t)strlen(tail);
    
    if (buff_len >= head_len + tail_len) {
        for (int32_t frame_start_index = buff_len - head_len - tail_len; frame_start_index >= 0; frame_start_index--) {
            uint16_t read_index = buffer->read_index + frame_start_index;

            // 匹配帧头
            uint8_t head_match = 1;
            for (uint16_t i = 0; i < head_len; i++)
                head_match &= RingBuffer_Read(buffer, read_index + i) == head[i];
            if (!head_match)
                continue;

            for (int32_t tail_index = head_len; tail_index <= buff_len - frame_start_index - tail_len; tail_index++) {

                // 匹配帧尾
                uint8_t tail_match = 1;
                for (uint16_t i = 0; i < tail_len; i++) {
                    tail_match &= RingBuffer_Read(buffer, read_index + tail_index + i) == tail[i];
                }
                if (!tail_match)
                    continue;

                const uint16_t frame_len = tail_index + tail_len;

                if (read_index >= RING_BUFFER_SIZE)
                    read_index = read_index - RING_BUFFER_SIZE;

                if (read_index + frame_len <= RING_BUFFER_SIZE)
                    memcpy(data, buffer->buffer + read_index, frame_len);
                else {
                    const uint16_t first_len = RING_BUFFER_SIZE - read_index;

                    memcpy(data, buffer->buffer + read_index, first_len);
                    memcpy(data + first_len, buffer->buffer, frame_len - first_len);
                }

                RingBuffer_AddReadIndex(buffer, frame_start_index + frame_len);
                return frame_len;
            }
        }
    }
    return 0;
}

/**
* @brief 通过数据帧的长度和帧头获取数据帧
* @param buffer  环形缓冲区指针
* @param data 存储获取数据的缓冲区
* @param head    帧头字符串
* @param len     数据帧长度
* @return 获取的数据帧长度
* @retval 0 没有获取到数据帧
* @retval len 成功获取到数据帧
*/
uint16_t RingBuffer_GetWithHLen(RING_BUFFER *buffer, uint8_t *data, const char *head, const uint16_t len) {
    const uint16_t buff_len = RingBuffer_GetLength(buffer);
    const uint16_t head_len = (uint16_t)strlen(head);
    
    if (buff_len >= len) {
        for (int32_t frame_start_index = buff_len - len; frame_start_index >= 0; frame_start_index--) {
            uint16_t read_index = buffer->read_index + frame_start_index;

            // 匹配帧头
            uint8_t head_match = 1;
            for (uint16_t i = 0; i < head_len; i++)
                head_match &= RingBuffer_Read(buffer, read_index + i) == head[i];
            if (!head_match)
                continue;

            if (read_index >= RING_BUFFER_SIZE)
                read_index = read_index - RING_BUFFER_SIZE;

            if (read_index + len <= RING_BUFFER_SIZE)
                memcpy(data, buffer->buffer + read_index, len);
            else {
                const uint16_t first_len = RING_BUFFER_SIZE - read_index;

                memcpy(data, buffer->buffer + read_index, first_len);
                memcpy(data + first_len, buffer->buffer, len - first_len);
            }

            RingBuffer_AddReadIndex(buffer, frame_start_index + len);
            return len;
        }
    }
    return 0;
}


/**
* @brief 通过数据帧的长度获取数据帧
* @param buffer  环形缓冲区指针
* @param data 存储获取数据的缓冲区
* @param len     数据帧长度
* @return 获取的数据帧长度
* @retval 0 缓冲区数据长度不足
* @retval len 成功获取到数据帧
*/
uint16_t RingBuffer_GetWithLen(RING_BUFFER *buffer, uint8_t *data, const uint16_t len) {
    const uint16_t buff_len = RingBuffer_GetLength(buffer);
    
    if (buff_len >= len) {
        if (buffer->read_index + len <= RING_BUFFER_SIZE)
            memcpy(data, buffer->buffer + buffer->read_index, len);
        else {
            const uint16_t first_len = RING_BUFFER_SIZE - buffer->read_index;

            memcpy(data, buffer->buffer + buffer->read_index, first_len);
            memcpy(data + first_len, buffer->buffer, len - first_len);
        }

        RingBuffer_AddReadIndex(buffer, len);
        return len;
    }
    return 0;
}

/**
* @brief 获取从帧头到下一个帧头的数据帧
* @param buffer  环形缓冲区指针
* @param data 存储获取数据的缓冲区
* @param head    帧头字符串
* @return 获取的数据帧长度
* @retval 0 没有获取到数据帧
*/
uint16_t RingBuffer_GetWithH(RING_BUFFER *buffer, uint8_t *data, const char *head) {
    const uint16_t buff_len = RingBuffer_GetLength(buffer);
    const uint16_t head_len = (uint16_t)strlen(head);
    
    if (buff_len >= 2 * head_len) {
        for (int32_t frame_start_index = 0; frame_start_index <= buff_len - head_len; frame_start_index++) {
            uint16_t read_index = buffer->read_index + frame_start_index;

            // 匹配第一个帧头
            uint8_t head_match = 1;
            for (uint16_t i = 0; i < head_len; i++)
                head_match &= RingBuffer_Read(buffer, read_index + i) == head[i];
            if (!head_match)
                continue;

            for (int32_t next_head_index = head_len; next_head_index <= buff_len - head_len - frame_start_index; next_head_index++) {

                // 匹配下一个帧头
                uint8_t next_head_match = 1;
                for (uint16_t i = 0; i < head_len; i++)
                    next_head_match &= RingBuffer_Read(buffer, read_index + next_head_index + i) == head[i];
                if (!next_head_match)
                    continue;

                const uint16_t frame_len = next_head_index;

                if (read_index >= RING_BUFFER_SIZE)
                    read_index = read_index - RING_BUFFER_SIZE;

                if (read_index + frame_len <= RING_BUFFER_SIZE)
                    memcpy(data, buffer->buffer + read_index, frame_len);
                else {
                    const uint16_t first_len = RING_BUFFER_SIZE - read_index;

                    memcpy(data, buffer->buffer + read_index, first_len);
                    memcpy(data + first_len, buffer->buffer, frame_len - first_len);
                }

                RingBuffer_AddReadIndex(buffer, frame_start_index + frame_len);
                return frame_len;
            }
        }
    }
    return 0;
}