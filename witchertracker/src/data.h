// data.h
#ifndef DATA_H
#define DATA_H

#include "types.h"

// Envanteri başlat
void init_inventory(Inventory* inv);

// Ingredient işlemleri
void add_ingredient(Inventory* inv, const char* name, int quantity);
int get_ingredient_quantity(const Inventory* inv, const char* name);

// Potion işlemleri
void add_potion(Inventory* inv, const char* name, int quantity);
int get_potion_quantity(const Inventory* inv, const char* name);

// Trophy işlemleri
void add_trophy(Inventory* inv, const char* monster, int quantity);
int get_trophy_quantity(const Inventory* inv, const char* monster);

// Yazdırma işlemleri
void print_all_ingredients(const Inventory* inv);
void print_all_potions(const Inventory* inv);
void print_all_trophies(const Inventory* inv);

// Monster ve potion işlemleri
void print_monster_effectiveness(const Inventory* inv, const char* monster);
void print_potion_formula(const Inventory* inv, const char* potion);
int has_effective_counter(const Inventory* inv, const char* monster);
void consume_effective_potions(Inventory* inv, const char* monster);

#endif
