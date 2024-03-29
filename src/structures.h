#pragma once

struct TSCB_Header
{
    char ID[4];				         // This will always be 54 53 43 42 ("TSCB")
    unsigned int Version;			 // Game will crash on load unless this is 10.
    unsigned int Unknown1;			 // Unknown. Game crashes on load unless this is 00 00 00 01.
    unsigned int BaseTableOffset;	 // file_base table relative offset.
    float WorldScale;				 // Normally 500.0. Scales terrain along X & Z axis.
    float TerrainHeightScale;		 // Used to calculate height values from HGHT. Normally 800.0.
    unsigned int MaterialInfoLength; // # of elements in material_info_array
    unsigned int AreaArrayLength;	 // # of elements in area_array
    unsigned int _pad[2];
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
    float ZPosition;
    float AreaSize;         // Length & width of the area
    float MinTerrainHeight; // Range from 0 to 1
    float MaxTerrainHeight; // Range from 0 to 1
    float MinWaterHeight;   // Range from 0 to 1
    float MaxWaterHeight;   // Range from 0 to 1
    unsigned int ExtraInfoElementCount; // The number of elements in the extra info array
    unsigned int FileBaseOffset; // Relative offset to file base name string
    // The wiki says Unknown2 & Unknown3 are "usually" 0, but I haven't found any instances
    // where they aren't (checked programmatically).
    unsigned int Unknown2;
    unsigned int Unknown3;
    unsigned int ref_extra; // Indicates if there will be an extra_info_array attached.
};

struct ExtraAreaArray
{
    unsigned int ExtraUnknown1; // Always 3.
    unsigned int GrassOrWater;  // Setting this to 0 = Grass, 1 = Water
    unsigned int ExtraUnknown3; // Always 1.
    unsigned int ExtraUnknown4; // Always 0.
};
