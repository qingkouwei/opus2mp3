#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opus.h"
#include <sys/time.h>
#include <unistd.h>
#include "lame.h"
#include "wavwriter.h"

#define DEFAULT_SAMPLERATEINHz 16000
#define DEFAULT_CHANNELCONFIG 1

#define MAX_PACKET 1500

#define WRITE_BUFFER_SIZE 520
#define READ_BUFFER_SIZE 40
#define DECODE_OUTPUT_SIZE 320

/**判断str1是否以str2开头
 * 如果是返回1
 * 不是返回0
 * 出错返回-1
 * */
int is_begin_with(const char * str1,char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) ||  (len1 == 0 || len2 == 0))
        return -1;
    char *p = str2;
    int i = 0;
    while(*p != '\0')
    {
        if(*p != str1[i])
            return 0;
        p++;
        i++;
    }
    return 1;
}

/**判断str1是否以str2结尾
 * 如果是返回1
 * 不是返回0
 * 出错返回-1
 * */
int is_end_with(const char *str1, char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) ||  (len1 == 0 || len2 == 0))
        return -1;
    while(len2 >= 1)
    {
        if(str2[len2 - 1] != str1[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}

int main(int argc, char **argv)
{
    //统计耗时
    struct timeval tv;
    gettimeofday(&tv,NULL);
    long start = tv.tv_sec*1000000 + tv.tv_usec;
    FILE *fin;
    FILE *fout;
    short *in = NULL;
    short *out = NULL;

    unsigned char *fbytes = NULL;
    int max_frame_size = DEFAULT_SAMPLERATEINHz*2;

    int isWav = 0;

    int error;
    size_t num_read;
    if (argc < 3)
    {
        fprintf(stderr, "usage: %s <raw opus input> <mp3 output>\n", argv[0]);
        return 1;
    }
    fin = fopen(argv[1], "rb");
    if (!fin)
    {
        fprintf(stderr, "cannot open input file: %s\n", argv[1]);
        return 1;
    }
    fout = fopen(argv[2], "wb");
    if (!fout)
    {
        fprintf(stderr, "cannot open output file: %s\n", argv[2]);
        return 1;
    }

    isWav = is_end_with(argv[2], "wav");

    int sampleRateInHz = DEFAULT_SAMPLERATEINHz;
    int channelConfig = DEFAULT_CHANNELCONFIG;

    OpusDecoder *pOpusDec = opus_decoder_create(sampleRateInHz, channelConfig, &error);
    if (error != OPUS_OK)
    {
        fprintf(stderr, "Cannot create decoder: %s\n", opus_strerror(error));
        goto failure;
    }
    
    fbytes = (unsigned char*)malloc(WRITE_BUFFER_SIZE);
    out = (short *)malloc(640 * sizeof(short));
    lame_global_flags *lame = NULL;
    if(isWav >= 0){
        lame = lame_init();
        lame_set_in_samplerate(lame, sampleRateInHz);
        lame_set_num_channels(lame, channelConfig);//输入流的声道
        lame_set_out_samplerate(lame, sampleRateInHz);
        lame_set_brate(lame, 16);
        lame_set_quality(lame, 7);
        lame_init_params(lame);
    }
    
    int i = 0;
    int flag = 0;
    while (1)
    {
    gettimeofday(&tv,NULL);
    long startDec = tv.tv_sec*1000000 + tv.tv_usec;
    unsigned char data[READ_BUFFER_SIZE];
    num_read = fread(data, 1, READ_BUFFER_SIZE, fin);
        
    if (num_read > 0)
    {
        int output_samples = opus_decode(pOpusDec, data, num_read, out, DECODE_OUTPUT_SIZE, 0);
        if(isWav > 0){
            if (fwrite(out, 2, output_samples, fout) != (unsigned)(output_samples)){
                printf("write wav error,output_samples = %d\n",output_samples);
                goto failure;
            }
        }else{
            int result = lame_encode_buffer(lame, out, out,output_samples, fbytes, WRITE_BUFFER_SIZE);
            if (fwrite(fbytes, 1, result, fout) != (unsigned)(result)){
                printf("write error,output_samples = %d, result = %d\n",output_samples,result);
                goto failure;
            }
        }
    }
    else{
        if(isWav > 0){
            if (fout)
                fclose(fout);
            //char *outputpath = malloc(strlen(argv[2]))
            converToWav(argv[2], 1, 16000, "output.wav");
            if(rename("output.wav", argv[2]) != 0){
                printf("重命名失败\n");
            }
        }else{
            int result = lame_encode_flush(lame, fbytes, WRITE_BUFFER_SIZE);
            printf("result:%d\n",result);
            if(result >= 0){
                if (fwrite(fbytes, 1, result, fout) != (unsigned)(result)){
                    fprintf(stderr, "222 Error writing.\n");
                 goto failure;
                }
            }
            lame_close(lame);
        }
        
        break;
    }
    gettimeofday(&tv,NULL);
    long endDec = tv.tv_sec*1000000 + tv.tv_usec;   
    //printf("decode %d speend:%lld\n", i++, endDec - startDec);    
    }
    fclose(fout);
failure:
    opus_decoder_destroy(pOpusDec);
    if (fin)
        fclose(fin);
    if (fout)
        fclose(fout);
    free(in);
    free(out);
    free(fbytes);
    gettimeofday(&tv,NULL);
    long end = tv.tv_sec*1000000 + tv.tv_usec;
    printf("spend  = %ld\n", end - start);
return 0;
}