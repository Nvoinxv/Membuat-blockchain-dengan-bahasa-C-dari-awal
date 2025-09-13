#include "../include/blockchain.h"
#include "../include/cJSON.h"
#include "../include/transaction.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Disini fungsi nya membuat blockchain //
Blockchain *blockchain_create() {
    Blockchain *chain = (Blockchain *)malloc(sizeof(Blockchain));
    if (!chain) {
        fprintf(stderr, "Failed to allocate memory for blockchain\n");
        return NULL;
    }

    chain->blocks = NULL; 
    chain->length = 0;
    return chain;
}

int blockchain_add_transaction(Blockchain *chain, size_t block_index,
                               const char *sender, const char *receiver,
                               double amount, const char *signature) {
    if (!chain || block_index >= chain->length) return -1;

    Block *b = chain->blocks[block_index];
    if (!b) return -1;

    // buat transaction baru
    Transaction *tx = malloc(sizeof(Transaction));
    if (!tx) return -1;

    // pakai strcpy karena field adalah array
    strncpy(tx->sender, sender, sizeof(tx->sender) - 1);
    tx->sender[sizeof(tx->sender) - 1] = '\0';

    strncpy(tx->receiver, receiver, sizeof(tx->receiver) - 1);
    tx->receiver[sizeof(tx->receiver) - 1] = '\0';

    tx->amount = amount;

    strncpy(tx->signature, signature, sizeof(tx->signature) - 1);
    tx->signature[sizeof(tx->signature) - 1] = '\0';

    // tambahkan ke block
    return block_add_transaction(b, tx);
}

// Free semua memory blockchain //
void blockchain_free(Blockchain *chain) {
    if (!chain) return;

    for (size_t i = 0; i < chain->length; i++) {
        block_free(chain->blocks[i]); // pakai fungsi block_free dari block.c
    }
    free(chain->blocks);
    free(chain);
}

// Tambahkan block ke blockchain //
int blockchain_add_block(Blockchain *chain, Block *b) {
    if (!chain || !b) return -1;

    // realloc array block
    Block **new_blocks = (Block **)realloc(chain->blocks, sizeof(Block *) * (chain->length + 1));
    if (!new_blocks) {
        fprintf(stderr, "Failed to allocate memory for new block\n");
        return -1;
    }

    chain->blocks = new_blocks;
    chain->blocks[chain->length] = b;
    chain->length++;

    return 0; 
}

// Validasi blockchain //
int blockchain_is_valid(const Blockchain *chain) {
    if (!chain) return 0;
    if (chain->length == 0) return 1; // kosong dianggap valid //

    for (size_t i = 0; i < chain->length; i++) {
        Block *b = chain->blocks[i];
        const char *prev_hash = (i == 0) ? NULL : chain->blocks[i - 1]->hash;

        // validasi block
        if (!block_is_valid(b, prev_hash, b->pow.difficulty)) {
            fprintf(stderr, "Block %zu tidak valid!\n", i);
            return 0;
        }
    }

    return 1; 
}


// Save hasil transaksi atau mining nya ke json //
int save_blockchain_to_json(const Blockchain *chain, const char *filename) {
    if (!chain || !filename) return 0;

    cJSON *array = cJSON_CreateArray();
    if (!array) return 0;

    for (size_t i = 0; i < chain->length; i++) {
        Block *b = chain->blocks[i];
        cJSON *obj = cJSON_CreateObject();

        cJSON_AddNumberToObject(obj, "index", b->index);
        cJSON_AddNumberToObject(obj, "timestamp", b->timestamp);
        cJSON_AddStringToObject(obj, "prev_hash", b->prev_hash ? b->prev_hash : "NULL");
        cJSON_AddStringToObject(obj, "hash", b->hash ? b->hash : "NULL");
        cJSON_AddStringToObject(obj, "data", b->data ? b->data : "");

        // ==== info mining ==== //
        cJSON_AddNumberToObject(obj, "nonce", b->pow.nonce);
        cJSON_AddNumberToObject(obj, "difficulty", b->pow.difficulty);
        cJSON_AddStringToObject(obj, "pow_hash", b->pow.hash ? b->pow.hash : "NULL");

        // transaksi //
        cJSON *tx_array = cJSON_CreateArray();
        for (size_t t = 0; t < b->tx_count; t++) {
            Transaction *tx = b->transactions[t];
            cJSON *tx_obj = cJSON_CreateObject();
            cJSON_AddStringToObject(tx_obj, "sender", tx->sender);
            cJSON_AddStringToObject(tx_obj, "receiver", tx->receiver);
            cJSON_AddNumberToObject(tx_obj, "amount", tx->amount);
            cJSON_AddStringToObject(tx_obj, "signature", tx->signature);
            cJSON_AddItemToArray(tx_array, tx_obj);
        }
        cJSON_AddItemToObject(obj, "transactions", tx_array);

        cJSON_AddItemToArray(array, obj);
    }

    char *json_str = cJSON_Print_Recursive(array, 0);
    cJSON_Delete(array);

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        free(json_str);
        return 0;
    }
    fprintf(fp, "%s", json_str);
    fclose(fp);
    free(json_str);
    return 1;
}
