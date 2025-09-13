#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "crypto.h"

// Struktur untuk Proof of Work //
typedef struct {
    uint32_t difficulty;   // berapa banyak leading zeros yg dibutuhin
    uint64_t nonce;        // nilai yg dicoba2
    char hash[SHA256_HEX_LENGTH + 1]; // hasil hash terakhir (hex)
} ProofOfWork;

// Inisialisasi PoW dengan difficulty tertentu //
void pow_init(ProofOfWork *pow, uint32_t difficulty);

// Jalankan mining (cari hash yang valid) //
// data = input yg di-hash (misal block header)
// hasil nonce dan hash valid disimpan di ProofOfWork
bool pow_mine(ProofOfWork *pow, const uint8_t *data, size_t len);

// Verifikasi hasil PoW (apakah hash sesuai difficulty) //
bool pow_verify(const char hash[SHA256_HEX_LENGTH + 1], uint32_t difficulty);

#endif
