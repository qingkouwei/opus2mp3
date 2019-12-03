#include "wavwriter.h"
#include <stdio.h>

long getFileSize(char* filename){
    FILE *fp = fopen(filename, "r");
    if(!fp) return -1;
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return size;
}
int converToWav(const char *pcmpath, int channels, int sample_rate, const char *wavepath){
    int bits = 16;
    struct WAVE_HEADER   pcmHEADER;
    struct WAVE_FMT   pcmFMT;
    struct WAVE_DATA   pcmDATA;
    FILE *fp, *fpout;

    fp = fopen(pcmpath, "rb");
    if (NULL == fp) {
        printf("file open failed\n");
        return -1;
    }
    long pcmSize = getFileSize((char*)pcmpath);
    //WAVE_HEADER
    memcpy(pcmHEADER.chunkId, "RIFF", strlen("RIFF"));
    pcmHEADER.chunkSize = 36 + pcmSize;
    memcpy(pcmHEADER.format, "WAVE", strlen("WAVE"));


    fpout = fopen(wavepath, "wb");
    if (NULL == fpout) {
        printf("file open failed\n");
        return -1;
    }
    printf("pcmHeader size = %d\n", sizeof(pcmHEADER));
    fwrite(&pcmHEADER, sizeof(pcmHEADER), 1, fpout);
    //WAVE_FMT

    pcmFMT.numChannels = channels;
    pcmFMT.sampleRate = sample_rate;
    pcmFMT.bitPerSample = bits;
    pcmFMT.byteRate = sample_rate*channels *pcmFMT.bitPerSample / 8;
    memcpy(pcmFMT.subchunk1Id, "fmt ", strlen("fmt "));
    pcmFMT.subchunk1Size = 16;
    pcmFMT.audioFormatTag = 1;
    pcmFMT.blockAlign = channels*pcmFMT.bitPerSample / 8;
    printf("pcmHepcmFMTader size = %d\n", sizeof(pcmFMT));
    fwrite(&pcmFMT, sizeof(pcmFMT), 1, fpout);

    //WAVE_DATA
    memcpy(pcmDATA.subchunk2Id, "data", strlen("data"));
    pcmDATA.subchunk2Size = pcmSize;
    printf("pcmDATA size = %d\n", sizeof(pcmDATA));
    fwrite(&pcmDATA, sizeof(pcmDATA), 1, fpout);
    char *buff=(char*)malloc(512);
    int len;
    while ((len = fread(buff, sizeof(char), 512, fp)) != 0)
    {
        fwrite(buff, sizeof(char), len, fpout);
    }
    free(buff);
    fclose(fp);
    fclose(fpout);
}