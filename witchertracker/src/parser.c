#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
// --- Genel Yardımcı Fonksiyonlar ---
static void trim_newline(char* str) {
    str[strcspn(str, "\n")] = 0;
}*/

// --- Komut Tipi Belirleme ---
CommandType get_command_type(const char* line) {
    if (strncmp(line, "Geralt loots", 12) == 0) return CMD_LOOT;
    if (strncmp(line, "Geralt trades", 13) == 0) return CMD_TRADE;
    if (strncmp(line, "Geralt brews", 12) == 0) return CMD_BREW;
    if (strncmp(line, "Geralt learns", 13) == 0) return CMD_LEARN;
    if (strncmp(line, "Geralt encounters a", 20) == 0) return CMD_ENCOUNTER;
    if (strncmp(line, "Total", 5) == 0 || strncmp(line, "What is", 7) == 0) return CMD_QUERY;
    if (strncmp(line, "Exit", 4) == 0) return CMD_EXIT;
    return CMD_INVALID;
}

// --- Loot ---
int parse_loot_command(const char* line, Ingredient* ingredients, int* count) {
    const char* ptr = line + 13;
    *count = 0;

    while (*ptr) {
        int qty;
        char name[MAX_NAME_LEN];
        if (sscanf(ptr, " %d %s", &qty, name) != 2 || qty <= 0) return 0;
        strncpy(ingredients[*count].name, name, MAX_NAME_LEN);
        ingredients[*count].quantity = qty;
        (*count)++;
        ptr = strchr(ptr, ',');
        if (!ptr) break;
        ptr++;
    }

    return 1;
}

// --- Trade ---
int parse_trade_command(const char* line, Trophy* trophies, int* trophy_count, Ingredient* ingredients, int* ingredient_count) {
    char before[512], after[512];
    if (!strstr(line, "for")) return 0;
    sscanf(line, "Geralt trades %[^\n] for %[^\n]", before, after);

    // Trophies
    *trophy_count = 0;
    char* token = strtok(before, ",");
    while (token != NULL) {
        int qty;
        char monster[MAX_NAME_LEN], check[16];
        if (sscanf(token, " %d %s %s", &qty, monster, check) != 3 || strcmp(check, "trophy") != 0 || qty <= 0)
            return 0;
        strncpy(trophies[*trophy_count].monster_name, monster, MAX_NAME_LEN);
        trophies[*trophy_count].quantity = qty;
        (*trophy_count)++;
        token = strtok(NULL, ",");
    }

    // Ingredients
    *ingredient_count = 0;
    token = strtok(after, ",");
    while (token != NULL) {
        int qty;
        char name[MAX_NAME_LEN];
        if (sscanf(token, " %d %s", &qty, name) != 2 || qty <= 0) return 0;
        strncpy(ingredients[*ingredient_count].name, name, MAX_NAME_LEN);
        ingredients[*ingredient_count].quantity = qty;
        (*ingredient_count)++;
        token = strtok(NULL, ",");
    }

    return 1;
}

// --- Brew ---
int parse_brew_command(const char* line, char* potion_name) {
    if (sscanf(line, "Geralt brews %[^\n]", potion_name) != 1) return 0;
    return 1;
}

// --- Learn: effectiveness ---
int parse_learn_effectiveness(const char* line, char* item_name, char* monster_name, int* is_sign) {
    *is_sign = 0;

    const char* sign_key = " sign is effective against ";
    const char* potion_key = " potion is effective against ";

    if (strstr(line, sign_key)) {
        *is_sign = 1;
        return sscanf(line, "Geralt learns %s sign is effective against %s", item_name, monster_name) == 2;
    }

    const char* mid = strstr(line, potion_key);
    if (!mid) return 0;

    int name_len = mid - (line + strlen("Geralt learns "));
    strncpy(item_name, line + strlen("Geralt learns "), name_len);
    item_name[name_len] = '\0';

    strcpy(monster_name, mid + strlen(potion_key));
    return 1;
}


// --- Learn: potion formula ---

int parse_learn_formula(const char* line, PotionFormula* formula) {
    const char* prefix = "Geralt learns ";
    const char* middle = " potion consists of ";
    const char* start = strstr(line, prefix);
    const char* mid = strstr(line, middle);

    if (!start || !mid) return 0;

    int name_len = mid - (line + strlen(prefix));
    if (name_len >= MAX_NAME_LEN) return 0;

    strncpy(formula->name, line + strlen(prefix), name_len);
    formula->name[name_len] = '\0';

    const char* ingredients_part = mid + strlen(middle);
    formula->ingredient_count = 0;

    char buffer[512];
    strncpy(buffer, ingredients_part, sizeof(buffer));
    char* token = strtok(buffer, ",");

    while (token != NULL) {
        int qty;
        char ing[MAX_NAME_LEN];
        if (sscanf(token, " %d %s", &qty, ing) != 2 || qty <= 0) return 0;
        strncpy(formula->ingredients[formula->ingredient_count].name, ing, MAX_NAME_LEN);
        formula->ingredients[formula->ingredient_count].quantity = qty;
        formula->ingredient_count++;
        token = strtok(NULL, ",");
    }

    return 1;
}


// --- Encounter ---
int parse_encounter_command(const char* line, char* monster_name) {
    return sscanf(line, "Geralt encounters a %s", monster_name) == 1;
}

// --- Total specific query ---
int parse_total_specific_query(const char* line, char* category, char* name) {
    return sscanf(line, "Total %s %s ?", category, name) == 2;
}

// --- Total all query ---
int parse_total_all_query(const char* line, char* category) {
    return sscanf(line, "Total %s ?", category) == 1;
}

// --- Effectiveness query ---
int parse_effectiveness_query(const char* line, char* monster_name) {
    return sscanf(line, "What is effective against %s ?", monster_name) == 1;
}

// --- Formula query ---
int parse_formula_query(const char* line, char* potion_name) {
    return sscanf(line, "What is in %[^\n] ?", potion_name) == 1;
}
