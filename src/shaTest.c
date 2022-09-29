#include <stdio.h>
#include <stdlib.h>

#include <openssl/sha.h>

#include <string.h>



int main() {
    uint8_t hash[SHA256_DIGEST_LENGTH];
    uint8_t tobeHASH[8] = {0};

    printf("Input message: ");
    scanf("%hhu", tobeHASH);

    SHA256(tobeHASH, sizeof(uint64_t), hash);
    
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
        printf("%02x", hash[i]);
    printf("\n");

    for (int i = 0; i < 8; ++i)
        printf("%02x", tobeHASH[i]);
    printf("\n");

    return 0;
}
