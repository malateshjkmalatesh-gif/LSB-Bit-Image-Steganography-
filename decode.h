#ifndef DECODE
#define DECODE
#include <stdio.h>
#include "types.h"

//structure to store details
typedef struct
{
    /*input/stego file info*/
    char* src_image_fname;  // To store the src image name
    FILE* fptr_src_image;   // To store the address of the src image

    /*to store size of magic string*/
    int magic_size;   //to store size of magic string

    /*output file info*/
    int size_dest_file_extn;    //to store destination file extension
    long size_dest_file;        //to store size destination file
    char dest_fname[20];        //to store destination file name
    FILE* fptr_dest_file;       //to store address of destination file

}DecodeInfo;

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/*Skip bmp_header*/
Status skip_bmp_header(FILE*);

/* Decode magic string size*/
Status decode_magic_string_size(DecodeInfo *decInfo);

/*decode magic string*/
Status decode_magic_string(DecodeInfo *decInfo);

/*Decode extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(int* file_size, DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(int file_size, DecodeInfo *decInfo);

/* Decode a byte from LSB */
char decode_byte_from_lsb(char *image_buffer);

/* Decode a size from lsb */
int decode_size_from_lsb(char *imageBuffer);

#endif
