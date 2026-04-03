#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "decode.h"

/* Function Definitions */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2], ".bmp") == NULL) 
    {
        
        return e_failure;
    }
    else
    {
        if(strcmp(strstr(argv[2], ".bmp"),".bmp"))
        {
            return e_failure;
        }
    }
    decInfo->src_image_fname = argv[2];

    if(argv[3]!=NULL)
    {
        int i;
        for(i=0; argv[3][i]!='\0'; i++)
        {
            if(argv[3][i] == '.') 
            {
                break;
            }
        }
        argv[3][i] = '\0';
        strcpy(decInfo->dest_fname, argv[3]);
    }
    else
    {
        strcpy(decInfo->dest_fname, "default");
    }

    return e_success;
}


Status skip_bmp_header(FILE* fptr)
{
    fseek(fptr, 54, SEEK_SET);
    if(ftell(fptr) != 54)
    {
        return e_failure;
    }
    return e_success;
}

Status decode_magic_string_size(DecodeInfo *decInfo)
{
    char imagebuff[32];
    fread(imagebuff, 1, 32, decInfo->fptr_src_image);
    decInfo->magic_size = decode_size_from_lsb(imagebuff);
    if(decInfo->magic_size == 0)
    {
        return e_failure;
    }
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char buff[8], magic_string[decInfo->magic_size + 1];
    for(int i=0; i<decInfo->magic_size; i++)
    {
        fread(buff, 1, 8, decInfo->fptr_src_image);
        magic_string[i] = decode_byte_from_lsb(buff);
    }
    char usr_input[10];
    printf("Enter your magic string: ");
    scanf("%9s",usr_input);

    if(strcmp(magic_string, usr_input)!=0)
    {
        printf("Magic string unmatched\n");
        return e_failure;
    }

    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buff[32];
    fread(buff, 1, 32, decInfo->fptr_src_image);
    decInfo->size_dest_file_extn = decode_size_from_lsb(buff);
    if(decInfo->size_dest_file == 0)
    {
        return e_failure;
    }
    printf("Size of extension : %d\n", decInfo->size_dest_file_extn);
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buff[8], file_extn[decInfo->size_dest_file_extn+1];
    for(int i=0; i<decInfo->size_dest_file_extn; i++)
    {
        fread(buff, 1, 8, decInfo->fptr_src_image);
        file_extn[i] = decode_byte_from_lsb(buff);
    }
    if(file_extn[0] != '.')
    {
        printf("Unable to decode extension\n");
        return e_failure;
    }

    int len = strlen(decInfo->dest_fname), i;
    for(i=0; i<decInfo->size_dest_file_extn; i++)
    {
        decInfo->dest_fname[len+i] = file_extn[i];
    }
    decInfo->dest_fname[len+i] = '\0';
    //open output file
    decInfo->fptr_dest_file = fopen(decInfo->dest_fname, "w");
    //Error handling
    if(decInfo->fptr_dest_file == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error : opening destinatoin file %s\n", decInfo->dest_fname);
        return e_failure;
    }
    printf("Extension : %s\n", decInfo->dest_fname);
    return e_success;
}

Status decode_secret_file_size(int* file_size, DecodeInfo *decInfo)
{
    char buff[32];
    fread(buff, 1, 32, decInfo->fptr_src_image);
   *file_size = decode_size_from_lsb(buff);
    if(*file_size == 0)
    {
        return e_failure;
    }
    return e_success;
}

Status decode_secret_file_data(int file_size, DecodeInfo *decInfo)
{
    char buff[8], data;
    for(int i=0; i<file_size; i++)
    {
        fread(buff, 1, 8, decInfo->fptr_src_image);
        data = decode_byte_from_lsb(buff);
        fputc(data, decInfo->fptr_dest_file);
    }
    return e_success;
}
int decode_size_from_lsb(char *imageBuffer)
{
    int size = 0;
    for(int i=31; i>=0; i--)
    {
        if(imageBuffer[31-i] & 1)
        {
            size = size | (1<<i);
        }
    }
    return size;
}

char decode_byte_from_lsb(char *imageBuffer)
{
    char ch = 0;
    for(int i=7; i>=0; i--)
    {
        if(imageBuffer[7-i] & 1)
        {
            ch = ch | (1<<i);
        }
    }
    return ch;
}
Status do_decoding(DecodeInfo *decInfo)
{
    //open stego.bmp as input file
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");

    //Error handling
    if(decInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error : opening file %s\n", decInfo->src_image_fname);
        return e_failure;
    }
    
    //call skip_bmp_header
    if(skip_bmp_header(decInfo->fptr_src_image) == e_failure)
    {
        printf("Header is not skipped correctly\n");
        return e_failure;
    }

    //call decode_magic_string_size
    if(decode_magic_string_size(decInfo) == e_failure)
    {
        printf("Unable to decode size of magic string\n");
        return e_failure;
    }

    if(decode_magic_string(decInfo) == e_failure)
    {
        printf("Unable to decode magic string\n");
        return e_failure;
    }

    if(decode_secret_file_extn_size(decInfo) == e_failure)
    {
        printf("Unable to decode size of extension\n");
        return e_failure;
    }

    if(decode_secret_file_extn(decInfo) == e_failure)
    {
        printf("Unable to decode extension\n");
        return e_failure;
    }

    int file_size;
    if(decode_secret_file_size(&file_size, decInfo) == e_failure)
    {
        printf("Unable to decode size of secret file\n");
        return e_failure;
    }

    printf("Size of file : %d\n", file_size);
    if(decode_secret_file_data(file_size, decInfo) == e_failure)
    {
        printf("Unable to decode secret file data\n");
        return e_failure;
    }

    return e_success;
}
