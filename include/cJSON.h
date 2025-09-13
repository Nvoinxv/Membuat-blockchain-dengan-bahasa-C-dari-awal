#ifndef CJSON_H
#define CJSON_H

#include <stddef.h>

typedef struct cJSON {
    struct cJSON *next, *prev;
    struct cJSON *child;

    int type;
    char *valuestring;
    int valueint;
    double valuedouble;
    char *string;
} cJSON;

// flag type //
#define cJSON_False 0
#define cJSON_True 1
#define cJSON_NULL 2
#define cJSON_Number 3
#define cJSON_String 4
#define cJSON_Array 5
#define cJSON_Object 6

// Core API //
// Bagian parse ini untuk baca string JSON dan ubah jadi objek C //
cJSON *cJSON_Parse(const char *value);
// Bagian print untuk ubah objek JSON jadi string //
char  *cJSON_Print(const cJSON *item);
// Bagian delete ini untuk hapus/free memory JSON yang udah dipakai //
void   cJSON_Delete(cJSON *c);

// Getters //
// ambil data dari JSON berdasarkan key //
cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON *object, const char *string);
// bikin array JSON kosong //
cJSON *cJSON_CreateArray(void);

// Creators //
// bikin objek JSON kosong //
cJSON *cJSON_CreateObject(void);
// masukin item (object/array) ke JSON dengan key tertentu //
void cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item);
// masukin angka ke JSON //
void cJSON_AddNumberToObject(cJSON *object, const char *string, double number);
// masukin string ke JSON //
void cJSON_AddStringToObject(cJSON *object, const char *string, const char *value);
// masukin item ke array JSON //
void cJSON_AddItemToArray(cJSON *array, cJSON *item);
// recursive print untuk object + array //
char *cJSON_Print_Recursive(const cJSON *item, int depth);

#endif 
