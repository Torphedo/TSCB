#pragma once

struct TSCB_Header
{
	// Header
	const char ID[4];				 // This will always be 54 53 43 42 ("TSCB")
	unsigned int Version;			 // Game will crash on load unless this is 10.
	unsigned int Unknown1;			 // Unknown. Game crashes on load unless this is 00 00 00 01.
	unsigned int BaseTableOffset;	 // file_base table relative offset.
	float WorldScale;				 // Normally 500.0. Scales terrain along X & Z axis.
	float TerrainHeightScale;		 // Used to calculate height values from HGHT. Normally 800.0.
	unsigned int MaterialInfoLength; // # of elements in material_info_array
	unsigned int AreaArrayLength;	 // # of elements in area_array
	unsigned int Pad1;				 // Probably padding
	unsigned int Pad2;				 // Probably padding
	float TileSize;					 // Used by area array.
	unsigned int Unknown2;			 // Unknown 00 00 00 08
};

struct MaterialInfoData
{
	unsigned int index;
	float Texture_U;
	float Texture_V;
	float Unknown1;
	float Unknown2;
};

struct AreaArrayData
{
    float XPosition;
    float YPosition;
    float AreaSize;         // "(length & width)"
    float MinTerrainHeight; // Range from 0 to 1
    float MaxTerrainHeight; // Range from 0 to 1
    float MinWaterHeight;   // Range from 0 to 1
    float MaxWaterHeight;   // Range from 0 to 1
    unsigned int Unknown1;  // Usually 0, 1, or 2. Crashes on 4 & 16.
    unsigned int FileBaseOffset; // Relative offset to file base name string
    unsigned int Unknown2;  // Usually 0.
    unsigned int Unknown3;  // Usually 0.
    unsigned int ref_extra; // Indicates if there will be an extra_info_array attached.

    // Extra Info table, only present if ref_extra != 0.

    unsigned int ExtraInfoLength; // Number of values in the array
    unsigned int HeaderUnknown;   // Only present if ExtraInfoLength = 8
};

struct ExtraAreaArray
{
    unsigned int ExtraUnknown1; // Always 3.
    unsigned int ExtraUnknown2; // Setting this to 0 = Grass, 1 = Water
    unsigned int ExtraUnknown3; // Always 1.
    unsigned int ExtraUnknown4; // Always 0.
};

void SwapEndianInt(int* input);
void SwapEndianUInt(unsigned int* input);
void SwapEndianFloat(float* floatinput);
