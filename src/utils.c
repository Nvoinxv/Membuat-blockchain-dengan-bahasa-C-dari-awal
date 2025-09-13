#include <stdio.h>
#include <time.h>
#include <string.h>
#include "../include/utils.h"
#include "../include/block.h" 

// timestamp sekarang //
long current_timestamp() {
    return (long)time(NULL); 
}

// convert timestamp ke string //
void timestamp_to_str(long ts, char *buffer, size_t size) {
    time_t t = (time_t)ts;
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// convert bytes ke hex string //
void bytes_to_hex(const uint8_t *bytes, size_t len, char *out_hex) {
    for (size_t i = 0; i < len; i++) {
        sprintf(out_hex + (i * 2), "%02x", bytes[i]);
    }
    out_hex[len * 2] = '\0';
}

// print block untuk debug //
void print_block(const struct Block *block) {
    if (!block) return;
    printf("Block #%llu\n", (unsigned long long)block->index);
    printf("Timestamp: %llu\n", (unsigned long long)block->timestamp);
    printf("Previous Hash: %s\n", block->prev_hash ? block->prev_hash : "NULL");
    printf("Hash: %s\n", block->hash ? block->hash : "NULL");
    printf("Nonce: %llu\n", (unsigned long long)block->nonce);
    printf("Transactions: %zu\n", block->tx_count);
}

// print transaksi untuk debug //
void print_transaction(const Transaction *tx) {
    if (!tx) return;
    printf("TX: %s -> %s | Amount: %.2f | Signature: %s\n",
           tx->sender ? tx->sender : "NULL",
           tx->receiver ? tx->receiver : "NULL",
           tx->amount,
           tx->signature ? tx->signature : "NULL");
}