#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include "crypto.h"
#include "network.h"
#include "transaction.h"   
#include "utils.h"

typedef struct Block {
    uint64_t index;               // posisi block pada chain //
    uint64_t timestamp;           // unix epoch seconds //
    uint64_t nonce;               // nonce untuk PoW //
    ProofOfWork pow;              // PoW data (nonce, hash, difficulty) //
    char *prev_hash;              // hex string (SHA256_HEX_LEN) dari block sebelumnya //
    char *hash;                   // hex string (SHA256_HEX_LEN) dari block ini //
    char *data;                   // payload (string) //

    Transaction **transactions;   // array pointer ke transaksi //
    size_t tx_count;              // jumlah transaksi dalam block //
} Block;

// Buat block baru (meng-copy strings) //
Block *block_create(uint64_t index, uint64_t timestamp, const char *prev_hash, const char *data);

// Free block dan semua alokasinya //
void block_free(Block *b);

// Hitung hash block berdasarkan (index,timestamp,nonce,prev_hash,data) //
int block_compute_hash(const Block *b, char out_hash[SHA256_HEX_LENGTH + 1]);

// Serialize block ke string (untuk contoh/logging). Caller harus free() hasilnya. //
char *block_serialize(const Block *b);

// Melakukan proof of work untuk jaringan blockchain yang kita pakai //
int block_proof_of_work(Block *b);

// Validasi block: hash recompute cocok + prev_hash match (jika prev_hash != NULL) + difficulty check //
int block_is_valid(const Block *b, const char *expected_prev_hash, unsigned int difficulty);

// Simpan block ke file JSON //
int save_block_to_json(const Block *b, const char *filename);

// Load block dari file JSON //
Block *load_block_from_json(const char *filename);

// Tambahkan transaksi ke block //
int block_add_transaction(Block *b, Transaction *tx);

// Serialize semua transaksi di block (untuk logging/debug) //
char *block_serialize_transactions(const Block *b);

// Validasi semua transaksi di block //
int block_validate_transactions(const Block *b);

// Convert timestamp block ke string readable (misal untuk print/logging) //
void block_timestamp_to_str(const Block *b, char *buffer, size_t size);

// Print debug block lengkap dengan transaksi //
void block_print_debug(const Block *b);

#endif
