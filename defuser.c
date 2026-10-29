#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    // TODO: 根据 phase_01 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t lo = (uint32_t)challenge, hi = (uint32_t)(challenge >> 32), mid = (uint32_t)(challenge >> 16);
    uint32_t t = (5u * hi) ^ (lo + 0xaa85u), a = (lo + 2u * mid) ^ 0x4c35u;
    values[0] = 5u * (a - t) + 0xe82u;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 6;
    (void)challenge;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    values[0] = (uint32_t)((challenge >> 35) & 0xffu);
    for (unsigned i = 1; i < 6; ++i) values[i] = 5u * values[i - 1] + ((uint32_t)((challenge >> (19u + 3u * i)) & 0x3fu) ^ (i + 5u));
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;
    // TODO: 根据 phase_03 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t x = (uint32_t)(challenge >> 3) ^ (uint32_t)(challenge >> 32); 
    values[0] = (uint32_t)challenge & 7u;
    switch (values[0]) {
        case 0: values[1] = (2u * x) ^ 0xe209u; break;
        case 1: values[1] = 5u * x + 0x97d5u; break;
        case 2: values[1] = 5u * x + 0x3307cu; break;
        case 3: values[1] = (2u * x) ^ 0x2368u; break;
        case 4: values[1] = 5u * x + 0x21cc3u; break;
        case 5: values[1] = 5u * x + 0xaef1u; break;
        case 6: values[1] = (x ^ 0xfe64u) + (x >> 1); break;
        case 7: values[1] = (x ^ 0x5cdu) + (x >> 2); break;
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t target = (uint32_t)challenge & 0x7fu, salt = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32), lo = 0, hi = 127;
    uint32_t keys[8], branches[8]; 
    unsigned depth = 0;
    for (;;) {
        uint32_t mid = lo + ((hi - lo) >> 1); 
        if (target == mid) { values[0] = (mid + 0xc9u) ^ salt; break; }
        keys[depth] = mid + salt; 
        branches[depth++] = target < mid ? 0x6u : 0x17u; 
        if (target < mid) hi = mid - 1u; 
        else lo = mid + 1u;
    }
    while (depth) { --depth; values[0] = (3u * values[0]) ^ keys[depth] ^ branches[depth]; }
    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    static const struct { 
        uint32_t value, tag; unsigned next; 
    } nodes[12] = {
        {0x7ac4u,0x646u,6},
        {0x3697u,0x820u,9},
        {0x3531u,0x15au,12},
        {0x8f61u,0x445u,10},
        {0x3338u,0x331u,11},
        {0x209du,0x977u,8},
        {0x27dbu,0x272u,5},
        {0x4024u,0x516u,1},
        {0xfb56u,0xb71u,3},
        {0xb000u,0xa75u,4},
        {0xcb0bu,0x1au,2},
        {0x414eu,0x72eu,0}
    };
    static const unsigned starts[8] = {7,1,9,4,11,0,6,5};
    unsigned index = starts[challenge & 7u], hops = (unsigned)((challenge >> 3) & 3u); 
    values[0] = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32) ^ 0x3a1au;
    for (unsigned step = 0; step <= hops; ++step) { 
        values[0] = (values[0] ^ nodes[index].value) + nodes[index].tag; 
        values[1] = nodes[index].tag; 
        if (step < hops) index = nodes[index].next;
    }
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    static const unsigned operations[8] = {5,4,5,1,0,3,1,2};
    unsigned indices[2] = {(unsigned)(challenge & 7u), (unsigned)((challenge >> 3) & 7u)}; 
    uint32_t x = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);

    for (unsigned i = 0; i < 2; ++i) {
        switch (operations[indices[i]]) {
            case 0: x = (x ^ 0xe039u) + 8u * x; break;
            case 1: x = ((x << 1) | (x >> 31)) ^ 0x69cu; break;
            case 2: x = (x >> 3) | (x << 29); break;
            case 3: x = (8u * x + 0x13f0u) ^ (7u * x); break;
            case 4: x = (x + 0xfe69u) ^ (x >> 5); break;
            case 5: x = (x ^ (x >> 2)) + 0x3a6au; break;
        }
        if (i == 0) x ^= 0x4d0eu;
    }
    values[0] = x + 0x2b91u;
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    if (out == NULL || cap == 0) return 1;
    uint32_t p = (uint32_t)challenge ^ (uint32_t)(challenge >> 32), q = (uint32_t)context ^ (uint32_t)(context >> 32);
    uint32_t a = (uint32_t)context & 3u, b = ((3u * (p ^ q) + 0x9118u) ^ 0xeb10u) ^ (a * 0x9118u) ^ (uint32_t)(context >> 16);
    uint32_t c = b ^ (q + a + 0xeb10u + (p >> 3));
    int n = snprintf(out, cap, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOPEN%" PRIu32 " %" PRIu32 " %" PRIu32, a, b, c); 
    if (n < 0 || (size_t)n >= cap) { out[0] = '\0'; return 1; }
    return 0;
}
