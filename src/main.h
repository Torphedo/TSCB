#pragma once

#define RYML_SINGLE_HDR_DEFINE_NOW
#include "vendor/rapidyaml-0.4.1.hpp"

ryml::Tree yaml_tree;
ryml::NodeRef root = yaml_tree.rootref();

struct header_yaml
{
    ryml::NodeRef Root;
    ryml::NodeRef WorldScale;
    ryml::NodeRef TerrainHeightScale;
    ryml::NodeRef TileSize;
    header_yaml()
    {
        Root = root["Header"];
        Root |= ryml::MAP;
    }
};

struct materialinfo_yaml
{
    ryml::NodeRef Root;
    ryml::NodeRef Array;

    ryml::NodeRef Index; // Index in the material info array
    ryml::NodeRef TextureCoordU;
    ryml::NodeRef TextureCoordV;
    ryml::NodeRef UnknownFloat1; // Unknown purpose float, ranges from 0 to 1.
    ryml::NodeRef UnknownFloat2; // Unknown purpose float, ranges from 0.2 to 1.63.
    materialinfo_yaml()
    {
        Root = root["MaterialInfo"];
        Root |= ryml::SEQ;
    }
};

struct AreaArray_yaml
{
    ryml::NodeRef Root;
    ryml::NodeRef Array;

    ryml::NodeRef Index; // Index in the area array
    ryml::NodeRef XPosition;
    ryml::NodeRef ZPosition;
    ryml::NodeRef AreaSize;
    ryml::NodeRef MinTerrainHeight;
    ryml::NodeRef MaxTerrainHeight;
    ryml::NodeRef MinWaterHeight;
    ryml::NodeRef MaxWaterHeight;
    ryml::NodeRef ExtraInfoElementCount;
    ryml::NodeRef Unknown2;
    ryml::NodeRef Unknown3;
    ryml::NodeRef ref_extra;
    AreaArray_yaml(unsigned int index)
    {
        Root = root["AreaArray"];
        Root |= ryml::SEQ;

        Array = Root[index];
        Array |= ryml::MAP;

        Index = Array["Index"];
        XPosition = Array["PositionX"];
        ZPosition = Array["PositionZ"];
        AreaSize = Array["AreaSize"];
        MinTerrainHeight = Array["MinTerrainHeight"];
        MaxTerrainHeight = Array["MaxTerrainHeight"];
        MinWaterHeight = Array["MinWaterHeight"];
        MaxWaterHeight = Array["MaxWaterHeight"];
        ExtraInfoElementCount = Array["ExtraInfoElementCount"];
        Unknown2 = Array["Unknown2"];
        Unknown3 = Array["Unknown3"];
        ref_extra = Array["ref_extra"];
    }
};

struct ExtraInfo_yaml
{
    ryml::NodeRef Root;
    ryml::NodeRef Array;

    ryml::NodeRef ExtraInfoLength;
    ryml::NodeRef ExtraUnknown1;
    ryml::NodeRef ExtraUnknown2; // 0 = Grass, 1 = Water
    ryml::NodeRef ExtraUnknown3;
    ryml::NodeRef ExtraUnknown4;
};

struct TSCB_Header
{
	char ID[4];				 // This will always be 54 53 43 42 ("TSCB")
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
    float ZPosition;
    float AreaSize;         // Length & width of the area
    float MinTerrainHeight; // Range from 0 to 1
    float MaxTerrainHeight; // Range from 0 to 1
    float MinWaterHeight;   // Range from 0 to 1
    float MaxWaterHeight;   // Range from 0 to 1
    unsigned int ExtraInfoElementCount; // The number of elements in the extra info array
    unsigned int FileBaseOffset; // Relative offset to file base name string
    unsigned int Unknown2;  // Usually 0.
    unsigned int Unknown3;  // Usually 0.
    unsigned int ref_extra; // Indicates if there will be an extra_info_array attached.
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
