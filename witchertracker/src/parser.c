#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_NAME_LEN 64

/*
// --- Genel Yardımcı Fonksiyonlar ---
static void trim_newline(char* str) {
    str[strcspn(str, "\n")] = 0;
}*/

// --- Komut Tipi Belirleme ---
CommandType get_command_type(const char* line) {
    if (strncmp(line, "Geralt loots ", 12) == 0) return CMD_LOOT;
    if (strncmp(line, "Geralt trades ", 13) == 0) return CMD_TRADE;
    if (strncmp(line, "Geralt brews ", 12) == 0) return CMD_BREW;
    if (strncmp(line, "Geralt learns ", 13) == 0) return CMD_LEARN;
    if (strncmp(line, "Geralt encounters a ", 19) == 0) return CMD_ENCOUNTER;
    if (strncmp(line, "What is in ", 10) == 0 ||
        strncmp(line, "What is effective against ", 24) == 0 ||
        strncmp(line, "How many ", 9) == 0 ||
        strncmp(line, "What are all ", 12) == 0) return CMD_QUERY;
    if (strcmp(line, "exit") == 0) return CMD_EXIT;
    return CMD_INVALID;
}

// --- Loot ---
int parse_loot_command(const char* line, Ingredient* ingredients, int* count) {
    char buffer[1024];
    strncpy(buffer, line + 13, sizeof(buffer));
    *count = 0;

    char* token = strtok(buffer, ",");
    while (token != NULL) {
        int qty;
        char name[MAX_NAME_LEN];
        if (sscanf(token, " %d %s", &qty, name) != 2 || qty <= 0) return 0;
        strncpy(ingredients[*count].name, name, MAX_NAME_LEN);
        ingredients[*count].quantity = qty;
        (*count)++;
        token = strtok(NULL, ",");
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
int parse_learn_effectiveness(const char* line, char* item, char* monster, int* is_sign) {
    const char* prefix = "Geralt learns ";
    const char* weak_against = " is weak against ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) {
        return 0;
    }
    
    // Find "is weak against"
    const char* weak_pos = strstr(line + strlen(prefix), weak_against);
    if (!weak_pos) {
        return 0;
    }
    
    // Extract monster name
    size_t monster_len = weak_pos - (line + strlen(prefix));
    if (monster_len >= MAX_NAME_LEN) {
        return 0;
    }
    strncpy(monster, line + strlen(prefix), monster_len);
    monster[monster_len] = '\0';
    
    // Extract item name
    const char* item_start = weak_pos + strlen(weak_against);
    size_t item_len = strlen(item_start);
    
    // Remove trailing newline if present
    if (item_len > 0 && item_start[item_len - 1] == '\n') {
        item_len--;
    }
    
    if (item_len >= MAX_NAME_LEN) {
        return 0;
    }
    
    strncpy(item, item_start, item_len);
    item[item_len] = '\0';
    
    // Check if it's a sign
    *is_sign = (strcmp(item, "Aard") == 0 || strcmp(item, "Igni") == 0 ||
                strcmp(item, "Yrden") == 0 || strcmp(item, "Quen") == 0 ||
                strcmp(item, "Axii") == 0);
    
    return 1;
}


// --- Learn: potion formula ---

int parse_learn_formula(const char* line, PotionFormula* formula) {
    const char* prefix = "Geralt learns ";
    const char* colon = ": ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) {
        return 0;
    }
    
    // Find the colon
    const char* colon_pos = strstr(line + strlen(prefix), colon);
    if (!colon_pos) {
        return 0;
    }
    
    // Extract potion name
    size_t name_len = colon_pos - (line + strlen(prefix));
    if (name_len >= MAX_NAME_LEN) {
        return 0;
    }
    strncpy(formula->name, line + strlen(prefix), name_len);
    formula->name[name_len] = '\0';
    
    // Parse ingredients after colon
    const char* ingredients_start = colon_pos + strlen(colon);
    char ingredients_copy[256];
    strncpy(ingredients_copy, ingredients_start, sizeof(ingredients_copy) - 1);
    ingredients_copy[sizeof(ingredients_copy) - 1] = '\0';
    
    // Remove trailing newline if present
    char* newline = strchr(ingredients_copy, '\n');
    if (newline) *newline = '\0';
    
    // Split by comma and space
    char* token = strtok(ingredients_copy, ",");
    formula->ingredient_count = 0;
    
    while (token && formula->ingredient_count < MAX_INGREDIENTS) {
        // Skip leading spaces
        while (*token == ' ') token++;
        
        // Parse quantity
        int quantity;
        char name[MAX_NAME_LEN];
        if (sscanf(token, "%d %s", &quantity, name) == 2) {
            formula->ingredients[formula->ingredient_count].quantity = quantity;
            strcpy(formula->ingredients[formula->ingredient_count].name, name);
            formula->ingredient_count++;
        }
        
        token = strtok(NULL, ",");
    }
    
    return formula->ingredient_count > 0;
}


// --- Encounter ---
int parse_encounter_command(const char* line, char* monster_name) {
    const char* prefix = "Geralt encounters a ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) {
        return 0;
    }
    
    // Extract monster name
    const char* monster_start = line + strlen(prefix);
    size_t name_len = strlen(monster_start);
    
    // Remove trailing newline if present
    if (name_len > 0 && monster_start[name_len - 1] == '\n') {
        name_len--;
    }
    
    if (name_len >= MAX_NAME_LEN) {
        return 0;
    }
    
    strncpy(monster_name, monster_start, name_len);
    monster_name[name_len] = '\0';
    
    return 1;
}

// --- Total specific query ---
int parse_total_specific_query(const char* line, char* category, char* name) {
    const char* qmark = strrchr(line, '?');
    if (!qmark) return 0;
    
    // First get the category
    if (sscanf(line, "Total %s", category) != 1) return 0;
    
    // Find where the category ends and name starts
    const char* name_start = line + strlen("Total ") + strlen(category) + 1;
    
    // Find the last space before the question mark
    const char* last_space = qmark;
    while (last_space > line && isspace(*(last_space - 1))) last_space--;
    
    // Calculate the length of the name
    int name_len = last_space - name_start;
    if (name_len <= 0 || name_len >= MAX_NAME_LEN) return 0;
    
    // Copy the name
    strncpy(name, name_start, name_len);
    name[name_len] = '\0';
    
    return 1;
}

// --- Total all query ---
int parse_total_all_query(const char* line, char* category) {
    const char* qmark = strrchr(line, '?');
    if (!qmark) return 0;
    
    // Find where the category starts
    const char* category_start = line + strlen("Total ");
    
    // Find the last space before the question mark
    const char* last_space = qmark;
    while (last_space > line && isspace(*(last_space - 1))) last_space--;
    
    // Calculate the length of the category
    int cat_len = last_space - category_start;
    if (cat_len <= 0 || cat_len >= MAX_NAME_LEN) return 0;
    
    // Copy the category
    strncpy(category, category_start, cat_len);
    category[cat_len] = '\0';
    
    return 1;
}

// --- Effectiveness query ---
int parse_effectiveness_query(const char* line, char* monster_name) {
    const char* qmark = strrchr(line, '?');
    if (!qmark) return 0;
    
    // Find where the monster name starts
    const char* monster_start = line + strlen("What is effective against ");
    
    // Find the last space before the question mark
    const char* last_space = qmark;
    while (last_space > line && isspace(*(last_space - 1))) last_space--;
    
    // Calculate the length of the monster name
    int name_len = last_space - monster_start;
    if (name_len <= 0 || name_len >= MAX_NAME_LEN) return 0;
    
    // Copy the monster name
    strncpy(monster_name, monster_start, name_len);
    monster_name[name_len] = '\0';
    
    return 1;
}

// --- Formula query ---
int parse_formula_query(const char* line, char* potion_name) {
    const char* prefix = "What is in ";
    const char* question_mark = "?";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) {
        return 0;
    }
    
    // Find question mark
    const char* qmark_pos = strstr(line + strlen(prefix), question_mark);
    if (!qmark_pos) {
        return 0;
    }
    
    // Extract potion name
    size_t name_len = qmark_pos - (line + strlen(prefix));
    
    // Skip trailing spaces
    while (name_len > 0 && isspace((unsigned char)*(line + strlen(prefix) + name_len - 1))) {
        name_len--;
    }
    
    if (name_len >= MAX_NAME_LEN || name_len == 0) {
        return 0;
    }
    
    strncpy(potion_name, line + strlen(prefix), name_len);
    potion_name[name_len] = '\0';
    
    return 1;
}
