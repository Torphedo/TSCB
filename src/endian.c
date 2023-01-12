#include "memory.h"

void SwapEndianUInt(unsigned int* input)
{
    unsigned int converted = 0;
    unsigned int value = *input;

    converted |= ((0xff & value) << 24);        // Places the first byte in the 4th byte of the output
    converted |= (((0xff << 8) & value) << 8);  // Places the second byte in the 3rd byte of the output
    converted |= (((0xff << 16) & value) >> 8); // Places the third byte in the 2nd byte of the output
    converted |= (((0xff << 24) & value) >> 24);// Places the fourth byte in the 1st byte of the output
    *input = converted;
}

void SwapEndianFloat(float* input)
{
    unsigned int converted = 0;
    unsigned int value = 0;

    // Copy float data into an int
    memcpy(&value, input, 4);

    converted |= ((0xff & value) << 24);         // Places the first byte in the 4th byte of the output
    converted |= (((0xff << 8) & value) << 8);   // Places the second byte in the 3rd byte of the output
    converted |= (((0xff << 16) & value) >> 8);  // Places the third byte in the 2nd byte of the output
    converted |= (((0xff << 24) & value) >> 24); // Places the fourth byte in the 1st byte of the output

    memcpy(input, &converted, 4);
}
