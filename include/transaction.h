#ifndef TRANSACTION_H
#define TRANSACTION_H

typedef struct {
    char sender[64];     // alamat pengirim //
    char receiver[64];   // alamat penerima //
    double amount;       // jumlah coin //
    char signature[128]; // tanda tangan digital //
} Transaction;

// fungsi untuk buat transaksi baru //
Transaction create_transaction(const char *sender, const char *receiver, double amount);

// fungsi untuk validasi transaksi //
int validate_transaction(Transaction *tx);

#endif
