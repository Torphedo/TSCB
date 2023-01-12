#include "yaml.h"
#include "structures.h"
extern "C" {
    #include "endian.h"
}

#define RYML_SINGLE_HDR_DEFINE_NOW
#include "vendor/rapidyaml-0.5.0.hpp"

struct header_yaml
{
    ryml::NodeRef Root;
    ryml::NodeRef WorldScale;
    ryml::NodeRef TerrainHeightScale;
    ryml::NodeRef TileSize;
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
};

ryml::Tree yaml_tree;
ryml::NodeRef root = yaml_tree.rootref();

int DumpTSCBToYaml(FILE* TSCB_file, FILE* yamlout)
{
    TSCB_Header header = {0};

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
    header_yaml.Root = root["Header"];
    header_yaml.Root |= ryml::MAP;
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

        materialinfo.Root = root["MaterialInfo"];
        materialinfo.Root |= ryml::SEQ;
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

        AreaArray_yaml area;

        area.Root = root["AreaArray"];
        area.Root |= ryml::SEQ;

        area.Array = area.Root[i];
        area.Array |= ryml::MAP;

        area.Index = area.Array["Index"];
        area.XPosition = area.Array["PositionX"];
        area.ZPosition = area.Array["PositionZ"];
        area.AreaSize = area.Array["AreaSize"];
        area.MinTerrainHeight = area.Array["MinTerrainHeight"];
        area.MaxTerrainHeight = area.Array["MaxTerrainHeight"];
        area.MinWaterHeight = area.Array["MinWaterHeight"];
        area.MaxWaterHeight = area.Array["MaxWaterHeight"];
        area.ExtraInfoElementCount = area.Array["ExtraInfoElementCount"];

        area.Index << i;
        area.XPosition << AreaArray.XPosition;
        area.ZPosition << AreaArray.ZPosition;
        area.AreaSize << AreaArray.AreaSize;
        area.MinTerrainHeight << AreaArray.MinTerrainHeight;
        area.MaxTerrainHeight << AreaArray.MaxTerrainHeight;
        area.MinWaterHeight << AreaArray.MinWaterHeight;
        area.MaxWaterHeight << AreaArray.MaxWaterHeight;
        area.ExtraInfoElementCount << AreaArray.ExtraInfoElementCount;


        if (AreaArray.ExtraInfoElementCount != 0)
        {
            // Extra Info table

            ryml::NodeRef Root = area.Root[i]; // Set the root of ExtraInfo to be in the root of the area array

            ryml::NodeRef GrassOrWater = Root["GrassOrWater"]; // Set up ExtraInfo array
            GrassOrWater |= ryml::SEQ;

            unsigned int ExtraInfoLength; // Number of values in the array

            // This data is read just to advance the file pointer, it can be calculated at save time.
            fread(&ExtraInfoLength, sizeof(unsigned int), 1, TSCB_file);
            if (AreaArray.ExtraInfoElementCount == 2)
            {
                unsigned int HeaderUnknown;   // Only present if ExtraInfoElementCount = 2
                fread(&HeaderUnknown, sizeof(unsigned int), 1, TSCB_file);
                SwapEndianUInt(&HeaderUnknown); // Not printed to YAML, we can insert this at save time
            }

            for (int j = 0; j < AreaArray.ExtraInfoElementCount; j++)
            {
                ExtraAreaArray ExtraInfo = {0};
                fread(&ExtraInfo, sizeof(unsigned int) * 4, 1, TSCB_file);
                SwapEndianUInt(&ExtraInfo.ExtraUnknown1); // Always 3.
                SwapEndianUInt(&ExtraInfo.GrassOrWater);  // Setting this to 0 = Grass, 1 = Water
                SwapEndianUInt(&ExtraInfo.ExtraUnknown3); // Always 1.
                SwapEndianUInt(&ExtraInfo.ExtraUnknown4); // Always 0.


                GrassOrWater[j] << ExtraInfo.GrassOrWater;
            }
        }
    }
    ryml::emit(yaml_tree, yamlout); // Writes the YAML tree to a file
    return 0;
}

int ReadYaml()
{
    FILE* yaml_in = fopen("MainField.yaml", "rb"); // Open YAML file

    fseek(yaml_in, 0L, SEEK_END); // Jump the file pointer to the end of the file
    unsigned long size = ftell(yaml_in); // Get the position of the pointer (filesize)
    rewind(yaml_in); // Jump the file pointer back to the start

    auto* yamlbuf = new char[size];
    fread(yamlbuf, size, 1, yaml_in);

    ryml::Tree tree = ryml::parse_in_arena(c4::to_csubstr(yamlbuf));

    float WorldScale = 0.0;
    ryml::NodeRef Header = tree.rootref()["Header"];
    Header["WorldScale"] >> WorldScale;

    fclose(yaml_in);
    return 0;
}
