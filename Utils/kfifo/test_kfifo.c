/*
 * test_kfifo.c
 * Comprehensive tests for kfifo (static + dynamic, element sizes, wrap,
 * out_linear_ptr, full/empty behaviour).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "kfifo.h"

static int failures = 0;

static void ok(const char *name)
{
    printf("[OK] %s\n", name);
}

static void fail(const char *name)
{
    printf("[FAIL] %s\n", name);
    failures++;
}

static void test_static_u8(void)
{
    unsigned char in[] = {1, 2, 3, 4, 5, 6};
    unsigned char out[6] = {0};
    unsigned int ret;

    DECLARE_KFIFO(fifo, unsigned char, 8);
    INIT_KFIFO(fifo);

    ret = kfifo_in(&fifo, in, sizeof(in));
    if (ret != sizeof(in)) { fail("static_u8: kfifo_in count"); return; }

    ret = kfifo_out(&fifo, out, sizeof(out));
    if (ret != sizeof(out)) { fail("static_u8: kfifo_out count"); return; }

    if (memcmp(in, out, sizeof(in)) != 0) { fail("static_u8: data mismatch"); return; }

    /* wrap-around test */
    unsigned char seq[7] = {10,11,12,13,14,15,16};
    unsigned char out2[7] = {0};

    /* write some, read some, then write crossing buffer end */
    ret = kfifo_in(&fifo, seq, 5);
    if (ret != 5) { fail("static_u8: pre-wrap in"); return; }
    ret = kfifo_out(&fifo, out2, 3);
    if (ret != 3) { fail("static_u8: pre-wrap out"); return; }
    ret = kfifo_in(&fifo, seq + 5, 4); /* this should wrap */
    if (ret != 4) { fail("static_u8: wrap in"); return; }

    /* drain remaining */
    unsigned char drain[6] = {0};
    ret = kfifo_out(&fifo, drain, 6);
    if (ret != 6) { fail("static_u8: drain out"); return; }

    ok("test_static_u8");
}

static void test_dynamic_u16(void)
{
    unsigned int ret;
    uint16_t in16[] = {1000, 2000, 3000, 4000};
    uint16_t out16[4] = {0};

    DECLARE_KFIFO_PTR(dfifo, uint16_t);
    ret = kfifo_alloc(&dfifo, 8);
    if (ret) { fail("dynamic_u16: alloc"); return; }

    ret = kfifo_in(&dfifo, in16, 4);
    if (ret != 4) { fail("dynamic_u16: in count"); kfifo_free(&dfifo); return; }

    ret = kfifo_out(&dfifo, out16, 4);
    if (ret != 4) { fail("dynamic_u16: out count"); kfifo_free(&dfifo); return; }

    if (memcmp(in16, out16, sizeof(in16)) != 0) { fail("dynamic_u16: data mismatch"); kfifo_free(&dfifo); return; }

    kfifo_free(&dfifo);
    ok("test_dynamic_u16");
}

static void test_out_linear_ptr(void)
{
    DECLARE_KFIFO(fifo, unsigned char, 8);
    INIT_KFIFO(fifo);

    unsigned char in[] = {20,21,22,23,24,25};
    unsigned char buf[6];
    unsigned int ret;

    ret = kfifo_in(&fifo, in, sizeof(in));
    if (ret != sizeof(in)) { fail("out_linear_ptr: in"); return; }

    unsigned char *ptr = NULL;
    unsigned int n = kfifo_out_linear_ptr(&fifo, &ptr, 8);
    if (n == 0 || ptr == NULL) { fail("out_linear_ptr: pointer/len"); return; }

    /* copy from ptr (linear portion) up to n elements */
    unsigned int copy = (n < sizeof(buf)) ? n : sizeof(buf);
    memcpy(buf, ptr, copy);

    /* compare first `copy` elements */
    if (memcmp(buf, in, copy) != 0) { fail("out_linear_ptr: data mismatch"); return; }

    /* finish by reading out using kfifo_out */
    unsigned char out[6];
    ret = kfifo_out(&fifo, out, sizeof(out));
    if (ret != sizeof(out)) { fail("out_linear_ptr: out count"); return; }
    if (memcmp(out, in, sizeof(in)) != 0) { fail("out_linear_ptr: out data mismatch"); return; }

    ok("test_out_linear_ptr");
}

static void test_full_empty(void)
{
    DECLARE_KFIFO(fifo, unsigned char, 4);
    INIT_KFIFO(fifo);

    unsigned char data[4] = {1,2,3,4};
    unsigned int ret;

    ret = kfifo_in(&fifo, data, 4);
    if (ret != 4) { fail("full_empty: fill count"); return; }

    if (!kfifo_is_full(&fifo)) { fail("full_empty: should be full"); return; }

    unsigned char out[4];
    ret = kfifo_out(&fifo, out, 4);
    if (ret != 4) { fail("full_empty: drain count"); return; }

    if (!kfifo_is_empty(&fifo)) { fail("full_empty: should be empty"); return; }

    ok("test_full_empty");
}

static void test_struct_frame(void)
{
    typedef struct {
        uint8_t sof;
        uint8_t len;
        uint8_t cmd;
        uint8_t payload[4];
        uint8_t crc;
    } frame_t;

    frame_t frames[3];
    for (int i = 0; i < 3; i++) {
        frames[i].sof = 0xAA;
        frames[i].len = (uint8_t)(sizeof(frames[i].payload) + 2); /* cmd + crc + payload */
        frames[i].cmd = (uint8_t)(0x10 + i);
        for (int j = 0; j < 4; j++)
            frames[i].payload[j] = (uint8_t)(i * 10 + j);
        uint8_t crc = 0;
        crc += frames[i].cmd;
        for (int j = 0; j < 4; j++)
            crc += frames[i].payload[j];
        frames[i].crc = crc;
    }

    DECLARE_KFIFO(fifo, frame_t, 8);
    INIT_KFIFO(fifo);

    unsigned int ret = kfifo_in(&fifo, frames, 3);
    if (ret != 3) { fail("struct_frame: in count"); return; }

    frame_t out[3];
    ret = kfifo_out(&fifo, out, 3);
    if (ret != 3) { fail("struct_frame: out count"); return; }

    if (memcmp(frames, out, sizeof(frames)) != 0) { fail("struct_frame: data mismatch"); return; }
    ok("test_struct_frame");
}

static void test_record_static(void)
{
    /* static record FIFO with 1-byte length field */
    STRUCT_KFIFO_REC_1(32) rfifo;
    INIT_KFIFO(rfifo);

    const unsigned char a[] = {1,2,3};
    const unsigned char b[] = {10,11,12,13,14};

    unsigned int ret;
    ret = kfifo_in(&rfifo, a, sizeof(a));
    if (ret != 3) { /* returns number of records written */ fail("record_static: in a"); return; }
    ret = kfifo_in(&rfifo, b, sizeof(b));
    if (ret != 5) { fail("record_static: in b"); return; }

    unsigned int len = kfifo_peek_len(&rfifo);
    if (len != sizeof(a)) { fail("record_static: peek_len a"); return; }

    unsigned char out1[8];
    ret = kfifo_out(&rfifo, out1, len);
    if (ret != len) { fail("record_static: out a count"); return; }
    if (memcmp(out1, a, len) != 0) { fail("record_static: out a data"); return; }

    len = kfifo_peek_len(&rfifo);
    if (len != sizeof(b)) { fail("record_static: peek_len b"); return; }
    unsigned char out2[16];
    ret = kfifo_out(&rfifo, out2, len);
    if (ret != len) { fail("record_static: out b count"); return; }
    if (memcmp(out2, b, len) != 0) { fail("record_static: out b data"); return; }

    ok("test_record_static");
}

static void test_record_dynamic(void)
{
    /* dynamic record FIFO (1-byte length) */
    struct kfifo_rec_ptr_1 drfifo;
    int err = kfifo_alloc(&drfifo, 32);
    if (err) { fail("record_dynamic: alloc"); return; }

    const unsigned char p1[1] = {0x55};
    const unsigned char p2[7] = {1,2,3,4,5,6,7};

    unsigned int ret;
    ret = kfifo_in(&drfifo, p1, sizeof(p1));
    if (ret != 1) { fail("record_dynamic: in p1"); kfifo_free(&drfifo); return; }
    ret = kfifo_in(&drfifo, p2, sizeof(p2));
    if (ret != 7) { fail("record_dynamic: in p2"); kfifo_free(&drfifo); return; }

    unsigned int len = kfifo_peek_len(&drfifo);
    if (len != sizeof(p1)) { fail("record_dynamic: peek_len p1"); kfifo_free(&drfifo); return; }

    unsigned char out1[8];
    ret = kfifo_out(&drfifo, out1, len);
    if (ret != len) { fail("record_dynamic: out p1 count"); kfifo_free(&drfifo); return; }
    if (memcmp(out1, p1, len) != 0) { fail("record_dynamic: out p1 data"); kfifo_free(&drfifo); return; }

    len = kfifo_peek_len(&drfifo);
    if (len != sizeof(p2)) { fail("record_dynamic: peek_len p2"); kfifo_free(&drfifo); return; }
    unsigned char out2[16];
    ret = kfifo_out(&drfifo, out2, len);
    if (ret != len) { fail("record_dynamic: out p2 count"); kfifo_free(&drfifo); return; }
    if (memcmp(out2, p2, len) != 0) { fail("record_dynamic: out p2 data"); kfifo_free(&drfifo); return; }

    kfifo_free(&drfifo);
    ok("test_record_dynamic");
}

static void test_struct_frame_r(void)
{
    /* Use record FIFO to store frame payload (cmd+payload+crc) as records */
    STRUCT_KFIFO_REC_1(32) rfifo;
    INIT_KFIFO(rfifo);

    typedef struct {
        uint8_t sof;
        uint8_t len;
        uint8_t cmd;
        uint8_t payload[4];
        uint8_t crc;
    } frame_t;

    frame_t frames[3];
    for (int i = 0; i < 3; i++) {
        frames[i].sof = 0xAA;
        frames[i].len = (uint8_t)(sizeof(frames[i].payload) + 2);
        frames[i].cmd = (uint8_t)(0x10 + i);
        for (int j = 0; j < 4; j++)
            frames[i].payload[j] = (uint8_t)(i * 10 + j);
        uint8_t crc = 0;
        crc += frames[i].cmd;
        for (int j = 0; j < 4; j++)
            crc += frames[i].payload[j];
        frames[i].crc = crc;
    }

    /* push payload-only (cmd + payload + crc) as records */
    for (int i = 0; i < 3; i++) {
        unsigned char rec[1 + 4 + 1];
        rec[0] = frames[i].cmd;
        memcpy(&rec[1], frames[i].payload, 4);
        rec[5] = frames[i].crc;
        unsigned int r = kfifo_in(&rfifo, rec, sizeof(rec));
        if (r != 6) { fail("struct_frame_r: in record"); return; }
    }

    /* read back using peek_len + out */
    for (int i = 0; i < 3; i++) {
        unsigned int len = kfifo_peek_len(&rfifo);
        if (len != 6) { fail("struct_frame_r: peek_len"); return; }
        unsigned char buf[6];
        unsigned int r = kfifo_out(&rfifo, buf, len);
        if (r != len) { fail("struct_frame_r: out count"); return; }
        if (buf[0] != frames[i].cmd) { fail("struct_frame_r: cmd mismatch"); return; }
        if (memcmp(&buf[1], frames[i].payload, 4) != 0) { fail("struct_frame_r: payload mismatch"); return; }
        if (buf[5] != frames[i].crc) { fail("struct_frame_r: crc mismatch"); return; }
    }

    ok("test_struct_frame_r");
}

int main(void)
{
    printf("Running kfifo tests...\n");

    test_static_u8();
    test_dynamic_u16();
    test_out_linear_ptr();
    test_full_empty();
    test_struct_frame();
    test_record_static();
    test_record_dynamic();
    test_struct_frame_r();

    if (failures == 0) {
        printf("All tests passed.\n");
        return 0;
    }
    else {
        printf("%d test(s) failed.\n", failures);
        return 2;
    }
}
