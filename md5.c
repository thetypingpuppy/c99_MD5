#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LEFTROTATE(x, c) (  (x << c) | (x >> (32 - c))   )

int main(int argc, char* argv[]){

    if (argc == 1){
        printf("Fatal error. No input given.");
        return 1;
    }

    uint64_t lengthOfInputInBits = strlen(argv[1]) * 8;
    uint64_t lengthOfInputInBytes = strlen(argv[1]) ;

    uint8_t* plengthOfInputInBits = (uint8_t*) &lengthOfInputInBits;
    uint8_t* pLengthOfInputInBytes = (uint8_t*) &lengthOfInputInBytes;

    // Print input
    printf("Input: \t\t\t\t");

    for(int i = 0; i<lengthOfInputInBytes; i++){
        printf("0x%x ", *(argv[1]+i));
    }

    // Size of input in bits
    printf("\r\nSize of Input: \t\t\t%i bits (0x%016x).", lengthOfInputInBits, lengthOfInputInBits);
    
    // Size of input in bits modulo 512
    int sizePlusOneModulo64 = (strlen(argv[1])+1)%64;

    printf("\r\n+ 1 modulo 64 (512 bits): \t%i bytes", sizePlusOneModulo64);

    printf("\r\nPadding required: \t\t%i bytes (to make 56 modulo 64)", 56 - sizePlusOneModulo64);

    uint64_t lengthOfPaddedInputInBytes = strlen(argv[1])+1+56-sizePlusOneModulo64 + 8;

    printf("\r\nPreprocessed input size: \t%i bytes", lengthOfPaddedInputInBytes);

    uint8_t* paddedInput = (uint8_t*) calloc(lengthOfPaddedInputInBytes,1);
    
    for(int i = 0; i < lengthOfInputInBytes; i++){
        *(paddedInput+i) = (uint8_t) *(argv[1]+i);
    }

    paddedInput[lengthOfInputInBytes] = 0x80;

    for(int i = lengthOfPaddedInputInBytes - 8; i < lengthOfPaddedInputInBytes; i++){
        paddedInput[i] = *(plengthOfInputInBits+i%8); //mod 8 works because n*64+56 mod 8 = 1, n*64+56 mod 8 = 2, etc...
    }

    printf("\r\nPreprocessed input: \t\t");

    for(int i = 0; i < lengthOfPaddedInputInBytes; i++){
        if(i%8==0 && i!=0){
            printf("\r\n\t\t\t\t");
        }
        printf("0x%02x ", *(paddedInput+i));
    }

    int s[64] = { 
                7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22, 
                5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20, 
                4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23, 
                6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 
                };

    int K[64] = {
                0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
                };

    //Initialize variables:
    uint32_t a0 = 0x67452301;//A
    uint32_t b0 = 0xefcdab89;//B
    uint32_t c0 = 0x98badcfe;//C
    uint32_t d0 = 0x10325476;//D

    uint32_t* paddedInput_ = (uint32_t*) paddedInput;

    uint32_t A;
    uint32_t B;
    uint32_t C;
    uint32_t D;
    uint32_t F;
    uint32_t g;

    for(uint32_t i = 0; i < lengthOfPaddedInputInBytes / 64; i++){

        A = a0;
        B = b0;
        C = c0;
        D = d0;
        
        for(uint32_t j = 0; j < 64; j++){

            if (j >= 0 && j < 16){
                F = (B & C) | ((~B) & D);
                g = j;

            }
            else if( j >= 16 && j <= 31){
                F = (D & B) | ((~D) & C);
                g = (5*j + 1) % 16;

            }
            else if(j >= 32 && j <=47){

                F = B ^ C ^ D;
                g = (3*j + 5) % 16;
            }

            else if(j >= 48 && j <= 63){
                F = C ^ (B | (~D));
                g = (7*j) % 16;

            }


            F = F + A + K[j] + paddedInput_[i*16+g];
            A = D;
            D = C;
            C = B;
            B = B + LEFTROTATE(F, s[j]);
        }

        a0 = a0 + A;
        b0 = b0 + B;
        c0 = c0 + C;
        d0 = d0 + D;
    }

    uint8_t* a0_ = (uint8_t*) &a0;
    uint8_t* b0_ = (uint8_t*) &b0;
    uint8_t* c0_ = (uint8_t*) &c0;
    uint8_t* d0_ = (uint8_t*) &d0;
    
    printf("\r\n\r\nDigest is: \t\t\t%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x\r\n", a0_[0],a0_[1],a0_[2],a0_[3],b0_[0],b0_[1],b0_[2],b0_[3],c0_[0],c0_[1],c0_[2],c0_[3],d0_[0],d0_[1],d0_[2],d0_[3]);

    free(paddedInput);

}