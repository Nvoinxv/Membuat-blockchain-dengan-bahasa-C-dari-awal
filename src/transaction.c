#include <stdio.h>
#include <string.h>
#include "../include/transaction.h"
#include "../include/crypto.h"

// Membuat transaksi //
Transaction create_transaction(const char *sender, const char *receiver, double amount) {
    Transaction tx;
    strncpy(tx.sender, sender, sizeof(tx.sender));
    strncpy(tx.receiver, receiver, sizeof(tx.receiver));
    tx.amount = amount;

    // Gabungkan data transaksi jadi string //
    char raw_data[256];
    snprintf(raw_data, sizeof(raw_data), "%s%s%f", sender, receiver, amount);

    // Hash pakai SHA-256 dan simpan hasilnya ke signature //
    sha256_hex((const uint8_t*)raw_data, strlen(raw_data), tx.signature);

    return tx;
}

// Validasi transaksi //
int validate_transaction(Transaction *tx) {
    if (tx->amount <= 0) return 0; 
    if (strlen(tx->sender) == 0 || strlen(tx->receiver) == 0) return 0;

    // Re-generate hash dari data transaksi //
    char raw_data[256];
    char expected_signature[SHA256_HEX_LENGTH + 1];
    snprintf(raw_data, sizeof(raw_data), "%s%s%f", tx->sender, tx->receiver, tx->amount);
    sha256_hex((const uint8_t*)raw_data, strlen(raw_data), expected_signature);

    // Bandingkan dengan signature yang ada //
    if (strcmp(tx->signature, expected_signature) != 0) {
        return 0; 
    }

    return 1; 
}
