LSB Bit Image Steganography
Author: Malatesh j kusagur
Date: 15-12-2025

1.Overview:

This is a command-line tool written in C that implements Least Significant Bit (LSB) Steganography.
It allows users to hide secret text files inside a .bmp image file without visibly changing the image.

2.Features:

Encode a secret file (e.g., .txt) into a BMP image
Decode hidden data from a BMP image
Checks if the image has enough capacity for the secret
Uses a "Magic String" to verify hidden data
Preserves the BMP header to maintain image validity
Tools and Technologies

Language: C
Concepts: Pointers, Structures, File I/O, Bitwise Operators
Image Format: BMP (Bitmap)
How to Compile

gcc -o stego main.c encode.c decode.c

3.How to Run:

Encode a secret file:

./stego -e beautiful.bmp secret.txt stego.bmp

Decode a hidden file:

./stego -d stego.bmp decoded_secret.txt

4.Project Structure:

main.c : Entry point, handles command line arguments
encode.c : Embeds secret data into BMP image
decode.c : Extracts hidden data from BMP image
common.h : Contains magic string definition
types.h : User-defined types and enums

5.Conclusion:
This project demonstrates image steganography using LSB, allowing secure hiding and retrieval of text files in BMP images while maintaining image quality.
