#include "../include/network.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Jadi disini fungsi nya melakukan //
// Mengecek apakah hash nya ada punya jumlah 0 di depan //
// Sesuai dengan dengan difficulty yang diminta //
static bool has_leading_zeros(const char *hash, uint32_t difficulty) {
    for (uint32_t i = 0; i < difficulty; i++) {
        if (hash[i] != '0') return false;
    }
    return true;
}

// Inisialisasi proof of work atau PoW dengan difficulty tertentu //
// Ini berguna untuk ke tahap PoW mine atau kita sebut miner nya PoW //
void pow_init(ProofOfWork *pow, uint32_t difficulty) {
    if (!pow) return;
    pow->difficulty = difficulty;
    pow->nonce = 0;
    memset(pow->hash, 0, sizeof(pow->hash));
}

// Fungsi ini kita akan melakukan mining untuk jaringan konsensus nya //
bool pow_mine(ProofOfWork *pow, const uint8_t *data, size_t len) {
    if (!pow || !data) return false;
    uint8_t buffer[1024];  

    while (1) {
        size_t buf_len = 0;
        memcpy(buffer, data, len);
        buf_len = len;
        memcpy(buffer + buf_len, &pow->nonce, sizeof(pow->nonce));
        buf_len += sizeof(pow->nonce);
        sha256_hex(buffer, buf_len, pow->hash);

        if (has_leading_zeros(pow->hash, pow->difficulty)) {
            return true; 
        }

        pow->nonce++;
        if (pow->nonce == UINT64_MAX) {
            return false; 
        }
    }
}

// Verifikasi untuk tugas PoW berdasarkan difficulty nya //
bool pow_verify(const char hash[SHA256_HEX_LENGTH + 1], uint32_t difficulty) {
    if (!hash) return false;
    return has_leading_zeros(hash, difficulty);
}
