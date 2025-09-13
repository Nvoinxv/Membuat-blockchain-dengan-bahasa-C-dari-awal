#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "block.h"
#include "transaction.h"

// Forward declaration
struct Block;

// waktu
long current_timestamp();
void timestamp_to_str(long ts, char *buffer, size_t size);

// konversi data
void bytes_to_hex(const uint8_t *bytes, size_t len, char *out_hex);

// debug
void print_block(const struct Block *block);
void print_transaction(const Transaction *tx);

#endif
