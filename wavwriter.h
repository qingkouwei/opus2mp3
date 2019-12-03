#ifndef __WAVWRITER_H__
#define __WAVWRITER_H__
#include "wave.h"
int converToWav(const char *pcmpath, int channels, int sample_rate, const char *wavepath);
#endif