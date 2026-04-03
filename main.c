#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{

    // Step 1 : Check the argc >= 4 true - > step 2
    // Step 2 : Call the check_operation_type(argv[1]) == e_encode )) true - > step 3
    if(argc >= 4 && check_operation_type(argv[1]) == e_encode)
    {
        // Step 3 : Declare structure variable EncodeInfo enc_info
        EncodeInfo enc_info;

        // Step 4 : Call the read_and_validate_encode_args(argv,&enc_info)== e_success)
        //  true -> Step 5 , false - > terminate the program
        if(read_and_validate_encode_args(argv,&enc_info)== e_failure)
        {
            printf("Invalid file names/arguments\n");
            return 1;
        }
        // Step 5 : Call the do_encoding (&encInfo);
        if(do_encoding(&enc_info)==e_failure)
        {
            printf("Invalid Operation\n");
            return 1;
        }
    }
    else if(argc >= 3 && check_operation_type(argv[1]) == e_decode)
    {
        // Step 3 : Declare structure variable decodeInfo enc_info
        DecodeInfo dec_info;

        // Step 4 : Call the read_and_validate_decode_args(argv,&dec_info)== e_success)
        //  true -> Step 5 , false - > terminate the program
        if(read_and_validate_decode_args(argv,&dec_info)== e_failure)
        {
            printf("Invalid file names/arguments\n");
            return 1;
        }
        // Step 5 : Call the do_decoding (&encInfo);
        if(do_decoding(&dec_info)==e_failure)
        {
            printf("Invalid Operation\n");
            return 1;
        }
    }
    else
    {
        printf("Invalid input\n");
    }
    return 0;
}

OperationType check_operation_type(char *symbol)
{
    // Step 1 : Check whether the symbol is -e or not true - > return e_encode false -> Step 2
    if(strcmp(symbol, "-e") == 0 )
    {
        return e_encode;
    }
    // Step 2 : Check whether the symbol is -d or not true - > return e_decode false -> return e_unsupported
    else if(strcmp(symbol, "-d") == 0 )
    {
        return e_decode;
    }

    return e_unsupported;

}
