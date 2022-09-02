#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>

#include "main.h"

int main()
{
	FILE* TSCB_file;
	struct TSCB_Header header;
	
    TSCB_file = fopen("MainField.tscb", "rb");
	if (TSCB_file == NULL)
	{
		printf("Couldn't open file! Place MainField.tscb next to the executable.\n");
		return 1;
	}

    // Reading header & fixing endian-ness
    fread(&header, sizeof(header), 1, TSCB_file);

    {
        SwapEndianUInt(&header.Unknown1);
        SwapEndianUInt(&header.BaseTableOffset);
        SwapEndianFloat(&header.WorldScale);
        SwapEndianFloat(&header.TerrainMaxHeight);
        SwapEndianUInt(&header.MaterialInfoLength);
        SwapEndianUInt(&header.AreaArrayLength);
        SwapEndianFloat(&header.TileSize);
        printf("Version: %d\n", header.Version);
        printf("Unknown 1: %d\n", header.Unknown1);
        printf("Base File Table Offset (Relative): 0x%x\n", header.BaseTableOffset);
        printf("World Scale: %f\n", header.WorldScale);
        printf("Terrain Max Height: %f\n", header.TerrainMaxHeight);
        printf("Size of Material Info Array: %d\n", header.MaterialInfoLength);
        printf("Size of Area Array: %d\n", header.AreaArrayLength);
        printf("Tile Size: %f\n", header.TileSize);
        printf("\n\nMaterial Information Section\n\n");
    }

	// Material Information Section
	unsigned int SectionSize;
	fread(&SectionSize, sizeof(unsigned int), 1, TSCB_file);
	SwapEndianUInt(&SectionSize);
	printf("Section Size: %u\n", SectionSize);

	// Lookup table containing relative offsets to every element of the material info array
	unsigned int Lookup[header.MaterialInfoLength];
    fread(&Lookup, sizeof(unsigned int), header.MaterialInfoLength, TSCB_file);
    for (int i = 0; i < header.MaterialInfoLength; i++)
    {
        SwapEndianUInt(&Lookup[i]);
    }

    // Texture coordinates and other metadata
	struct MaterialInfoData MatInfo[header.MaterialInfoLength];
	fread(&MatInfo, sizeof(MatInfo), 1, TSCB_file);
	for (int i = 0; i < header.MaterialInfoLength; i++)
	{
		SwapEndianUInt(&MatInfo[i].index);
        SwapEndianFloat(&MatInfo[i].Texture_V);
        SwapEndianFloat(&MatInfo[i].Texture_U);
        SwapEndianFloat(&MatInfo[i].Unknown1);
        SwapEndianFloat(&MatInfo[i].Unknown2);
    }


	fclose(TSCB_file);
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
