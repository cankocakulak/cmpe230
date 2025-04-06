// data.c
#include "data.h"
#include <string.h>
#include <stdio.h>

void init_inventory(Inventory* inv) {
    inv->ingredient_count = 0;
    inv->potion_count = 0;
    inv->trophy_count = 0;
}

// Ingredient ekleme
void add_ingredient(Inventory* inv, const char* name, int quantity) {
    for (int i = 0; i < inv->ingredient_count; ++i) {
        if (strcmp(inv->ingredients[i].name, name) == 0) {
            inv->ingredients[i].quantity += quantity;
            return;
        }
    }
    // Yeni ekleme
    strcpy(inv->ingredients[inv->ingredient_count].name, name);
    inv->ingredients[inv->ingredient_count].quantity = quantity;
    inv->ingredient_count++;
}

int get_ingredient_quantity(const Inventory* inv, const char* name) {
    for (int i = 0; i < inv->ingredient_count; ++i) {
        if (strcmp(inv->ingredients[i].name, name) == 0) {
            return inv->ingredients[i].quantity;
        }
    }
    return 0;
}

// Potion ekleme
void add_potion(Inventory* inv, const char* name, int quantity) {
    for (int i = 0; i < inv->potion_count; ++i) {
        if (strcmp(inv->potions[i].name, name) == 0) {
            inv->potions[i].quantity += quantity;
            return;
        }
    }
    // Yeni ekleme
    strcpy(inv->potions[inv->potion_count].name, name);
    inv->potions[inv->potion_count].quantity = quantity;
    inv->potion_count++;
}

int get_potion_quantity(const Inventory* inv, const char* name) {
    for (int i = 0; i < inv->potion_count; ++i) {
        if (strcmp(inv->potions[i].name, name) == 0) {
            return inv->potions[i].quantity;
        }
    }
    return 0;
}

// Trophy ekleme
void add_trophy(Inventory* inv, const char* monster, int quantity) {
    for (int i = 0; i < inv->trophy_count; ++i) {
        if (strcmp(inv->trophies[i].monster_name, monster) == 0) {
            inv->trophies[i].quantity += quantity;
            return;
        }
    }
    // Yeni ekleme
    strcpy(inv->trophies[inv->trophy_count].monster_name, monster);
    inv->trophies[inv->trophy_count].quantity = quantity;
    inv->trophy_count++;
}

int get_trophy_quantity(const Inventory* inv, const char* monster) {
    for (int i = 0; i < inv->trophy_count; ++i) {
        if (strcmp(inv->trophies[i].monster_name, monster) == 0) {
            return inv->trophies[i].quantity;
        }
    }
    return 0;
}

// Sadece örnek çıktı için
void print_all_ingredients(const Inventory* inv) {
    for (int i = 0; i < inv->ingredient_count; ++i) {
        printf("%d %s\n", inv->ingredients[i].quantity, inv->ingredients[i].name);
    }
}
