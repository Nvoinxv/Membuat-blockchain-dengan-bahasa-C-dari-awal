#include "../include/block.h"
#include "../include/crypto.h"
#include "../include/cJSON.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <errno.h>

// Kita akan membuat logika block //
// Disini block berfungsi sebagai transaksi atau jaringn blok pertama di blockchain //

// Disini kita akan membuat logic fungsi helper //
// Kita buat ini biar aman saja saat ada salinan string baru //
// Di bagian xstrdup itu bagian buat manajemen salinan string baru di heap //
static char *xstrdup(const char *s) {
    if (!s) return NULL;
    char *d = strdup(s);
    return d;
}

// Bagian build hash input //
// Tugas nya nyusun string gabungan (index, timestamp, nonce, prev_hash, data) yang nanti dipakai buat dihitung hash //
static char *build_hash_input(const Block *b) {
    size_t data_len = b->data ? strlen(b->data) : 0;
    size_t prev_len = b->prev_hash ? strlen(b->prev_hash) : 0;

    size_t buf_len = 20 + 20 + prev_len + data_len + 5;
    char *buf = malloc(buf_len);
    if (!buf) return NULL;

    int written = snprintf(buf, buf_len,
                           "%" PRIu64 ":%" PRIu64 ":%s:%s",
                           b->index,
                           b->timestamp,
                           b->prev_hash ? b->prev_hash : "",
                           b->data ? b->data : "");
    if (written < 0 || (size_t)written >= buf_len) {
        free(buf);
        return NULL;
    }
    return buf;
}


// Lanjut ke manajemen block disini ada 2 fungsi //
// Block_create dan Block_create //
// Untuk Block_free tugas nya adalah bikin block baru lalu //
// Isi field: index, timestamp, nonce awal, prev_hash, data, dan hash kosong //
// Yang terakhir langsung hitung hash pertama kali //
// Tugas fungsi bagian ini adalah //

Block *block_create(uint64_t index, uint64_t timestamp, const char *prev_hash, const char *data) {
    Block *b = calloc(1, sizeof(Block));
    if (!b) return NULL;

    b->index = index;
    b->timestamp = timestamp;
    b->nonce = 0;
    b->prev_hash = xstrdup(prev_hash ? prev_hash : "");
    b->data = xstrdup(data ? data : "");
    b->hash = calloc(SHA256_HEX_LENGTH + 1, 1);   // perbaiki size allocate

    if (!b->prev_hash || !b->data || !b->hash) {
        block_free(b);
        return NULL;
    }

    if (block_compute_hash(b, b->hash) != 0) {
        block_free(b);
        return NULL;
    }
    return b;
}

// Tugas fungsi block_free ini adalah //
// Bersihin semua memori yang dipakai blok (prev_hash, hash, data, lalu struct block) //
void block_free(Block *b) {
    if (!b) return;
    if (b->prev_hash) free(b->prev_hash);
    if (b->hash) free(b->hash);
    if (b->data) free(b->data);
    free(b);
}

// Tugas fungsi block_compute_hash nya adalah //
// Ambil blok data dalam bentuk string //
// Lalu juga hitung SHA-256 dalam bentuk hex string //
int block_compute_hash(const Block *b, char out_hash[SHA256_HEX_LENGTH + 1]) {
    if (!b || !out_hash) return -1;
    char *input = build_hash_input(b);
    if (!input) return -1;

    uint8_t digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, (uint8_t *)input, strlen(input));
    sha256_final(&ctx, digest);

    sha256_hex(digest, SHA256_DIGEST_LENGTH, out_hash);

    free(input);
    return 0;
}

// Fungsi ini tugas nya untuk //
// Ngitung berapa banyak 0 di depan string hash hex //
// Jadi ini bakal penting untuk melakukan proof of work //
static unsigned int count_leading_hex_zeros(const char *hex) {
    unsigned int count = 0;
    for (const char *p = hex; *p; ++p) {
        if (*p == '0') count++;
        else break;
    }
    return count;
}

// Membuat jaringan proof of work //
// Cuma disini karna gw udah buat di network.h //
// Jadi kek cuma panggil panggil fungsi saja //
int block_proof_of_work(Block *b) {
    if (!b) return -1;

    char *input = build_hash_input(b);
    if (!input) return -1;

    bool ok = pow_mine(&b->pow, (uint8_t*)input, strlen(input));
    free(input);

    return ok ? 0 : -1;
}

// Kita akan buat representasi block jadi string untuk debbug //
// Jadi kita bakal lebih gampang untuk debbug nya //
// Dan juga block serialize itu guna nya mengubah data dari format internal (struct, class) ke format yang bisa disimpan atau dikirim //

char *block_serialize(const Block *b) {
    if (!b) return NULL;
    const char *prev = b->prev_hash ? b->prev_hash : "";
    const char *hash = b->hash ? b->hash : "";
    const char *data = b->data ? b->data : "";

    size_t buf_len = 200 + strlen(prev) + strlen(hash) + strlen(data);
    char *buf = malloc(buf_len);
    if (!buf) return NULL;

    int written = snprintf(buf, buf_len,
        "Block(index=%" PRIu64 ", ts=%" PRIu64 ", nonce=%" PRIu64 ", prev_hash=%s, hash=%s, data=%s)",
        b->index, b->timestamp, b->nonce, prev, hash, data);
    if (written < 0 || (size_t)written >= buf_len) {
        free(buf);
        return NULL;
    }
    return buf;
}


// Memvalidasi apakah blok tersebut sudah valid atau belum //
int block_is_valid(const Block *b, const char *expected_prev_hash, unsigned int difficulty) {
    if (!b) return 0;

    if (expected_prev_hash) {
        if (!b->prev_hash) return 0;
        if (strcmp(b->prev_hash, expected_prev_hash) != 0) return 0;
    }

    char recomputed[SHA256_HEX_LENGTH + 1];
    if (block_compute_hash(b, recomputed) != 0) return 0;

    if (!b->hash) return 0;
    if (strcmp(recomputed, b->hash) != 0) return 0;

    if (difficulty > 0) {
        if (count_leading_hex_zeros(recomputed) < difficulty) return 0;
    }
    return 1;
}

// Save block yang kita buat ke file json //
// Untuk di simpan dan sebagai bukti transaksi nya //
int save_block_to_json(const Block *b, const char *filename) {
    if (!b || !filename) return -1;

    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "index", b->index);
    cJSON_AddNumberToObject(json, "timestamp", b->timestamp);
    cJSON_AddNumberToObject(json, "nonce", b->nonce);
    cJSON_AddStringToObject(json, "prev_hash", b->prev_hash ? b->prev_hash : "");
    cJSON_AddStringToObject(json, "hash", b->hash ? b->hash : "");
    cJSON_AddStringToObject(json, "data", b->data ? b->data : "");

    char *json_str = cJSON_Print(json);
    if (!json_str) {
        cJSON_Delete(json);
        return -1;
    }

    FILE *f = fopen(filename, "w");
    if (!f) {
        free(json_str);
        cJSON_Delete(json);
        return -1;
    }
    fprintf(f, "%s", json_str);
    fclose(f);

    free(json_str);
    cJSON_Delete(json);
    return 0;
}

// Kalau fungsi ini jika baca JSON dari string dan di ubah jadi struktur blok //
Block *load_block_from_json(const char *filename) {
    if (!filename) return NULL;

    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char *data = malloc(len + 1);
    if (!data) {
        fclose(f);
        return NULL;
    }
    fread(data, 1, len, f);
    data[len] = '\0';
    fclose(f);

    cJSON *json = cJSON_Parse(data);
    free(data);
    if (!json) return NULL;

    uint64_t index = cJSON_GetObjectItemCaseSensitive(json, "index")->valuedouble;
    uint64_t timestamp = cJSON_GetObjectItemCaseSensitive(json, "timestamp")->valuedouble;
    uint64_t nonce = cJSON_GetObjectItemCaseSensitive(json, "nonce")->valuedouble;
    const char *prev_hash = cJSON_GetObjectItemCaseSensitive(json, "prev_hash")->valuestring;
    const char *hash = cJSON_GetObjectItemCaseSensitive(json, "hash")->valuestring;
    const char *block_data = cJSON_GetObjectItemCaseSensitive(json, "data")->valuestring;

    Block *b = block_create(index, timestamp, prev_hash, block_data);
    if (!b) {
        cJSON_Delete(json);
        return NULL;
    }

    b->nonce = nonce;
    if (hash) {
        b->hash = strdup(hash);
    }

    cJSON_Delete(json);
    return b;
}

// Tambahkan transaksi ke block //
int block_add_transaction(Block *b, Transaction *tx) {
    if (!b || !tx) return -1;

    // Alokasi ulang array transaksi //
    Transaction **new_array = realloc(b->transactions, sizeof(Transaction*) * (b->tx_count + 1));
    if (!new_array) return -1;

    b->transactions = new_array;
    b->transactions[b->tx_count] = tx;   // simpan pointer transaksi //
    b->tx_count++;                       // tambah counter //

    return 0;
}

// Serialize semua transaksi di block (untuk logging/debug) //
char *block_serialize_transactions(const Block *b) {
    if (!b || b->tx_count == 0) return strdup("[]");  

    cJSON *json_array = cJSON_CreateArray();
    if (!json_array) return NULL;

    for (size_t i = 0; i < b->tx_count; i++) {
        Transaction *tx = b->transactions[i];
        cJSON *tx_json = cJSON_CreateObject();
        cJSON_AddStringToObject(tx_json, "sender", tx->sender);
        cJSON_AddStringToObject(tx_json, "receiver", tx->receiver);
        cJSON_AddNumberToObject(tx_json, "amount", tx->amount);
        cJSON_AddStringToObject(tx_json, "signature", tx->signature);
        cJSON_AddItemToArray(json_array, tx_json);
    }

    char *json_str = cJSON_Print(json_array);
    cJSON_Delete(json_array);
    return json_str;
}

// Validasi semua transaksi di block //
int block_validate_transactions(const Block *b) {
    if (!b) return 0;

    for (size_t i = 0; i < b->tx_count; i++) {
        Transaction *tx = b->transactions[i];
        // validasi sederhana (amount > 0, sender/receiver tidak kosong, signature sesuai SHA-256) //
        if (!validate_transaction(tx)) {
            return 0; // ada transaksi invalid //
        }
    }
    return 1; // semua transaksi valid //
}

// Membuat convert waktu biar bisa kebaca oleh kita dengan string //
void block_timestamp_to_str(const Block *b, char *buffer, size_t size) {
    timestamp_to_str(b->timestamp, buffer, size);
}

// Print buat cek bug transaksi //
void block_print_debug(const Block *b) {
    print_block(b);
    for (size_t i = 0; i < b->tx_count; i++) {
        print_transaction(b->transactions[i]);
    }
}