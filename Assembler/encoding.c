#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void print_decimal_to_binary(int num) {
    if (num == 0) {
        printf("0"); /* Special case for zero */
        return;
    }

    int bits = sizeof(num) * 8; /* Number of bits in an integer */
    printf("%d\n", bits);
    /* Iterate over each bit from left to right */
    for (int i = bits - 1; i >= 0; i--) {
        int bit = (num >> i) & 1; /* Extract the i-th bit */
        printf("%d", bit);
    }
    printf("\n");
}


/* Base64 character mapping table */
static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Function to encode a 6-bit number to base64 */
char encode_six_bit_number(unsigned int bits) {
    printf("DEBUG bits: %d\n", bits);
    return base64_chars[bits];
}

int encode_base64(unsigned int number) {
    /* Extract the two 6 - bit groups */
    unsigned int group1 = (number >> 6) & 0b111111; // 000000111111 & 111111 = 111111 = '/'
    unsigned int group2 = number & 0b111111; // 111111101010 & 111111 = 101010 = 'q'

    /* Encode each group using base64 */
    char encoded_char1 = encode_six_bit_number(group1);
    char encoded_char2 = encode_six_bit_number(group2);

    /* Print the base64 encoded characters */
    printf("\nBinary number to encode:\n");
    print_decimal_to_binary(number);
    printf("Base64 encoded characters: %c%c\n", encoded_char1, encoded_char2);
    return 0;
}