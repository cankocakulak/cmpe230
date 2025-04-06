#ifndef LOGIC_H
#define LOGIC_H

#include "types.h"

// Global yapılar (örnek, daha sonra singleton'a çevrilebilir)
extern Inventory inventory;

// Fonksiyonlar
void handle_loot(Ingredient* ingredients, int count);
void handle_trade(Trophy* trophies, int trophy_count, Ingredient* ingredients, int ingredient_count);
void handle_brew(const char* potion_name);
void handle_learn_effectiveness(const char* item, const char* monster, int is_sign);
void handle_learn_formula(PotionFormula* formula);
void handle_encounter(const char* monster);
void handle_total_specific(const char* category, const char* name);
void handle_total_all(const char* category);
void handle_effectiveness_query(const char* monster);
void handle_formula_query(const char* potion_name);
void handle_total_specific_query(const char* category, const char* name);
void handle_total_all_query(const char* category);

#endif
