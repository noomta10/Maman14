#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "assembler.h"
#include "encoding.h"


/* Base64 character mapping table */
static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


/*  Encode a 6-bit number to base64 */
char encode_six_bit_number(unsigned int bits) {
    return base64_chars[bits];
}

/* Get an unsinged int number, divide its 12 bits into two groups and encode each group into base 64.
Return the base 64 encoded number */
char* encode_base64(unsigned int number) {
    char final_base64[3];
    char* final_base64_pointer = final_base64;

    /* Extract the two 6-bit groups */
    unsigned int number_left_part = (number >> 6) & 0x3F; /* Left part of the number AND 111111. Extract left part */ 
    unsigned int number_right_part = number & 0x3F; /* Right part of the number AND 111111. Extract right part */

    /* Encode each group using base64 */
    char encoded_char1 = encode_six_bit_number(number_left_part);
    char encoded_char2 = encode_six_bit_number(number_right_part);

    /* Add the data to the buffer */
    final_base64[0] = encoded_char1;
    final_base64[1] = encoded_char2;
    final_base64[2] = '\0';

    return final_base64_pointer;
}
