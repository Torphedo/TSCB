#include <cstdio>

#include "main.h"

int main()
{
	FILE* TSCB_file;
	TSCB_Header header = {0};
	
    TSCB_file = fopen("MainField.tscb", "rb");
	if (TSCB_file == nullptr)
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
    }

    root |= ryml::MAP;

    header_yaml header_yaml;
    header_yaml.WorldScale = header_yaml.Root["WorldScale"];
    header_yaml.TerrainHeightScale = header_yaml.Root["TerrainHeightScale"];
    header_yaml.TileSize = header_yaml.Root["TileSize"];

    header_yaml.WorldScale << header.WorldScale;
    header_yaml.TerrainHeightScale << header.TerrainHeightScale;
    header_yaml.TileSize << header.TileSize;

	// Material Information Section
	unsigned int SectionSize;
	fread(&SectionSize, sizeof(unsigned int), 1, TSCB_file);
	SwapEndianUInt(&SectionSize);

	/*
	   Lookup table containing relative offsets to every element of the material info array.
       This isn't parsed or stored because the table will be re-generated on save. The only
       reason it's read at all is to advance the file pointer.
    */

	auto* MatLookup = new unsigned int[sizeof(unsigned int) * header.MaterialInfoLength];
    fread(MatLookup, sizeof(unsigned int) * header.MaterialInfoLength, 1, TSCB_file);

    // Texture coordinates and other metadata
	for (int i = 0; i < header.MaterialInfoLength; i++)
	{
        MaterialInfoData MatInfo = {0};
        fread(&MatInfo, sizeof(MatInfo), 1, TSCB_file);
        SwapEndianUInt(&MatInfo.index);
        SwapEndianFloat(&MatInfo.Texture_V);
        SwapEndianFloat(&MatInfo.Texture_U);
        SwapEndianFloat(&MatInfo.Unknown1);
        SwapEndianFloat(&MatInfo.Unknown2);

        materialinfo_yaml materialinfo;

        materialinfo.Array = materialinfo.Root[i];
        materialinfo.Array |= ryml::MAP;
        materialinfo.Index = materialinfo.Array["Index"];
        materialinfo.TextureCoordU = materialinfo.Array["TextureCoordU"];
        materialinfo.TextureCoordV = materialinfo.Array["TextureCoordV"];
        materialinfo.UnknownFloat1 = materialinfo.Array["UnknownFloat1"];
        materialinfo.UnknownFloat2 = materialinfo.Array["UnknownFloat2"];

        materialinfo.Index << i;
        materialinfo.TextureCoordU << MatInfo.Texture_U;
        materialinfo.TextureCoordV << MatInfo.Texture_V;
        materialinfo.UnknownFloat1 << MatInfo.Unknown1;
        materialinfo.UnknownFloat2 << MatInfo.Unknown2;
    }

    // Area Array

    // Same reasoning as in the Material Information lookup table.
    auto* AreaLookup = new unsigned int[sizeof(unsigned int) * header.AreaArrayLength];
    fread(AreaLookup, sizeof(unsigned int) * header.AreaArrayLength, 1, TSCB_file);

    for (int i = 0; i < header.AreaArrayLength; i++)
    {
        // This is getting freed at the end of the loop right now. The next
        // step is to stream the data to the YAML tree in the loop.
        AreaArrayData AreaArray = {0};

        fread(&AreaArray, sizeof(AreaArray), 1, TSCB_file);
        SwapEndianFloat(&AreaArray.XPosition);
        SwapEndianFloat(&AreaArray.ZPosition);
        SwapEndianFloat(&AreaArray.AreaSize);
        SwapEndianFloat(&AreaArray.MinTerrainHeight);
        SwapEndianFloat(&AreaArray.MaxTerrainHeight);
        SwapEndianFloat(&AreaArray.MinWaterHeight);
        SwapEndianFloat(&AreaArray.MaxWaterHeight);
        SwapEndianUInt(&AreaArray.ExtraInfoElementCount);
        SwapEndianUInt(&AreaArray.FileBaseOffset);
        SwapEndianUInt(&AreaArray.Unknown2);
        SwapEndianUInt(&AreaArray.Unknown3);
        SwapEndianUInt(&AreaArray.ref_extra);

        AreaArray_yaml area(i);

        area.Index << i;
        area.XPosition << AreaArray.XPosition;
        area.ZPosition << AreaArray.ZPosition;
        area.AreaSize << AreaArray.AreaSize;
        area.MinTerrainHeight << AreaArray.MinTerrainHeight;
        area.MaxTerrainHeight << AreaArray.MaxTerrainHeight;
        area.MinWaterHeight << AreaArray.MinWaterHeight;
        area.MaxWaterHeight << AreaArray.MaxWaterHeight;
        area.ExtraInfoElementCount << AreaArray.ExtraInfoElementCount;
        area.ref_extra << AreaArray.ref_extra;


        if (AreaArray.ref_extra != 0)
        {
            // Extra Info table, only present if ref_extra != 0.

            ExtraInfo_yaml ExtraYaml;

            ExtraYaml.Root = area.Root;
            ExtraYaml.Root |= ryml::SEQ;

            ExtraYaml.Array = area.Array;
            ExtraYaml.Array |= ryml::MAP;

            ExtraYaml.ExtraInfoLength = ExtraYaml.Array["ExtraInfoLength"];

            unsigned int ExtraInfoLength; // Number of values in the array

            fread(&ExtraInfoLength, sizeof(unsigned int), 1, TSCB_file);
            SwapEndianUInt(&ExtraInfoLength);
            ExtraYaml.ExtraInfoLength << ExtraInfoLength;
            if (ExtraInfoLength == 8)
            {
                unsigned int HeaderUnknown;   // Only present if ExtraInfoLength = 8
                fread(&HeaderUnknown, sizeof(unsigned int), 1, TSCB_file);
                SwapEndianUInt(&HeaderUnknown); // Not printed to YAML, we can insert this at save time
            }

            for (int j = 0; j < AreaArray.ExtraInfoElementCount; j++)
            {
                ExtraAreaArray ExtraInfo = {0};
                fread(&ExtraInfo, sizeof(unsigned int) * 4, 1, TSCB_file);
                SwapEndianUInt(&ExtraInfo.ExtraUnknown1); // Always 3.
                SwapEndianUInt(&ExtraInfo.ExtraUnknown2); // Setting this to 0 = Grass, 1 = Water
                SwapEndianUInt(&ExtraInfo.ExtraUnknown3); // Always 1.
                SwapEndianUInt(&ExtraInfo.ExtraUnknown4); // Always 0.

                ExtraYaml.ExtraUnknown2 = ExtraYaml.Array["ExtraUnknown2"];
                ExtraYaml.ExtraUnknown2 << ExtraInfo.ExtraUnknown2;
            }
        }
    }

    FILE* yamlout = fopen("MainField.yaml", "wb");
    ryml::emit(yaml_tree, yamlout); // Writes the YAML tree to a file
    fclose(yamlout);

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
