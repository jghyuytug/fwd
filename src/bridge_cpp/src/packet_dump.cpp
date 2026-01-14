/*
 * packet_dump.cpp - packet_dump.h 全局状态定义
 */

#include "../include/packet_dump.h"

FILE* g_packet_dump_file = NULL;
int g_packet_dump_enabled = 0;
uint32_t g_packet_dump_count = 0;

