#pragma once

#include <stddef.h>

#include "pvf/pvf.h"

typedef struct SimAppConfig {
    const char* pvf_path;
    const char* imagepacks2_dir;
    const char* out_dir;
} SimAppConfig;

typedef struct SimApp {
    SimAppConfig cfg;
    PackSetInternal pvf;
    int pvf_open;
} SimApp;

int SimApp_Init(SimApp* app, const SimAppConfig* cfg);
void SimApp_Shutdown(SimApp* app);

int SimApp_ListAni(SimApp* app, const char* contains, int limit);
int SimApp_DumpPvfFile(SimApp* app, const char* path_in_pvf, size_t bytes);

int SimApp_DecodeAniToPpm(SimApp* app, const char* ani_path_in_pvf, int max_frames_per_action);

/* Batch processing */
int SimApp_BatchDecodeAni(SimApp* app, const char* filter, int max_files, int max_frames_per_action);
