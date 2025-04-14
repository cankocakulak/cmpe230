#ifndef PARSER_H
#define PARSER_H

#include "../include/types.h"

// Komut türünü belirle (loot, trade, brew, learn, etc.)
CommandType get_command_type(const char* line);

// Loot
int parse_loot_command(const char* line, Ingredient* ingredients, int* count);

// Trade
int parse_trade_command(const char* line, Trophy* trophies, int* trophy_count, Ingredient* ingredients, int* ingredient_count);

// Brew
int parse_brew_command(const char* line, char* potion_name);

// Learn effectiveness (sign or potion)
int parse_learn_effectiveness(const char* line, char* item_name, char* monster_name, int* is_sign);

// Learn potion formula
int parse_learn_formula(const char* line, PotionFormula* formula);

// Encounter
int parse_encounter_command(const char* line, char* monster_name);

// Questions
int parse_total_specific_query(const char* line, char* category, char* name);
int parse_total_all_query(const char* line, char* category);
int parse_effectiveness_query(const char* line, char* monster_name);
int parse_formula_query(const char* line, char* potion_name);

#endif
