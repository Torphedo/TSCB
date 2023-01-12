#include <cstdio>
#include "yaml.h"

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        printf("No file specified.\n");
        return -1;
    }

    // argv[1] = "bin/MainField_no_water.tscb";

    FILE* TSCB_file = fopen(argv[1], "rb");
    if (TSCB_file == nullptr)
    {
        printf("Couldn't open file!\n");
        return -1;
    }

    FILE* yamlout = fopen("output.yaml", "wb");
    DumpTSCBToYaml(TSCB_file, yamlout);
    fclose(yamlout);
    fclose(TSCB_file);

	return 0;
}
