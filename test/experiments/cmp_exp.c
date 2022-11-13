#define N 10000000

#include <string.h>
#include <stdio.h>
#include <time.h>
#include "../../src/os-challenge-util.h"



int main() {
    uint8_t correct_hash[SHA256_DIGEST_LENGTH];
    response_t res;
    res.num = N;
    SHA256(res.bytes, sizeof(uint64_t), correct_hash);
    res.num = 22;
    SHA256(res.bytes, sizeof(uint64_t), correct_hash);
    uint8_t guess_hash[SHA256_DIGEST_LENGTH];

    clock_t start, end;
    

    // for-loop
    start = clock();
    for (res.num = 1; res.num <= N; ++res.num) {
        SHA256(res.bytes, sizeof(uint64_t), guess_hash);
        for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
            if (correct_hash[j] != guess_hash[j])
                break;
        }
    }
    end = clock();
    printf("For-loop: %f ms\n", (end - start) * 1000.0 / CLOCKS_PER_SEC);

    // // memcmp
    // start = clock();
    // for (res.num = 1; res.num <= N; ++res.num) {
    //     SHA256(res.bytes, sizeof(uint64_t), guess_hash);
    //     if (memcmp(correct_hash, guess_hash, SHA256_DIGEST_LENGTH) == 0)
    //         break;
    // }
    // end = clock(); 
    // printf("memcmp: %f ms\n", (end - start) * 1000.0 / CLOCKS_PER_SEC);



    // // Inline while loop
    // start = clock();
    // int i = 0; 
    // for (res.num = 1; res.num <= N; ++res.num) {
    //     SHA256(res.bytes, sizeof(uint64_t), guess_hash);
    //     while (i < SHA256_DIGEST_LENGTH) {
    //         if (correct_hash[i] != guess_hash[i])
    //             break;
    //         ++i;    
    //     }
    //     i = 0;
    // }
    // end = clock();
    // printf("While-loop: %f ms\n", (end - start) * 1000.0 / CLOCKS_PER_SEC);

    // // memcmp with quick-check lazy eval
    // start = clock();
    // for (res.num = 1; res.num <= N; ++res.num) {
    //     SHA256(res.bytes, sizeof(uint64_t), guess_hash);
    //     if (correct_hash[0] == guess_hash[0] && memcmp(correct_hash, guess_hash, SHA256_DIGEST_LENGTH) == 0)
    //         break;
    // }
    // end = clock(); 
    // printf("memcmp with lazy eval: %f ms\n", (end - start) * 1000.0 / CLOCKS_PER_SEC);

    // XOR-method
    start = clock();
    for (res.num = 1; res.num <= N; ++res.num) {
        SHA256(res.bytes, sizeof(uint64_t), guess_hash);
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            if (correct_hash[i] ^ guess_hash[i])
                break;
        }
    }
    end = clock(); 
    printf("XOR: %f ms\n", (end - start) * 1000.0 / CLOCKS_PER_SEC);

    // Typecast-long-method
    start = clock();
    for (res.num = 1; res.num <= N; ++res.num) {
        SHA256(res.bytes, sizeof(uint64_t), guess_hash);
        for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
            if ((int)correct_hash[j] - (int)guess_hash[j])
                break;
        }
    }
    end = clock(); 
    printf("Typecast: %f ms\n", (end - start) * 1000.0 / CLOCKS_PER_SEC);

    // ASM
    start = clock();
    for (res.num = 1; res.num <= N; ++res.num) {
        SHA256(res.bytes, sizeof(uint64_t), guess_hash);
        for (int j = 0; j < SHA256_DIGEST_LENGTH; ++j) {
            asm (
                "xorl %%eax %%ebx" 
                "jne %l[notEqual]"
                : /* No outputs */
                : "a" (correct_hash[j]), "b" (guess_hash[j])
                : "notEqual"
            );
        }
        notEqual:
    }
    end = clock(); 
    printf("ASM: %f ms\n", (end - start) * 1000.0 / CLOCKS_PER_SEC);

}