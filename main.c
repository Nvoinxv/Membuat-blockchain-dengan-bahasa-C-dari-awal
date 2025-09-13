#include "../Blockchain/include/blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

int main() {
    Blockchain *chain = blockchain_create();
    if (!chain) return 1;

    // buat genesis block //
    Block *genesis = block_create(0, current_timestamp(), NULL, "Genesis Block");
    block_proof_of_work(genesis);          // mining genesis block //
    blockchain_add_block(chain, genesis);

    char prev_hash[SHA256_HEX_LENGTH + 1];
    strcpy(prev_hash, genesis->hash);

    // mining 1000 block berikutnya //
    for (int i = 1; i <= 1000; i++) {
        char data[64];
        snprintf(data, sizeof(data), "Block %d data", i);

        Block *b = block_create(i, current_timestamp(), prev_hash, data);
        block_proof_of_work(b);            
        blockchain_add_block(chain, b);

        // sekarang block sudah ada di chain, baru tambahkan transaksi //
        blockchain_add_transaction(chain, i, "Alice", "Bob", 50.0, "signature123");
        blockchain_add_transaction(chain, i, "Charlie", "Dave", 25.0, "sig456");

        strcpy(prev_hash, b->hash);        // update prev_hash untuk next block //

        if (i % 100 == 0) {                // print setiap 100 block //
            printf("Mined block %d: %s\n", i, b->hash);
        }
    }
    
    printf("Blockchain length: %zu\n", chain->length);
    for (size_t i = 0; i < chain->length; i++) {
        printf("Block %zu hash: %s\n", i, chain->blocks[i]->hash);
    }
        
    // simpan seluruh blockchain ke satu file JSON //
    save_blockchain_to_json(chain, "blockchain.json");

    // validasi chain //
    if (blockchain_is_valid(chain)) {
        printf("Blockchain valid!\n");
    }

    blockchain_free(chain);
    return 0;
}
