#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#include "wave.h"

void main(int argc, char * argv[]) {

    //Check for valid number of arguments or display help
    if(argc < 8) {
        printf("Usage:\n./Tone -l [length] -s [frequency] [amplitude] -o [output-file] -r [sample-rate]\n");
        printf("-l length of tone to produce in seconds\n");    
        printf("-s Creates sine wave. Can be used multiple times. Frequency (Hz) and amplitude (0 - 32767) of each tone. \n");  
        printf("-o File to write to\n");
        printf("-r samples per second (kHz). Note: Must be double highest frequency in tone.\n");   
        return;
    }

    //Organize arguments
    int length, sinf[10], sina[10], samplerate;
    memset(sinf, 0, sizeof(int) * 10);
    memset(sina, 0, sizeof(int) * 10);
    char * output = NULL;
    int i = 0;
    int count;
    for(count = 1; count < argc; count++){
        char first = *argv[count];
        int second = *(argv[count] + 1);    
        if (first == '-') {
            switch (second) {
                case 's':
                    sinf[i] = atoi(argv[count+1]);
                    sina[i] = atoi(argv[count+2]);
                    printf("sinf = %d, sina = %d\n", sinf[i], sina[i]);
                    i++;
                    break;
                case 'l':
                    length = atoi(argv[count+1]);
                    break;
                case 'o':
                    output = argv[count+1];
                    break;
                case 'r':
                    samplerate = atoi(argv[count+1]) * 1000;
                    break;
            }
        }
    }

    printf("parse param success\n");
    //Allocate memory for wav file
    size_t size = sizeof(struct WAVE_HEADER) + sizeof(struct WAVE_FMT) + sizeof(struct WAVE_DATA) + (length * samplerate * 2);
    printf("wave head size = %d\n", size);
    void * buffer = malloc(size);
    memset(buffer,0,size);

    printf("malloc buffer success\n");
    //Fill buffer with headers
    struct WAVE_HEADER * WAV = (struct WAVE_HEADER *)buffer;
    struct WAVE_FMT * Format = (struct WAVE_FMT *)(WAV + 1);
    struct WAVE_DATA * Data = (struct WAVE_DATA *)(Format + 1);
    printf("wav = %p, format = %p, data = %p, chunkId = %p\n", WAV, Format, Data, WAV->chunkId);

    memcpy(WAV->chunkId, "RIFF", strlen("RIFF"));
    WAV->chunkSize = (uint32_t)size - 8;
    memcpy(WAV->format, "WAVE", sizeof("WAVE"));
    

    memcpy(Format->subchunk1Id, "fmt ", sizeof("fmt "));
    Format->subchunk1Size = 16;
    Format->audioFormatTag = 1;
    Format->numChannels = 1;
    Format->sampleRate = (uint32_t)samplerate;
    Format->byteRate = 16/8* samplerate;
    Format->blockAlign = 2;
    Format->bitPerSample = Format->blockAlign * 8;

    memcpy(Data->subchunk2Id, "data", sizeof("data"));
    Data->subchunk2Size = length * samplerate * 2;

    //Generate Sound
    printf("Generating sound...\n");
    short * sound = (short *)(Data + 1);
    short total;
    float time;
    float increment = 1.0/(float)samplerate;
    float TWOPI = 6.28318531f;
    unsigned int sample_count = length * samplerate;
    for (unsigned int i = 0; i < sample_count; i++)
    {
        unsigned int j = i % samplerate; // normalize the sample position so that we don't blow up in the subsequent multiplication
        float f = 0.0f;
        int result;

        for (int x = 0; x < 10; x++)
        {
            f += sina[x] * sin((sinf[x] * j * TWOPI) / samplerate);
        }

        result = (long)f;

        //clamp to 16-bit
        if (result > 32767)
        {
            result = 32767;
        }
        else if (result < -32768)
        {
            result = -32768;
        }

        sound[i] = (short)result;

        //printf("%d\n", sound[i]);

    }

    //Write buffer to file
    FILE * out = fopen(output, "w");
    fwrite(buffer, size, 1, out);
    printf("Wrote to %s\n", output);

    return;

}