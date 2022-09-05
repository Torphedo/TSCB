#include <stdio.h>
#include <memory.h>
#include <stdint.h>

#include "main.h"
#include "ryml.h"

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
        SwapEndianFloat(&header.TerrainHeightScale);
        SwapEndianUInt(&header.MaterialInfoLength);
        SwapEndianUInt(&header.AreaArrayLength);
        SwapEndianFloat(&header.TileSize);
        printf("Version: %d\n", header.Version);
        printf("Unknown 1: %d\n", header.Unknown1);
        printf("Base File Table Offset (Relative): 0x%x\n", header.BaseTableOffset);
        printf("World Scale: %f\n", header.WorldScale);
        printf("Terrain Height Scale: %f\n", header.TerrainHeightScale);
        printf("Size of Material Info Array: %d\n", header.MaterialInfoLength);
        printf("Size of Area Array: %d\n", header.AreaArrayLength);
        printf("Tile Size: %f\n", header.TileSize);
        printf("\n\nMaterial Information Section\n\n");
    }

    FILE* yamlout = fopen("MainField.yaml", "wb");
    ryml_set_header_data(header);

	// Material Information Section
	unsigned int SectionSize;
	fread(&SectionSize, sizeof(unsigned int), 1, TSCB_file);
	SwapEndianUInt(&SectionSize);
	printf("Section Size: %u\n", SectionSize);

	// Lookup table containing relative offsets to every element of the material info array
	unsigned int MatLookup[header.MaterialInfoLength];
    fread(&MatLookup, sizeof(unsigned int), header.MaterialInfoLength, TSCB_file);
    {
    for (int i = 0; i < header.MaterialInfoLength; i++)
        SwapEndianUInt(&MatLookup[i]);  // Fix endian-ness
    }

    // Texture coordinates and other metadata
	for (int i = 0; i < header.MaterialInfoLength; i++)
	{
        struct MaterialInfoData MatInfo;
        fread(&MatInfo, sizeof(MatInfo), 1, TSCB_file);
        SwapEndianUInt(&MatInfo.index);
        SwapEndianFloat(&MatInfo.Texture_V);
        SwapEndianFloat(&MatInfo.Texture_U);
        SwapEndianFloat(&MatInfo.Unknown1);
        SwapEndianFloat(&MatInfo.Unknown2);
        ryml_set_matinfo_data(MatInfo, i);
    }

    // Area Array

    unsigned int AreaLookup[header.AreaArrayLength];
    fread(&AreaLookup, sizeof(unsigned int), header.AreaArrayLength, TSCB_file);
    for (int i = 0; i < header.AreaArrayLength; i++)
    {
        SwapEndianUInt(&AreaLookup[i]); // Fix endian-ness
    }

    struct AreaArrayData AreaArray[header.AreaArrayLength];
    for (int i = 0; i < header.AreaArrayLength; i++)
    {
        fread(&AreaArray[i], sizeof(AreaArray), 1, TSCB_file);
        SwapEndianFloat(&AreaArray[i].XPosition);
        SwapEndianFloat(&AreaArray[i].YPosition);
        SwapEndianFloat(&AreaArray[i].AreaSize);
        SwapEndianFloat(&AreaArray[i].MinTerrainHeight);
        SwapEndianFloat(&AreaArray[i].MaxTerrainHeight);
        SwapEndianFloat(&AreaArray[i].MinWaterHeight);
        SwapEndianFloat(&AreaArray[i].MaxWaterHeight);
        SwapEndianUInt(&AreaArray[i].Unknown1);
        SwapEndianUInt(&AreaArray[i].FileBaseOffset);
        SwapEndianUInt(&AreaArray[i].Unknown2);
        SwapEndianUInt(&AreaArray[i].Unknown3);
        SwapEndianUInt(&AreaArray[i].ref_extra);

        if (AreaArray[i].ref_extra != 0)
        {
            fread(&AreaArray[i].ExtraInfoLength, sizeof(unsigned int), 1, TSCB_file);
            SwapEndianUInt(&AreaArray[i].ExtraInfoLength);
            if (AreaArray[i].ExtraInfoLength == 8)
            {
                fread(&AreaArray[i].HeaderUnknown, sizeof(unsigned int), 1, TSCB_file);
                SwapEndianUInt(&AreaArray[i].HeaderUnknown);
            }
            for (int j = -1; j < (AreaArray[i].ExtraInfoLength / 4); j++)
            {
                struct ExtraAreaArray ExtraInfo;
                fread(&ExtraInfo, sizeof(unsigned int) * 4, 1, TSCB_file);
                SwapEndianUInt(&ExtraInfo.ExtraUnknown1); // Always 3.
                SwapEndianUInt(&ExtraInfo.ExtraUnknown2); // Setting this to 0 = Grass, 1 = Water
                SwapEndianUInt(&ExtraInfo.ExtraUnknown3); // Always 1.
                SwapEndianUInt(&ExtraInfo.ExtraUnknown4); // Always 0.
            }
        }
    }

    ryml_emit(yamlout);

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
