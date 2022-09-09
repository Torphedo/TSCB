#include <cstdio>
#include <memory.h>

#include "main.h"
#include "yaml.h"

int main()
{
    FILE* TSCB_file = fopen("MainField.tscb", "rb");
    if (TSCB_file == nullptr)
    {
        printf("Couldn't open file! Place MainField.tscb next to the executable.\n");
        return 1;
    }
    else
    {
        FILE* yamlout = fopen("MainField.yaml", "wb");
        DumpTSCBToYaml(TSCB_file, yamlout);
        fclose(yamlout);
        fclose(TSCB_file);
    }
	return 0;
}

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

void SwapEndianInt(int* input)
{
	int converted = 0;
    int value = *input;

    converted |= ((0xff & value) << 24);         // Places the first byte in the 4th byte of the output
    converted |= (((0xff << 8) & value) << 8);   // Places the second byte in the 3rd byte of the output
    converted |= (((0xff << 16) & value) >> 8);  // Places the third byte in the 2nd byte of the output
    converted |= (((0xff << 24) & value) >> 24); // Places the fourth byte in the 1st byte of the output
    *input = converted;
}

void SwapEndianFloat(float* floatinput)
{
	// Copy float data into an int
    unsigned int converted = 0;
    unsigned int value = 0;

    memcpy(&value, floatinput, 4);

    converted |= ((0xff & value) << 24);         // Places the first byte in the 4th byte of the output
    converted |= (((0xff << 8) & value) << 8);   // Places the second byte in the 3rd byte of the output
    converted |= (((0xff << 16) & value) >> 8);  // Places the third byte in the 2nd byte of the output
    converted |= (((0xff << 24) & value) >> 24); // Places the fourth byte in the 1st byte of the output

    memcpy(floatinput, &converted, 4);
}
