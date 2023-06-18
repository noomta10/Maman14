#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


/* Base64 character mapping table */
static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


/* For debugging */
void print_decimal_to_binary(int num, int number_of_bits_to_print) {
    //if (num == 0) {
    //    printf("0"); /* Special case for zero */
    //    return;
    //}

    //int bits = sizeof(num) * 8; /* Number of bits in an integer */
    /*printf("%d\n", bits);*/
    /* Iterate over each bit from left to right */
    for (int i = number_of_bits_to_print - 1; i >= 0; i--) {
        int bit = (num >> i) & 1; /* Extract the i-th bit */
        printf("%d", bit);
    }
    //printf("\n");
}


/* Function to encode a 6-bit number to base64 */
char encode_six_bit_number(unsigned int bits) {
    return base64_chars[bits];
}


char* encode_base64(unsigned int number) {
    char final_base64[3];
    /* Extract the two 6 - bit groups */
    unsigned int number_left_part = (number >> 6) & 0b111111; // left part of the number & 111111 
    unsigned int number_right_part = number & 0b111111; // right part of the number & 111111 

    /* Encode each group using base64 */
    char encoded_char1 = encode_six_bit_number(number_left_part);
    char encoded_char2 = encode_six_bit_number(number_right_part);

    final_base64[0] = encoded_char1;
    final_base64[1] = encoded_char2;
    final_base64[2] = '\0';

    return final_base64;
}