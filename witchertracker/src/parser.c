#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

CommandType get_command_type(const char* line) {
    if (strncmp(line, "Geralt loots", 12) == 0) return CMD_LOOT;
    if (strncmp(line, "Exit", 4) == 0) return CMD_EXIT;
    return CMD_INVALID;
}

int parse_loot_command(const char* line, Ingredient* ingredients, int* count) {
    // line: "Geralt loots 3 Rebis, 4 Vitriol"
    const char* ptr = line + 13;  // Skip "Geralt loots "
    *count = 0;

    while (*ptr) {
        int qty;
        char name[MAX_NAME_LEN];

        // sscanf ile sayıyı ve ismi çekiyoruz
        if (sscanf(ptr, " %d %s", &qty, name) != 2 || qty <= 0) {
            return 0;  // invalid format
        }

        ingredients[*count].quantity = qty;
        strncpy(ingredients[*count].name, name, MAX_NAME_LEN);
        (*count)++;

        // Sonraki virgüle veya sonuna geç
        ptr = strchr(ptr, ',');
        if (!ptr) break;
        ptr++; // virgulden sonra geç
    }

    return 1; // success
}
