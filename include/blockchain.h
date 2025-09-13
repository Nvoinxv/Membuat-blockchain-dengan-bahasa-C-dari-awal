#include "block.h"

typedef struct Blockchain {
    Block **blocks;   // array pointer ke Block //
    size_t length;    // jumlah block di chain //
} Blockchain;

Blockchain *blockchain_create();
void blockchain_free(Blockchain *chain);
int blockchain_add_block(Blockchain *chain, Block *b);
int blockchain_is_valid(const Blockchain *chain);

int blockchain_add_transaction(Blockchain *chain, size_t block_index,
                               const char *sender, const char *receiver,
                               double amount, const char *signature);
                               
// deklarasi untuk save/load blockchain JSON //
int save_blockchain_to_json(const Blockchain *chain, const char *filename);
Blockchain *load_blockchain_from_json(const char *filename);