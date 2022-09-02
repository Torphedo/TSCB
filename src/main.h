#pragma once

struct TSCB_Header
{
	// Header
	const char ID[4];				 // This will always be 54 53 43 42 ("TSCB")
	unsigned int Version;			 // Game will crash on load unless this is 10.
	unsigned int Unknown1;			 // Unknown. Game crashes on load unless this is 00 00 00 01.
	unsigned int BaseTableOffset;	 // file_base table relative offset.
	float WorldScale;				 // Normally 500.0. Scales terrain along X & Z axis.
	float TerrainMaxHeight;			 // Used to calculate height values from HGHT. Normally 800.0.
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

void SwapEndianInt(int* input);
void SwapEndianUInt(unsigned int* input);
void SwapEndianFloat(float* floatinput);
