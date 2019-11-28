//opus decode encode tools
#include <stdio.h>
#include <stdlib.h>
#include "opus.h"
#include <sys/time.h>
#include <unistd.h>

#define DEFAULT_SAMPLERATEINHz 16000
#define DEFAULT_CHANNELCONFIG 1
#define DEFAULT_BITRATE 16000

#define READ_BUFFER_SIZE 640

int main(int argc, char **argv)
{
    FILE *fin;
    FILE *fout;
    short *in = NULL;
    short *out = NULL;
    if (argc != 3)
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
    int sampleRateInHz = DEFAULT_SAMPLERATEINHz;
    int channelConfig = DEFAULT_CHANNELCONFIG;
    int bitrate = DEFAULT_BITRATE;
    int error;
    size_t num_read;
    OpusEncoder *pOpusEnc = opus_encoder_create(sampleRateInHz, channelConfig,
                                                OPUS_APPLICATION_RESTRICTED_LOWDELAY,
                                                &error);
    if (pOpusEnc) {
        opus_encoder_ctl(pOpusEnc, OPUS_SET_VBR(0));//0:CBR, 1:VBR
        opus_encoder_ctl(pOpusEnc, OPUS_SET_VBR_CONSTRAINT(true));
        opus_encoder_ctl(pOpusEnc, OPUS_SET_BITRATE( bitrate* 1000));
        opus_encoder_ctl(pOpusEnc, OPUS_SET_COMPLEXITY(complexity));//8    0~10
        opus_encoder_ctl(pOpusEnc, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
        opus_encoder_ctl(pOpusEnc, OPUS_SET_LSB_DEPTH(16));
        opus_encoder_ctl(pOpusEnc, OPUS_SET_DTX(0));
        opus_encoder_ctl(pOpusEnc, OPUS_SET_INBAND_FEC(0));
        opus_encoder_ctl(pOpusEnc, OPUS_SET_PACKET_LOSS_PERC(0));
    }

    unsigned char *out = (short *)malloc(READ_BUFFER_SIZE * sizeof(char));
    while (1)
    {
        unsigned char data[READ_BUFFER_SIZE];
        num_read = fread(data, 1, READ_BUFFER_SIZE, fin);
        short sData[num_read/2];
        memcpy(data , sData, num_read );
        if (num_read > 0)
        {
            int result = opus_encode(pOpusEnc, sData, num_read/2, out,
                           READ_BUFFER_SIZE * sizeof(char));
            if (fwrite(out, 1, result, fout) != (unsigned)(result)){
                printf("write error\n",output_samples,result);
                goto failure;
            }
        }
        else{
            break;
        }
    }
    fclose(fout);
    fclose(fin);
failure:
    printf("encode end!\n");
    opus_encoder_destroy(pOpusEnc);//销毁解码器
}