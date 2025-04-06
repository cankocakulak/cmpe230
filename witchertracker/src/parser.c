/**
 * @file parser.c
 * @brief Implementation of command parsing functions
 * 
 * This file implements the functions for parsing various game commands,
 * including loot, trade, brew, learn, and query commands.
 */

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

/**
 * @brief Determines the type of command from the input line
 * 
 * Checks the input line against known command patterns to determine
 * the type of command being issued.
 * 
 * @param line The input command line
 * @return The type of command (CMD_LOOT, CMD_TRADE, etc.)
 */
CommandType get_command_type(const char* line) {
    if (strncmp(line, "Geralt loots ", 12) == 0) return CMD_LOOT;
    if (strncmp(line, "Geralt trades ", 13) == 0) return CMD_TRADE;
    if (strncmp(line, "Geralt brews ", 12) == 0) return CMD_BREW;
    if (strncmp(line, "Geralt learns ", 13) == 0) return CMD_LEARN;
    if (strncmp(line, "Geralt encounters a ", 19) == 0) return CMD_ENCOUNTER;
    if (strncmp(line, "What is in ", 10) == 0 ||
        strncmp(line, "What is effective against ", 24) == 0 ||
        strncmp(line, "Total ", 6) == 0 ||
        strncmp(line, "How many ", 9) == 0 ||
        strncmp(line, "What are all ", 12) == 0) return CMD_QUERY;
    if (strcmp(line, "exit") == 0) return CMD_EXIT;
    return CMD_INVALID;
}

/**
 * @brief Parses a loot command to extract ingredients
 * 
 * Extracts ingredient names and quantities from a loot command.
 * 
 * @param line The input command line
 * @param ingredients Array to store parsed ingredients
 * @param count Pointer to store the number of ingredients
 * @return 1 if parsing was successful, 0 otherwise
 */
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

/**
 * @brief Parses a trade command to extract trophies and ingredients
 * 
 * Extracts trophy and ingredient information from a trade command.
 * 
 * @param line The input command line
 * @param trophies Array to store parsed trophies
 * @param trophy_count Pointer to store the number of trophies
 * @param ingredients Array to store parsed ingredients
 * @param ingredient_count Pointer to store the number of ingredients
 * @return 1 if parsing was successful, 0 otherwise
 */
int parse_trade_command(const char* line, Trophy* trophies, int* trophy_count, Ingredient* ingredients, int* ingredient_count) {
    const char* prefix = "Geralt trades ";
    const char* for_str = " for ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) return 0;
    
    // Find "for"
    const char* for_pos = strstr(line + strlen(prefix), for_str);
    if (!for_pos) return 0;
    
    // Extract trophies part
    char trophies_str[256];
    size_t trophies_len = for_pos - (line + strlen(prefix));
    if (trophies_len >= sizeof(trophies_str)) return 0;
    strncpy(trophies_str, line + strlen(prefix), trophies_len);
    trophies_str[trophies_len] = '\0';
    
    // Extract ingredients part
    const char* ingredients_start = for_pos + strlen(for_str);
    
    // Parse trophies
    *trophy_count = 0;
    char* token = strtok(trophies_str, ",");
    while (token && *trophy_count < MAX_TROPHIES) {
        // Skip leading spaces
        while (*token == ' ') token++;
        
        int quantity;
        char monster[MAX_NAME_LEN];
        if (sscanf(token, "%d %s trophy", &quantity, monster) == 2 && quantity > 0) {
            trophies[*trophy_count].quantity = quantity;
            strcpy(trophies[*trophy_count].monster_name, monster);
            (*trophy_count)++;
        } else {
            return 0;
        }
        
        token = strtok(NULL, ",");
    }
    
    // Parse ingredients
    *ingredient_count = 0;
    char ingredients_str[256];
    strcpy(ingredients_str, ingredients_start);
    
    token = strtok(ingredients_str, ",");
    while (token && *ingredient_count < MAX_INGREDIENTS) {
        // Skip leading spaces
        while (*token == ' ') token++;
        
        int quantity;
        char name[MAX_NAME_LEN];
        if (sscanf(token, "%d %s", &quantity, name) == 2 && quantity > 0) {
            ingredients[*ingredient_count].quantity = quantity;
            strcpy(ingredients[*ingredient_count].name, name);
            (*ingredient_count)++;
        } else {
            return 0;
        }
        
        token = strtok(NULL, ",");
    }
    
    return *trophy_count > 0 && *ingredient_count > 0;
}

/**
 * @brief Parses a brew command to extract the potion name
 * 
 * @param line The input command line
 * @param potion_name Buffer to store the potion name
 * @return 1 if parsing was successful, 0 otherwise
 */
int parse_brew_command(const char* line, char* potion_name) {
    if (sscanf(line, "Geralt brews %[^\n]", potion_name) != 1) return 0;
    return 1;
}

/**
 * @brief Parses a learn effectiveness command
 * 
 * Extracts the item name, monster name, and whether it's a sign or potion.
 * 
 * @param line The input command line
 * @param item Buffer to store the item name
 * @param monster Buffer to store the monster name
 * @param is_sign Pointer to store whether the item is a sign
 * @return 1 if parsing was successful, 0 otherwise
 */
int parse_learn_effectiveness(const char* line, char* item, char* monster, int* is_sign) {
    const char* prefix = "Geralt learns ";
    const char* sign_suffix = " sign is effective against ";
    const char* potion_suffix = " potion is effective against ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) {
        return 0;
    }
    
    // Try sign format first
    const char* sign_pos = strstr(line + strlen(prefix), sign_suffix);
    if (sign_pos) {
        // Extract sign name
        size_t name_len = sign_pos - (line + strlen(prefix));
        if (name_len >= MAX_NAME_LEN) {
            return 0;
        }
        strncpy(item, line + strlen(prefix), name_len);
        item[name_len] = '\0';
        
        // Extract monster name
        const char* monster_start = sign_pos + strlen(sign_suffix);
        size_t monster_len = strlen(monster_start);
        
        // Remove trailing newline if present
        if (monster_len > 0 && monster_start[monster_len - 1] == '\n') {
            monster_len--;
        }
        
        if (monster_len >= MAX_NAME_LEN) {
            return 0;
        }
        
        strncpy(monster, monster_start, monster_len);
        monster[monster_len] = '\0';
        
        *is_sign = 1;
        return 1;
    }
    
    // Try potion format
    const char* potion_pos = strstr(line + strlen(prefix), potion_suffix);
    if (potion_pos) {
        // Extract potion name
        size_t name_len = potion_pos - (line + strlen(prefix));
        if (name_len >= MAX_NAME_LEN) {
            return 0;
        }
        strncpy(item, line + strlen(prefix), name_len);
        item[name_len] = '\0';
        
        // Extract monster name
        const char* monster_start = potion_pos + strlen(potion_suffix);
        size_t monster_len = strlen(monster_start);
        
        // Remove trailing newline if present
        if (monster_len > 0 && monster_start[monster_len - 1] == '\n') {
            monster_len--;
        }
        
        if (monster_len >= MAX_NAME_LEN) {
            return 0;
        }
        
        strncpy(monster, monster_start, monster_len);
        monster[monster_len] = '\0';
        
        *is_sign = 0;
        return 1;
    }
    
    return 0;
}

/**
 * @brief Parses a learn formula command
 * 
 * Extracts the potion name and its ingredients from a formula learning command.
 * 
 * @param line The input command line
 * @param formula Structure to store the parsed formula
 * @return 1 if parsing was successful, 0 otherwise
 */
int parse_learn_formula(const char* line, PotionFormula* formula) {
    const char* prefix = "Geralt learns ";
    const char* potion_suffix = " potion consists of ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) {
        return 0;
    }
    
    // Find "potion consists of"
    const char* suffix_pos = strstr(line + strlen(prefix), potion_suffix);
    if (!suffix_pos) {
        return 0;
    }
    
    // Extract potion name
    size_t name_len = suffix_pos - (line + strlen(prefix));
    if (name_len >= MAX_NAME_LEN) {
        return 0;
    }
    strncpy(formula->name, line + strlen(prefix), name_len);
    formula->name[name_len] = '\0';
    
    // Parse ingredients after "consists of"
    const char* ingredients_start = suffix_pos + strlen(potion_suffix);
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
        if (sscanf(token, "%d %s", &quantity, name) == 2 && quantity > 0) {
            formula->ingredients[formula->ingredient_count].quantity = quantity;
            strcpy(formula->ingredients[formula->ingredient_count].name, name);
            formula->ingredient_count++;
        }
        
        token = strtok(NULL, ",");
    }
    
    return formula->ingredient_count > 0;
}

/**
 * @brief Parses an encounter command
 * 
 * Extracts the monster name from an encounter command.
 * 
 * @param line The input command line
 * @param monster Buffer to store the monster name
 * @return 1 if parsing was successful, 0 otherwise
 */
int parse_encounter_command(const char* line, char* monster_name) {
    const char* prefix = "Geralt encounters a ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) {
        return 0;
    }
    
    // Extract monster name
    const char* start = line + strlen(prefix);
    size_t name_len = strlen(start);
    
    // Remove trailing newline and spaces
    while (name_len > 0 && (start[name_len - 1] == '\n' || isspace((unsigned char)start[name_len - 1]))) {
        name_len--;
    }
    
    if (name_len >= MAX_NAME_LEN || name_len == 0) {
        return 0;
    }
    
    // Check if monster name contains only alphabetic characters and spaces
    for (size_t i = 0; i < name_len; i++) {
        if (!isalpha((unsigned char)start[i]) && start[i] != ' ') {
            return 0;
        }
    }
    
    strncpy(monster_name, start, name_len);
    monster_name[name_len] = '\0';
    
    return 1;
}

/**
 * @brief Parses a total specific query command
 * 
 * Extracts the category and item name from a total query.
 * 
 * @param line The input command line
 * @param category Buffer to store the category
 * @param name Buffer to store the item name
 * @return 1 if parsing was successful, 0 otherwise
 */
int parse_total_specific_query(const char* line, char* category, char* name) {
    const char* prefix = "Total ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) return 0;
    
    // Find question mark
    const char* qmark = strrchr(line, '?');
    if (!qmark) return 0;
    
    // Extract category and name
    const char* start = line + strlen(prefix);
    size_t len = qmark - start;
    
    // Skip trailing spaces
    while (len > 0 && isspace((unsigned char)*(start + len - 1))) {
        len--;
    }
    
    // Skip leading spaces
    while (len > 0 && isspace((unsigned char)*start)) {
        start++;
        len--;
    }
    
    // Copy to buffer for processing
    char buffer[256];
    if (len >= sizeof(buffer)) return 0;
    strncpy(buffer, start, len);
    buffer[len] = '\0';
    
    // Split into category and name
    char* space = strchr(buffer, ' ');
    if (!space) return 0;
    *space = '\0';
    space++;
    
    // Skip leading spaces in name
    while (*space && isspace((unsigned char)*space)) {
        space++;
    }
    
    // Get category
    if (strlen(buffer) >= MAX_NAME_LEN) return 0;
    strcpy(category, buffer);
    
    // Get name
    if (strlen(space) >= MAX_NAME_LEN) return 0;
    strcpy(name, space);
    
    // Check if category is valid
    if (strcmp(category, "ingredient") != 0 &&
        strcmp(category, "potion") != 0 &&
        strcmp(category, "trophy") != 0) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Parses a total all query command
 * 
 * Extracts the category from a total all query.
 * 
 * @param line The input command line
 * @param category Buffer to store the category
 * @return 1 if parsing was successful, 0 otherwise
 */
int parse_total_all_query(const char* line, char* category) {
    const char* prefix = "Total ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) return 0;
    
    // Find question mark
    const char* qmark = strrchr(line, '?');
    if (!qmark) return 0;
    
    // Extract category
    const char* start = line + strlen(prefix);
    size_t len = qmark - start;
    
    // Skip trailing spaces
    while (len > 0 && isspace((unsigned char)*(start + len - 1))) {
        len--;
    }
    
    // Skip leading spaces
    while (len > 0 && isspace((unsigned char)*start)) {
        start++;
        len--;
    }
    
    // Copy category
    if (len >= MAX_NAME_LEN) return 0;
    strncpy(category, start, len);
    category[len] = '\0';
    
    // Check if category is valid
    if (strcmp(category, "ingredient") != 0 &&
        strcmp(category, "potion") != 0 &&
        strcmp(category, "trophy") != 0) {
        return 0;
    }
    
    return 1;
}

/**
 * @brief Parses an effectiveness query command
 * 
 * Extracts the monster name from an effectiveness query.
 * 
 * @param line The input command line
 * @param monster Buffer to store the monster name
 * @return 1 if parsing was successful, 0 otherwise
 */
int parse_effectiveness_query(const char* line, char* monster_name) {
    const char* prefix = "What is effective against ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) {
        return 0;
    }
    
    // Find question mark
    const char* qmark = strrchr(line, '?');
    if (!qmark) return 0;
    
    // Extract monster name
    const char* monster_start = line + strlen(prefix);
    size_t name_len = qmark - monster_start;
    
    // Skip trailing spaces
    while (name_len > 0 && isspace((unsigned char)*(monster_start + name_len - 1))) {
        name_len--;
    }
    
    if (name_len >= MAX_NAME_LEN || name_len == 0) {
        return 0;
    }
    
    strncpy(monster_name, monster_start, name_len);
    monster_name[name_len] = '\0';
    
    return 1;
}

/**
 * @brief Parses a formula query command
 * 
 * Extracts the potion name from a formula query.
 * 
 * @param line The input command line
 * @param potion_name Buffer to store the potion name
 * @return 1 if parsing was successful, 0 otherwise
 */
int parse_formula_query(const char* line, char* potion_name) {
    const char* prefix = "What is in ";
    
    // Check prefix
    if (strncmp(line, prefix, strlen(prefix)) != 0) {
        return 0;
    }
    
    // Find question mark
    const char* qmark = strrchr(line, '?');
    if (!qmark) {
        return 0;
    }
    
    // Extract potion name
    const char* start = line + strlen(prefix);
    size_t name_len = qmark - start;
    
    // Skip trailing spaces before question mark
    while (name_len > 0 && isspace((unsigned char)*(start + name_len - 1))) {
        name_len--;
    }
    
    if (name_len >= MAX_NAME_LEN || name_len == 0) {
        return 0;
    }
    
    strncpy(potion_name, start, name_len);
    potion_name[name_len] = '\0';
    
    return 1;
}
