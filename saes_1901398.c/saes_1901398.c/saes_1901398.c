#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 2
#define KEY_SIZE 2

// S-Box lookup table
static uint8_t sbox[16] = {
    0x9, 0x4, 0xA, 0xB,
    0xD, 0x1, 0x8, 0x5,
    0x6, 0x2, 0x0, 0x3,
    0xC, 0xE, 0xF, 0x7
};

// Inverse Substitution Box (Inverse S-Box)
static uint16_t InvSBox[16] = {
    0xA, 0x5, 0x9, 0xB,
    0x1, 0x7, 0x8, 0xF,
    0x6, 0x0, 0x2, 0x3,
    0xC, 0x4, 0xD, 0xE
};

// Function to substitute a nibble using the S-Box
uint8_t subNib(uint8_t nibble) {
    // Separate the nibble into two 2-bit values
    uint8_t upperNibble = (nibble & 0xF0) >> 4;  // Get the upper 4 bits
    uint8_t lowerNibble = nibble & 0x0F;         // Get the lower 4 bits

    // Perform S-Box substitution for each 4-bit value
    uint8_t upperResult = sbox[upperNibble];
    uint8_t lowerResult = sbox[lowerNibble];

    // Combine the results into a single byte
    return (upperResult << 4) | lowerResult;
}

// Inverse Substitution (S-box) function for decryption
uint8_t subNibInverse(uint8_t nibble) {
    // Separate the nibble into two 2-bit values
    uint8_t upperNibble = (nibble & 0xF0) >> 4;  // Get the upper 4 bits
    uint8_t lowerNibble = nibble & 0x0F;         // Get the lower 4 bits

    // Perform S-Box substitution for each 4-bit value
    uint8_t upperResult = InvSBox[upperNibble];
    uint8_t lowerResult = InvSBox[lowerNibble];

    // Combine the results into a single byte
    return (upperResult << 4) | lowerResult;
}

// Function to rotate the nibbles in a byte
uint8_t RotNib(uint8_t byte) {
    // Extract upper and lower nibbles
    uint8_t upperNibble = (byte >> 4) & 0x0F;
    uint8_t lowerNibble = byte & 0x0F;

    // Swap the nibbles and combine them
    return (lowerNibble << 4) | upperNibble;
}

// Shift Row function
void shiftRow(uint8_t state[BLOCK_SIZE]) {
    // No shift for Row 0
    // Shift Row 1
    // Replace first 4 bits in Row 2 with last 4 bits
    state[1] = ((state[1] & 0x0F) << 4) | ((state[1] & 0xF0) >> 4);
}

// Galois Field Multiplication
uint8_t galoisMultiply(uint8_t a, uint8_t b) {
    uint8_t result = 0;
    uint8_t hiBitSet;

    for (int i = 0; i < 4; i++) {
        if ((b & 1) == 1) {
            result ^= a;
        }
        hiBitSet = a & 0x08;
        a <<= 1;
        if (hiBitSet == 0x08) {
            a ^= 0x13; // 0x3 is the irreducible polynomial x^4 + x + 1 in GF(2^4)
        }
        b >>= 1;
    }
    return result;
}

// Mix Columns function
void mixColumns(uint8_t state[BLOCK_SIZE]) {
    uint8_t result[BLOCK_SIZE];

    uint8_t a = (state[0] >> 4) & 0x0F;
    uint8_t b = state[0] & 0x0F;
    uint8_t c = (state[1] >> 4) & 0x0F;
    uint8_t d = state[1] & 0x0F;

    uint8_t Result1 = galoisMultiply(0x01, a) ^ galoisMultiply(0x04, c);
    uint8_t Result2 = galoisMultiply(0x01, b) ^ galoisMultiply(0x04, d);
    result[0] = (Result1 << 4) | Result2;
    state[0] = result[0];

    Result1 = galoisMultiply(0x04, a) ^ galoisMultiply(0x01, c);
    Result2 = galoisMultiply(0x04, b) ^ galoisMultiply(0x01, d);
    result[1] = (Result1 << 4) | Result2;
    state[1] = result[1];
}

// Inverse Mix Columns function
void mixColumnsInverse(uint8_t state[BLOCK_SIZE]) {
    uint8_t result[BLOCK_SIZE];

    uint8_t a = (state[0] >> 4) & 0x0F;
    uint8_t b = state[0] & 0x0F;
    uint8_t c = (state[1] >> 4) & 0x0F;
    uint8_t d = state[1] & 0x0F;

    uint8_t Result1 = galoisMultiply(0x09, a) ^ galoisMultiply(0x02, c);
    uint8_t Result2 = galoisMultiply(0x09, b) ^ galoisMultiply(0x02, d);
    result[0] = (Result1 << 4) | Result2;
    state[0] = result[0];

    Result1 = galoisMultiply(0x02, a) ^ galoisMultiply(0x09, c);
    Result2 = galoisMultiply(0x02, b) ^ galoisMultiply(0x09, d);
    result[1] = (Result1 << 4) | Result2;
    state[1] = result[1];
}


// Add Round Key function
void addRoundKey(uint8_t state[BLOCK_SIZE], uint16_t Key) {
    uint8_t result[BLOCK_SIZE];

    uint8_t a = (Key >> 12) & 0x0F;
    uint8_t b = (Key >> 8) & 0x0F;
    uint8_t c = (Key >> 4) & 0x0F;
    uint8_t d = Key & 0x0F;

    result[0] = (a << 4) | c;
    result[1] = (b << 4) | d;

    state[0] = result[0] ^ state[0];
    state[1] = result[1] ^ state[1];
}

// Simplified AES Encrypt function
void simplifiedAESEncrypt(uint8_t state[BLOCK_SIZE], uint8_t key[KEY_SIZE]) {
    // Generate round keys
    uint8_t roundKeys[KEY_SIZE * 3];

    uint8_t a = key[0] & 0x0F;
    uint8_t b = key[1] & 0x0F;
    roundKeys[1] = (a << 4) | b;

    a = (key[0] >> 4) & 0x0F;
    b = (key[1] >> 4) & 0x0F;
    roundKeys[0] = (a << 4) | b;

    roundKeys[2] = roundKeys[0] ^ 0x80 ^ subNib(RotNib(roundKeys[1]));
    roundKeys[3] = roundKeys[2] ^ roundKeys[1];
    roundKeys[4] = roundKeys[2] ^ 0x30 ^ subNib(RotNib(roundKeys[3]));
    roundKeys[5] = roundKeys[4] ^ roundKeys[3];

    uint16_t key0 = (roundKeys[0] << 8) | roundKeys[1];
    uint16_t key1 = (roundKeys[2] << 8) | roundKeys[3];
    uint16_t key2 = (roundKeys[4] << 8) | roundKeys[5];

    // Initial Round Key Addition
    addRoundKey(state, key0);

    // Perform one round of Simplified AES
    // Nibble Substitution
    state[0] = subNib(state[0]);
    state[1] = subNib(state[1]);

    // Shift Rows
    shiftRow(state);

    // Mix Columns
    mixColumns(state);

    // Add Round Key
    addRoundKey(state, key1);

    // Nibble Substitution
    state[0] = subNib(state[0]);
    state[1] = subNib(state[1]);

    // Shift Rows
    shiftRow(state);

    // Final Round Key Addition
    addRoundKey(state, key2);
}

// Simplified AES Decrypt function
void simplifiedAESDecrypt(uint8_t state[BLOCK_SIZE], uint8_t key[KEY_SIZE]) {
    // Generate round keys
    uint8_t roundKeys[KEY_SIZE * 3];

    uint8_t a = key[0] & 0x0F;
    uint8_t b = key[1] & 0x0F;
    roundKeys[1] = (a << 4) | b;

    a = (key[0] >> 4) & 0x0F;
    b = (key[1] >> 4) & 0x0F;
    roundKeys[0] = (a << 4) | b;

    roundKeys[2] = roundKeys[0] ^ 0x80 ^ subNib(RotNib(roundKeys[1]));
    roundKeys[3] = roundKeys[2] ^ roundKeys[1];
    roundKeys[4] = roundKeys[2] ^ 0x30 ^ subNib(RotNib(roundKeys[3]));
    roundKeys[5] = roundKeys[4] ^ roundKeys[3];

    uint16_t key0 = (roundKeys[0] << 8) | roundKeys[1];
    uint16_t key1 = (roundKeys[2] << 8) | roundKeys[3];
    uint16_t key2 = (roundKeys[4] << 8) | roundKeys[5];

    // Initial Round Key Addition
    addRoundKey(state, key2);

    // Inverse Shift Rows 
    shiftRow(state);

    // Inverse Nibble Substitution
    state[0] = subNibInverse(state[0]);
    state[1] = subNibInverse(state[1]);

    addRoundKey(state, key1);

    // Inverse Mix Columns
    mixColumnsInverse(state);

    // Inverse Shift Rows
    shiftRow(state);

    // Inverse Nibble Substitution
    state[0] = subNibInverse(state[0]);
    state[1] = subNibInverse(state[1]);
    // Final Round Key Addition
    addRoundKey(state, key0);
}

int main(int argc, const char* argv[]) {
    // Command line argument validation
    if (argc != 4){
        printf("Needs 3 parameters. Expects:\n\t%s  ENC|DEC  key  data\n", argv[0]);
        exit(1);
    }
    // Extract key and data from command line arguments
    uint8_t key[KEY_SIZE] = {00,00};
    uint8_t data[BLOCK_SIZE] = {00,00};

    // Convert hexadecimal string to integer
    uint16_t Key = strtol(argv[2], NULL, 16);
    uint16_t Data = strtol(argv[3], NULL, 16);

    // Initialize key and data with the parsed values
    uint8_t result[BLOCK_SIZE];
    uint8_t a = (Key >> 12) & 0x0F;
    uint8_t b = (Key >> 8) & 0x0F;
    uint8_t c = (Key >> 4) & 0x0F;
    uint8_t d = Key & 0x0F;
    result[0] = (a << 4) | c;
    result[1] = (b << 4) | d;
    key[0] = result[0] ^ key[0];
    key[1] = result[1] ^ key[1];

    a = (Data >> 12) & 0x0F;
    b = (Data >> 8) & 0x0F;
    c = (Data >> 4) & 0x0F;
    d = Data & 0x0F;
    result[0] = (a << 4) | c;
    result[1] = (b << 4) | d;
    data[0] = result[0] ^ data[0];
    data[1] = result[1] ^ data[1];

    // Perform encryption or decryption based on the command
    if (strcmp(argv[1], "ENC") == 0) {
        simplifiedAESEncrypt(data, key);
        uint8_t e = data[0] & 0x0F;
        uint8_t f = data[1] & 0x0F;
        uint8_t resultE[BLOCK_SIZE];
        resultE[1] = (e << 4) | f;
        e = (data[0] >> 4) & 0x0F;
        f = (data[1] >> 4) & 0x0F;
        resultE[0] = (e << 4) | f;
        printf("Encrypted data: %02x%02x\n", resultE[0], resultE[1]);
        return 1;
    }
    else if (strcmp(argv[1], "DEC") == 0) {
        simplifiedAESDecrypt(data, key);
        uint8_t e = data[0] & 0x0F;
        uint8_t f = data[1] & 0x0F;
        uint8_t resultE[BLOCK_SIZE];
        resultE[1] = (e << 4) | f;
        e = (data[0] >> 4) & 0x0F;
        f = (data[1] >> 4) & 0x0F;
        resultE[0] = (e << 4) | f;
	    printf("Decrypted data: %02x%02x\n", resultE[0], resultE[1]);
	    return 1;
    }
    else{
        printf("Invalid Input.\n");
        exit(1);
    }

    return 0;
}