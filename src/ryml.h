#pragma once
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void ryml_emit(FILE* file);
void ryml_set_matinfo_data(struct MaterialInfoData matinfodata, int idx);
void ryml_set_header_data(struct TSCB_Header header_data);

#ifdef __cplusplus
}
#endif
