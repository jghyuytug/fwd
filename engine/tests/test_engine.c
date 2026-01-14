/**
 * @file test_engine.c
 * @brief Engine core module tests
 */

#include "../engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Test counters */
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        printf("  Testing %s... ", #name); \
        tests_run++; \
        if (test_##name()) { \
            printf("PASS\n"); \
            tests_passed++; \
        } else { \
            printf("FAIL\n"); \
        } \
    } while(0)

/* ============= Ring Buffer Tests ============= */

static int test_ring_buffer_basic(void)
{
    RingBuffer buf;
    u8 data[] = {1, 2, 3, 4, 5};
    u8 read[5];
    u32 written, read_count;

    if (ring_buffer_init(&buf, 64) != ENGINE_OK) return 0;

    written = ring_buffer_write(&buf, data, 5);
    if (written != 5) { ring_buffer_cleanup(&buf); return 0; }

    if (ring_buffer_readable(&buf) != 5) { ring_buffer_cleanup(&buf); return 0; }

    read_count = ring_buffer_read(&buf, read, 5);
    if (read_count != 5) { ring_buffer_cleanup(&buf); return 0; }

    if (memcmp(data, read, 5) != 0) { ring_buffer_cleanup(&buf); return 0; }

    if (!ring_buffer_is_empty(&buf)) { ring_buffer_cleanup(&buf); return 0; }

    ring_buffer_cleanup(&buf);
    return 1;
}

static int test_ring_buffer_wrap(void)
{
    RingBuffer buf;
    u8 data[32];
    u8 read[32];
    int i;

    if (ring_buffer_init(&buf, 16) != ENGINE_OK) return 0;

    for (i = 0; i < 32; i++) data[i] = (u8)i;

    /* Write 10 bytes */
    ring_buffer_write(&buf, data, 10);
    /* Read 8 bytes */
    ring_buffer_read(&buf, read, 8);
    /* Write 10 more (should wrap) */
    ring_buffer_write(&buf, data + 10, 10);
    /* Should have 12 bytes now */
    if (ring_buffer_readable(&buf) != 12) { ring_buffer_cleanup(&buf); return 0; }

    ring_buffer_cleanup(&buf);
    return 1;
}

/* ============= Event Bus Tests ============= */

static int event_handler_called = 0;
static EventType last_event_type = EVENT_NONE;

static void test_event_handler(EventBus* bus, const EventBase* event, void* user_data)
{
    (void)bus;
    (void)user_data;
    event_handler_called++;
    last_event_type = event->type;
}

static int test_event_bus_basic(void)
{
    EventBus bus;
    int handler_id;

    if (event_bus_init(&bus) != ENGINE_OK) return 0;

    event_handler_called = 0;
    last_event_type = EVENT_NONE;

    handler_id = event_subscribe(&bus, EVENT_PLAYER_LOGIN, test_event_handler, NULL);
    if (handler_id < 0) { event_bus_cleanup(&bus); return 0; }

    event_publish_simple(&bus, EVENT_PLAYER_LOGIN, 1, 0);

    if (event_handler_called != 1) { event_bus_cleanup(&bus); return 0; }
    if (last_event_type != EVENT_PLAYER_LOGIN) { event_bus_cleanup(&bus); return 0; }

    /* Unrelated event should not trigger */
    event_handler_called = 0;
    event_publish_simple(&bus, EVENT_PLAYER_LOGOUT, 1, 0);
    if (event_handler_called != 0) { event_bus_cleanup(&bus); return 0; }

    event_bus_cleanup(&bus);
    return 1;
}

static int test_event_bus_queue(void)
{
    EventBus bus;

    if (event_bus_init(&bus) != ENGINE_OK) return 0;

    event_handler_called = 0;
    event_subscribe(&bus, EVENT_NONE, test_event_handler, NULL);  /* All events */

    /* Queue events */
    event_queue_simple(&bus, EVENT_PLAYER_LOGIN, 1, 0);
    event_queue_simple(&bus, EVENT_PLAYER_LOGOUT, 1, 0);

    if (event_queue_count(&bus) != 2) { event_bus_cleanup(&bus); return 0; }

    /* Dispatch */
    event_dispatch_queued(&bus, 0);

    if (event_handler_called != 2) { event_bus_cleanup(&bus); return 0; }

    event_bus_cleanup(&bus);
    return 1;
}

/* ============= Timer Wheel Tests ============= */

static int timer_callback_count = 0;

static void test_timer_callback(TimerWheel* wheel, u32 timer_id, void* user_data)
{
    (void)wheel;
    (void)timer_id;
    (void)user_data;
    timer_callback_count++;
}

static int test_timer_basic(void)
{
    TimerWheel wheel;
    u32 timer_id;

    if (timer_wheel_init(&wheel) != ENGINE_OK) return 0;

    timer_callback_count = 0;

    /* Initialize with base time */
    timer_wheel_update(&wheel, 1000);

    timer_id = timer_add(&wheel, 50, test_timer_callback, NULL);  /* 50ms */
    if (timer_id == 0) { timer_wheel_cleanup(&wheel); return 0; }

    if (!timer_is_active(&wheel, timer_id)) { timer_wheel_cleanup(&wheel); return 0; }

    /* Update with 100ms elapsed - should trigger timer */
    timer_wheel_update(&wheel, 1100);

    if (timer_callback_count != 1) { timer_wheel_cleanup(&wheel); return 0; }
    if (timer_is_active(&wheel, timer_id)) { timer_wheel_cleanup(&wheel); return 0; }

    timer_wheel_cleanup(&wheel);
    return 1;
}

static int test_timer_repeat(void)
{
    TimerWheel wheel;
    u32 timer_id;

    if (timer_wheel_init(&wheel) != ENGINE_OK) return 0;

    timer_callback_count = 0;

    /* Initialize with base time */
    timer_wheel_update(&wheel, 1000);

    timer_id = timer_add_repeat(&wheel, 20, 20, test_timer_callback, NULL);  /* Every 20ms */
    if (timer_id == 0) { timer_wheel_cleanup(&wheel); return 0; }

    /* Update with 100ms elapsed - should fire multiple times */
    timer_wheel_update(&wheel, 1100);

    /* Should have fired at least twice (20, 40, 60, 80, 100) */
    if (timer_callback_count < 2) { timer_wheel_cleanup(&wheel); return 0; }

    /* Timer should still be active (repeating) */
    if (!timer_is_active(&wheel, timer_id)) { timer_wheel_cleanup(&wheel); return 0; }

    timer_wheel_cleanup(&wheel);
    return 1;
}

/* ============= Pool Allocator Tests ============= */

static int test_pool_basic(void)
{
    PoolAllocator pool;
    void* obj1;
    void* obj2;

    if (pool_init(&pool, 64, 10) != ENGINE_OK) return 0;

    obj1 = pool_alloc(&pool);
    if (!obj1) { pool_cleanup(&pool); return 0; }

    obj2 = pool_alloc(&pool);
    if (!obj2) { pool_cleanup(&pool); return 0; }

    if (pool_available(&pool) != 8) { pool_cleanup(&pool); return 0; }

    pool_free(&pool, obj1);
    if (pool_available(&pool) != 9) { pool_cleanup(&pool); return 0; }

    pool_cleanup(&pool);
    return 1;
}

static int test_pool_exhaust(void)
{
    PoolAllocator pool;
    void* objs[5];
    void* extra;
    int i;

    if (pool_init(&pool, 32, 5) != ENGINE_OK) return 0;

    for (i = 0; i < 5; i++) {
        objs[i] = pool_alloc(&pool);
        if (!objs[i]) { pool_cleanup(&pool); return 0; }
    }

    if (!pool_is_full(&pool)) { pool_cleanup(&pool); return 0; }

    extra = pool_alloc(&pool);
    if (extra != NULL) { pool_cleanup(&pool); return 0; }  /* Should fail */

    pool_cleanup(&pool);
    return 1;
}

/* ============= Config Loader Tests ============= */

static int test_config_memory(void)
{
    Config cfg;

    if (config_init(&cfg) != ENGINE_OK) return 0;

    config_set_string(&cfg, "server", "host", "127.0.0.1");
    config_set_int(&cfg, "server", "port", 8080);
    config_set_string(&cfg, NULL, "debug", "true");

    if (strcmp(config_get_string(&cfg, "server", "host", ""), "127.0.0.1") != 0) {
        config_cleanup(&cfg);
        return 0;
    }

    if (config_get_int(&cfg, "server", "port", 0) != 8080) {
        config_cleanup(&cfg);
        return 0;
    }

    if (!config_get_bool(&cfg, NULL, "debug", 0)) {
        config_cleanup(&cfg);
        return 0;
    }

    config_cleanup(&cfg);
    return 1;
}

/* ============= Main ============= */

int main(void)
{
    printf("\n=== Game Engine Test Suite ===\n");
    printf("Version: %s\n\n", engine_version());

    printf("[Ring Buffer]\n");
    TEST(ring_buffer_basic);
    TEST(ring_buffer_wrap);

    printf("\n[Event Bus]\n");
    TEST(event_bus_basic);
    TEST(event_bus_queue);

    printf("\n[Timer Wheel]\n");
    TEST(timer_basic);
    TEST(timer_repeat);

    printf("\n[Pool Allocator]\n");
    TEST(pool_basic);
    TEST(pool_exhaust);

    printf("\n[Config Loader]\n");
    TEST(config_memory);

    printf("\n=== Results: %d/%d tests passed ===\n\n",
           tests_passed, tests_run);

    return (tests_passed == tests_run) ? 0 : 1;
}
