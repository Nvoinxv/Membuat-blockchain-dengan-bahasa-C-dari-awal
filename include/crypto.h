#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include <stddef.h>

#define SHA256_DIGEST_LENGTH 32  // Bytes //
#define SHA256_BLOCK_SIZE 64     // Bytes //
#define SHA256_HEX_LENGTH (SHA256_DIGEST_LENGTH * 2)

typedef struct {
    uint32_t state[8];      // inisial langkah (a..h) //
    uint64_t bitlen;        // total angka proses bit //
    uint8_t data[SHA256_BLOCK_SIZE];
    size_t datalen;         // bytes berada di data [] //
} SHA256_CTX;

// Core API //
// Bagian init adalah buat ngereset atau buat wadah baru untuk memulai proses hasing 256 //
void sha256_init(SHA256_CTX *ctx);
// Masukin data untuk di proses hasing nya //
void sha256_update(SHA256_CTX *ctx, const uint8_t *data, size_t len);
// nutup proses hashing dan ngeluarin hasil akhirnya dalam bentuk hash biner //
void sha256_final(SHA256_CTX *ctx, uint8_t out_hash[SHA256_DIGEST_LENGTH]);
// langsung bikin hash dari data dan hasilnya dikasih dalam bentuk string heksadesimal //
void sha256_hex(const uint8_t *data, size_t len, char out_hex[SHA256_HEX_LENGTH + 1]);

#endif 
