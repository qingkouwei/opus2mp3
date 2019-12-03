 #include <stdio.h>
 #include <stdint.h>
 #include <stdlib.h>
 #include "wave.h"
 int main(int argc, char **argv)
 {
    FILE *fp = NULL;
    
     Wav wav;
     struct WAVE_HEADER riff;
     struct WAVE_FMT fmt;
     struct WAVE_DATA data;
 
     fp = fopen("test.wav", "rb");
     if (!fp) {
         printf("can't open audio file\n");
         exit(1);
     }
 
     fread(&wav, 1, sizeof(wav), fp);
     
     riff = wav.riff;
     fmt = wav.fmt;
     data = wav.data;
 
     printf("ChunkID \t%c%c%c%c\n", riff.chunkId[0], riff.chunkId[1], riff.chunkId[2], riff.chunkId[3]);
     printf("ChunkSize \t%d\n", riff.chunkSize);
     printf("Format \t\t%c%c%c%c\n", riff.format[0], riff.format[1], riff.format[2], riff.format[3]);
     
     printf("\n");
     
     printf("Subchunk1ID \t%c%c%c%c\n", fmt.subchunk1Id[0], fmt.subchunk1Id[1], fmt.subchunk1Id[2], fmt.subchunk1Id[3]);
     printf("Subchunk1Size \t%d\n", fmt.subchunk1Size);
     printf("AudioFormat \t%d\n", fmt.audioFormatTag);
     printf("NumChannels \t%d\n", fmt.numChannels);
     printf("SampleRate \t%d\n", fmt.sampleRate);
     printf("ByteRate \t%d\n", fmt.byteRate);
     printf("BlockAlign \t%d\n", fmt.blockAlign);
     printf("BitsPerSample \t%d\n", fmt.bitPerSample);
     
     printf("\n");
 
     printf("blockID \t%c%c%c%c\n", data.subchunk2Id[0], data.subchunk2Id[1], data.subchunk2Id[2], data.subchunk2Id[3]);
     printf("blockSize \t%d\n", data.subchunk2Size);
     
     printf("\n");
    
     printf("duration \t%d\n", data.subchunk2Size / fmt.byteRate);
 }