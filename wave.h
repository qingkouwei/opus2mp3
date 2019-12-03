#ifndef __WAVE_H__
#define __WAVE_H__
#include <stdint.h>
struct WAVE_HEADER{
    //RIFF
    char chunkId[4];
    //long int 4字节 某个大小
    uint32_t chunkSize;
    //WAVE
    char format[4];
};
struct WAVE_FMT{
    char subchunk1Id[4];
    uint32_t subchunk1Size;
    uint16_t audioFormatTag;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitPerSample;
};

struct WAVE_DATA{
    char subchunk2Id[4];
    uint32_t subchunk2Size;
};
typedef struct WAVE {
   struct WAVE_HEADER riff;
   struct WAVE_FMT fmt;
   struct WAVE_DATA data;
} Wav;
#endif