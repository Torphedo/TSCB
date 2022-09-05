#define RYML_SINGLE_HDR_DEFINE_NOW
#include "vendor/rapidyaml-0.4.1.hpp"

#include <iostream>
#include "ryml.h"


ryml::Tree globaltree;
ryml::NodeRef root;

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
    ryml::NodeRef Index;
    ryml::NodeRef TextureCoordU;
    ryml::NodeRef TextureCoordV;
    materialinfo_yaml()
    {
        Root = root["MaterialInfo"];
        Root |= ryml::SEQ;
    }
};

void ryml_set_header_data(struct TSCB_Header header_data)
{
    root = globaltree.rootref();
    root |= ryml::MAP;

    struct header_yaml header;
    header.WorldScale = header.Root["WorldScale"];
    header.TerrainHeightScale = header.Root["TerrainHeightScale"];
    header.TileSize = header.Root["TileSize"];

    header.WorldScale << header_data.WorldScale;
    header.TerrainHeightScale << header_data.TerrainHeightScale;
    header.TileSize << header_data.TileSize;
}

void ryml_set_matinfo_data(struct MaterialInfoData matinfodata, int idx)
{
    root = globaltree.rootref();
    root |= ryml::MAP;

    struct materialinfo_yaml materialinfo;

    materialinfo.Array = materialinfo.Root[idx];
    materialinfo.Array |= ryml::MAP;
    materialinfo.Index = materialinfo.Array["Index"];
    materialinfo.TextureCoordU = materialinfo.Array["TextureCoordU"];
    materialinfo.TextureCoordV = materialinfo.Array["TextureCoordV"];

    materialinfo.Index << idx;
    materialinfo.TextureCoordU << matinfodata.Texture_U;
    materialinfo.TextureCoordV << matinfodata.Texture_V;
}

void ryml_emit(FILE* file)
{
    ryml::emit(globaltree, file); // Replace file with stdout to print to console
}
