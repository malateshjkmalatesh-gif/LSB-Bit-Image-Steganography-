#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr,0,SEEK_END);
    return (uint)ftell(fptr);
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

//read data from command line arguments entered by user and validate them
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //validate if 3rd argument is actually a .bmp file
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
    //store that .bmp file name into structure
    encInfo->src_image_fname = argv[2];

    //validate if 4rd argument is actually a .txt
    if(strstr(argv[3], ".txt") == NULL) 
    {
        return e_failure;
    }
    else
    {
        if(strcmp(strstr(argv[3], ".txt"),".txt"))
        {
            return e_failure;
        }
    }

    //store name to structure
    encInfo->secret_fname = argv[3];

    //store extension to structure
    strcpy(encInfo->extn_secret_file, ".txt");

    /*validate if there is 5th argument 
     if it is there then check if it is .bmp file*/
    if(argv[4] != NULL) 
    {
        if(strstr(argv[4], ".bmp") != NULL && strcmp(strstr(argv[4], ".bmp"),".bmp")==0) 
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
    
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }
    printf("Hello\n");

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    //store the size of image into structure
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    //store size of secret file into structure
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    //calculate required size
    uint req_size = 4*8 + strlen(MAGIC_STRING)*8 + 4*8 + strlen(encInfo->extn_secret_file)*8 + 4*8 + strlen(encInfo->secret_data)*8;

    //validate for required size and image capacity
    if(req_size < encInfo->image_capacity)
    {
        return e_success;
    }
    return e_failure;
}

//copy bmp file header of image to output image file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    //move file pointer to beginning
    fseek(fptr_src_image, 0, SEEK_SET);

    //read 54 bytes and write to destination image
    char buff[54];
    fread(buff, 1, 54, fptr_src_image);
    fwrite(buff, 1, 54, fptr_dest_image);

    //validate if pointer of destination image is 54 or not
    if(ftell(fptr_dest_image) != 54)
    {
        return e_failure;
    }
    return e_success;
}

//encode magic string to a image file 
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    //buffer to store 8 bytes
    char buff[8];
    for(int i=0; i<strlen(magic_string); i++)
    {
        //read 8 bytes from source image and store it in 8 bytes of buff
        fread(buff, 1, 8, encInfo->fptr_src_image);

        //call encode byte to LSB
        if(encode_byte_to_lsb(magic_string[i], buff)==e_failure)
        {
            return e_failure;
        }

        //write that 8 bytes to output/stego image files
        fwrite(buff, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;

}

//encode size of secret file extension into output image
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    //buffer to read and store 32 bytes of input image
    char buff[32];
    fread(buff, 1, 32, encInfo->fptr_src_image);

    //call encode size to LSB
    if(encode_size_to_lsb(size, buff) == e_failure)
    {
        return e_failure;
    }

    //write those 32 bytes into output image
    fwrite(buff, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

//encode extension of secret file
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    //buffer to store 8 bytes
    char buff[8];
    for(int i=0; i<strlen(file_extn); i++)
    {
        //read 8 bytes from source image and store in 8 bytes
        fread(buff, 1, 8, encInfo->fptr_src_image);

        //call encode byte to LSB
        if(encode_byte_to_lsb(file_extn[i], buff)==e_failure)
        {
            return e_failure;
        }

        //write that 8 bytes to output/stego image files
        fwrite(buff, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

//encode size of secret file
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    //buffer to read and store 32 bytes of input image
    char buff[32];
    fread(buff, 1, 32, encInfo->fptr_src_image);

    //call encode size to LSB
    if(encode_size_to_lsb((int)file_size, buff) == e_failure)
    {
        return e_failure;
    }

    //write that 32 bytes to output/stego image files
    fwrite(buff, 1, 32, encInfo->fptr_stego_image);
    return e_success;
}

//encode data of secret file
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //buffer to store 8 bytes
    char buff[8];
    for(int i=0; i<encInfo->size_secret_file; i++)
    {
        //read 8 bytes from source image and store in 8 bytes
        fread(buff, 1, 8, encInfo->fptr_src_image);

        //call encode byte to LSB
        if(encode_byte_to_lsb(encInfo->secret_data[i], buff) == e_failure)
        {
            return e_failure;
        }

        //write that 8 bytes to output/stego image files
        fwrite(buff, 1, 8, encInfo->fptr_stego_image);
    }
    return e_success;

}

//copy remaiing data of input image to ouput image
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;
    while((ch=fgetc(fptr_src))!=EOF)
    {
        fputc(ch, fptr_dest);
    }
    return e_success;
}

//encode byte of data to lsb of every bytes of image buffer
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=7; i>=0; i--)
    {
        image_buffer[7-i] = image_buffer[7-i] & (~1);
        if(data & (1<<i))
        {
            image_buffer[7-i] = image_buffer[7-i] | 1;
        }
    }
    return e_success;
}

//encode byte of size to lsb of every bytes of image buffer
Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for(int i=31; i>=0; i--)
    {
        imageBuffer[31-i] = (imageBuffer[31-i] & ~(1));
        if(size & (1<<i))
        {
            imageBuffer[31-i] = imageBuffer[31-i] | 1;
        }
    }
    return e_success;
}

//encode size of magic string size
Status encode_magic_string_size(int size, EncodeInfo *enc_info)
{
    //buffer to read and store 32 bytes of input image
    char buff[32];
    fread(buff, 1, 32, enc_info->fptr_src_image);

    //call encode size to LSB
    if(encode_size_to_lsb(size, buff) == e_failure)
    {
        return e_failure;
    }

    //write that 8 bytes to output/stego image files
    fwrite(buff, 1, 32, enc_info->fptr_stego_image);
    return e_success;
}

//main encoding and validations
Status do_encoding(EncodeInfo *encInfo)
{
    //call open_files == e_failure;
    if(open_files(encInfo) == e_failure)
    {
        printf("Unable to open files\n");
        return e_failure;
    }

    //store secret file data into structure
    char ch;
    int i=0;
    while((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        encInfo->secret_data[i++] = ch;
    }

    //call check capacity and validate
    if(check_capacity(encInfo) == e_failure)
    {
        printf("Size of source image is lesser\n");
        return e_failure;
    }
    
    //call copy_bmp_header and validate
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Header is not copied correctly\n");
        return e_failure;
    }

    //call encode_magic_string_size and validate
    if(encode_magic_string_size(strlen(MAGIC_STRING), encInfo) == e_failure)
    {
        printf("Unable to Encode of size of magic string\n");
        return e_failure;
    }

    //call encode_magic_string and validate
    if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("Unable to Encode of magic string\n");
        return e_failure;
    }

    //call encode_secret_file_extension_size and validate
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure)
    {
        printf("Unable to encode size of secret file extension\n");
        return e_failure;
    }

    //call encode_secret_file_extn and validate
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("Unable to encode secret file extension\n");
        return e_failure;
    }

    //call encode_secret_file_size and validate
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("Unable to encode size of secret file\n");
        return e_failure;
    }

    //call encode_secret_file_data and validate
    if(encode_secret_file_data(encInfo) == e_failure)
    {
        printf("Unable to encode secret file\n");
        return e_failure;
    }
    //call copy_remaining_img_data and validate
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Unable to copy remaining image data\n");
        return e_failure;
    }

    return e_success;
}
