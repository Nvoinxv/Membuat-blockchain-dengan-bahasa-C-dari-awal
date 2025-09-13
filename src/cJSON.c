#include "../include/cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Utility //
static cJSON *cJSON_New_Item(void) {
    cJSON *node = (cJSON *)calloc(1, sizeof(cJSON));
    return node;
}

// Buat object kosong //
cJSON *cJSON_CreateObject(void) {
    cJSON *item = cJSON_New_Item();
    item->type = cJSON_Object;
    return item;
}

// Tambah child ke object //
void cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item) {
    if (!object || !item) return;
    item->string = strdup(string);
    if (!object->child) {
        object->child = item;
    } else {
        cJSON *c = object->child;
        while (c->next) c = c->next;
        c->next = item;
        item->prev = c;
    }
}

// Tambah nomer //
void cJSON_AddNumberToObject(cJSON *object, const char *string, double number) {
    cJSON *item = cJSON_New_Item();
    item->type = cJSON_Number;
    item->valuedouble = number;
    item->valueint = (int)number;
    cJSON_AddItemToObject(object, string, item);
}

// masukin item ke array JSON //
void cJSON_AddItemToArray(cJSON *array, cJSON *item) {
    if (!array || !item) return;

    item->next = NULL;
    item->prev = NULL;

    if (!array->child) {
        // kalau array kosong, langsung jadi child pertama
        array->child = item;
    } else {
        // kalau udah ada child, tambahkan di akhir linked list
        cJSON *last = array->child;
        while (last->next) last = last->next;

        last->next = item;
        item->prev = last;
    }
}

// Tambah string //
void cJSON_AddStringToObject(cJSON *object, const char *string, const char *value) {
    cJSON *item = cJSON_New_Item();
    item->type = cJSON_String;
    item->valuestring = strdup(value);
    cJSON_AddItemToObject(object, string, item);
}

// Print JSON //
char *cJSON_Print(const cJSON *item) {
    if (!item || item->type != cJSON_Object) return NULL;

    char *out = (char *)malloc(1024);
    strcpy(out, "{");
    cJSON *child = item->child;
    while (child) {
        char buffer[256];
        if (child->type == cJSON_String) {
            snprintf(buffer, sizeof(buffer), "\"%s\":\"%s\"", child->string, child->valuestring);
        } else if (child->type == cJSON_Number) {
            snprintf(buffer, sizeof(buffer), "\"%s\":%d", child->string, child->valueint);
        } else {
            snprintf(buffer, sizeof(buffer), "\"%s\":null", child->string);
        }
        strcat(out, buffer);
        if (child->next) strcat(out, ",");
        child = child->next;
    }
    strcat(out, "}");
    return out;
}

// Delete //
void cJSON_Delete(cJSON *c) {
    if (!c) return;
    cJSON *child = c->child;
    while (child) {
        cJSON *next = child->next;
        cJSON_Delete(child);
        child = next;
    }
    if (c->valuestring) free(c->valuestring);
    if (c->string) free(c->string);
    free(c);
}

// Getter  //
cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON *object, const char *string) {
    if (!object || object->type != cJSON_Object) return NULL;
    cJSON *child = object->child;
    while (child) {
        if (child->string && strcmp(child->string, string) == 0) {
            return child;
        }
        child = child->next;
    }
    return NULL;
}

// bikin array JSON kosong //
cJSON *cJSON_CreateArray(void) {
    cJSON *array = calloc(1, sizeof(cJSON));
    if (!array) return NULL;

    array->type = cJSON_Array;   // tandai ini sebagai array //
    array->child = NULL;         // awalnya kosong //
    array->next = NULL;
    array->prev = NULL;
    array->string = NULL;        // array tidak punya key //

    return array;
}

// Parser //
cJSON *cJSON_Parse(const char *value) {
    if (!value) return NULL;

    cJSON *root = cJSON_CreateObject();

    char key[128], val[256];
    const char *p = value;
    while ((p = strchr(p, '\"'))) {
        const char *q = strchr(p + 1, '\"');
        strncpy(key, p + 1, q - p - 1);
        key[q - p - 1] = '\0';

        p = strchr(q + 1, ':');
        if (!p) break;
        p++;

        if (*p == '\"') { // string //
            q = strchr(p + 1, '\"');
            strncpy(val, p + 1, q - p - 1);
            val[q - p - 1] = '\0';
            cJSON_AddStringToObject(root, key, val);
            p = q + 1;
        } else { // number //
            int num = atoi(p);
            cJSON_AddNumberToObject(root, key, num);
            while (*p && *p != ',' && *p != '}') p++;
        }
    }
    return root;
}

// Helper: append string dinamis //
static void append_str(char **out, size_t *size, const char *s) {
    size_t len = strlen(*out) + strlen(s) + 1;
    if (len > *size) {
        *size = len * 2;
        *out = (char *)realloc(*out, *size);
    }
    strcat(*out, s);
}

// Helper: indentasi //
static void append_indent(char **out, size_t *size, int depth) {
    for (int i = 0; i < depth; i++) {
        append_str(out, size, "    "); // 4 spasi per level //
    }
}

// Versi rekursif dengan indentasi //
char *cJSON_Print_Recursive(const cJSON *item, int depth) {
    if (!item) return NULL;

    size_t size = 1024;
    char *out = (char *)malloc(size);
    out[0] = '\0';

    if (item->type == cJSON_Object) {
        append_str(&out, &size, "{\n");
        cJSON *child = item->child;
        while (child) {
            append_indent(&out, &size, depth + 1);

            char buffer[512];
            if (child->type == cJSON_String) {
                snprintf(buffer, sizeof(buffer), "\"%s\": \"%s\"", child->string, child->valuestring);
            } else if (child->type == cJSON_Number) {
                snprintf(buffer, sizeof(buffer), "\"%s\": %d", child->string, child->valueint);
            } else if (child->type == cJSON_Object || child->type == cJSON_Array) {
                char *nested = cJSON_Print_Recursive(child, depth + 1);
                snprintf(buffer, sizeof(buffer), "\"%s\": %s", child->string, nested);
                free(nested);
            } else {
                snprintf(buffer, sizeof(buffer), "\"%s\": null", child->string);
            }

            append_str(&out, &size, buffer);
            if (child->next) append_str(&out, &size, ",\n"); 
            else append_str(&out, &size, "\n");
            child = child->next;
        }
        append_indent(&out, &size, depth);
        append_str(&out, &size, "}");
    } else if (item->type == cJSON_Array) {
        append_str(&out, &size, "[\n");
        cJSON *child = item->child;
        while (child) {
            append_indent(&out, &size, depth + 1);
            char *nested = cJSON_Print_Recursive(child, depth + 1);
            append_str(&out, &size, nested);
            free(nested);
            if (child->next) append_str(&out, &size, ",\n");
            else append_str(&out, &size, "\n");
            child = child->next;
        }
        append_indent(&out, &size, depth);
        append_str(&out, &size, "]");
    } else if (item->type == cJSON_String) {
        snprintf(out, size, "\"%s\"", item->valuestring);
    } else if (item->type == cJSON_Number) {
        snprintf(out, size, "%d", item->valueint);
    } else {
        snprintf(out, size, "null");
    }

    return out;
}