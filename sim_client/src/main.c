#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sim_app.h"

static void print_usage(const char* exe)
{
    printf("Sim Client (offline)\n");
    printf("\n");
    printf("Usage:\n");
    printf("  %s decode-ani <Script.pvf> <ImagePacks2_dir> <ani_path_in_pvf> [frames] [out_dir]\n", exe);
    printf("  %s batch-ani  <Script.pvf> <ImagePacks2_dir> [filter] [max_files] [frames] [out_dir]\n", exe);
    printf("  %s list-ani   <Script.pvf> [contains] [limit]\n", exe);
    printf("  %s dump-pvf   <Script.pvf> <path_in_pvf> [bytes]\n", exe);
    printf("\n");
    printf("Example:\n");
    printf("  %s decode-ani C:/path/Script.pvf C:/path/imagepacks2 creature/common/animation/levelup_dodgeunder.ani 5 out\n", exe);
    printf("  %s batch-ani  C:/path/Script.pvf C:/path/imagepacks2 swordman 20 3 out\n", exe);
}

static int cmd_decode_ani(int argc, char** argv)
{
    if (argc < 5) return -1;

    SimAppConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.pvf_path = argv[2];
    cfg.imagepacks2_dir = argv[3];
    cfg.out_dir = (argc >= 7) ? argv[6] : "out";

    const char* ani = argv[4];
    int frames = (argc >= 6) ? atoi(argv[5]) : 5;

    SimApp app;
    int r = SimApp_Init(&app, &cfg);
    if (r != 0) {
        fprintf(stderr, "Init failed (%d)\n", r);
        return 2;
    }
    int decoded = SimApp_DecodeAniToPpm(&app, ani, frames);
    SimApp_Shutdown(&app);
    if (decoded < 0) return 3;
    printf("Decoded frames: %d\n", decoded);
    return 0;
}

static int cmd_list_ani(int argc, char** argv)
{
    if (argc < 3) return -1;
    const char* pvf = argv[2];
    const char* contains = (argc >= 4) ? argv[3] : "";
    int limit = (argc >= 5) ? atoi(argv[4]) : 50;

    SimAppConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.pvf_path = pvf;
    cfg.imagepacks2_dir = ""; /* unused */
    cfg.out_dir = "out";

    SimApp app;
    int r = SimApp_Init(&app, &cfg);
    if (r != 0) {
        fprintf(stderr, "Init failed (%d)\n", r);
        return 2;
    }
    int printed = SimApp_ListAni(&app, contains, limit);
    SimApp_Shutdown(&app);
    return printed < 0 ? 3 : 0;
}

static int cmd_dump_pvf(int argc, char** argv)
{
    if (argc < 4) return -1;
    const char* pvf = argv[2];
    const char* path = argv[3];
    size_t bytes = (argc >= 5) ? (size_t)strtoul(argv[4], NULL, 10) : 256;

    SimAppConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.pvf_path = pvf;
    cfg.imagepacks2_dir = ""; /* unused */
    cfg.out_dir = "out";

    SimApp app;
    int r = SimApp_Init(&app, &cfg);
    if (r != 0) {
        fprintf(stderr, "Init failed (%d)\n", r);
        return 2;
    }
    int rc = SimApp_DumpPvfFile(&app, path, bytes);
    SimApp_Shutdown(&app);
    return rc == 0 ? 0 : 3;
}

static int cmd_batch_ani(int argc, char** argv)
{
    if (argc < 4) return -1;

    SimAppConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.pvf_path = argv[2];
    cfg.imagepacks2_dir = argv[3];
    cfg.out_dir = (argc >= 8) ? argv[7] : "out";

    const char* filter = (argc >= 5) ? argv[4] : "";
    int max_files = (argc >= 6) ? atoi(argv[5]) : 10;
    int frames = (argc >= 7) ? atoi(argv[6]) : 3;

    SimApp app;
    int r = SimApp_Init(&app, &cfg);
    if (r != 0) {
        fprintf(stderr, "Init failed (%d)\n", r);
        return 2;
    }
    int decoded = SimApp_BatchDecodeAni(&app, filter, max_files, frames);
    SimApp_Shutdown(&app);
    if (decoded < 0) return 3;
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "decode-ani") == 0) {
        int rc = cmd_decode_ani(argc, argv);
        if (rc == -1) print_usage(argv[0]);
        return rc == -1 ? 1 : rc;
    }
    if (strcmp(argv[1], "list-ani") == 0) {
        int rc = cmd_list_ani(argc, argv);
        if (rc == -1) print_usage(argv[0]);
        return rc == -1 ? 1 : rc;
    }
    if (strcmp(argv[1], "dump-pvf") == 0) {
        int rc = cmd_dump_pvf(argc, argv);
        if (rc == -1) print_usage(argv[0]);
        return rc == -1 ? 1 : rc;
    }
    if (strcmp(argv[1], "batch-ani") == 0) {
        int rc = cmd_batch_ani(argc, argv);
        if (rc == -1) print_usage(argv[0]);
        return rc == -1 ? 1 : rc;
    }

    print_usage(argv[0]);
    return 1;
}
