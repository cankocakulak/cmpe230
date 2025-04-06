// data.c
#include "data.h"
#include <string.h>
#include <stdio.h>

void init_inventory(Inventory* inv) {
    inv->ingredient_count = 0;
    inv->potion_count = 0;
    inv->trophy_count = 0;
    inv->formula_count = 0;
    inv->bestiary_count = 0;
}

// Ingredient ekleme
void add_ingredient(Inventory* inv, const char* name, int quantity) {
    // Find existing ingredient
    for (int i = 0; i < inv->ingredient_count; i++) {
        if (strcmp(inv->ingredients[i].name, name) == 0) {
            inv->ingredients[i].quantity += quantity;
            return;
        }
    }
    
    // Add new ingredient
    if (inv->ingredient_count < MAX_INGREDIENTS) {
        strcpy(inv->ingredients[inv->ingredient_count].name, name);
        inv->ingredients[inv->ingredient_count].quantity = quantity;
        inv->ingredient_count++;
    }
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
    int count = 0;
    for (int i = 0; i < inv->ingredient_count; ++i) {
        if (inv->ingredients[i].quantity > 0) {
            if (count > 0) printf(", ");
            printf("%d %s", inv->ingredients[i].quantity, inv->ingredients[i].name);
            count++;
        }
    }
    if (count == 0) printf("0");
    printf("\n");
}

void print_all_potions(const Inventory* inv) {
    int count = 0;
    for (int i = 0; i < inv->potion_count; i++) {
        if (inv->potions[i].quantity > 0) {
            if (count > 0) printf(", ");
            printf("%d %s", inv->potions[i].quantity, inv->potions[i].name);
            count++;
        }
    }
    if (count == 0) printf("0");
    printf("\n");
}

void print_all_trophies(const Inventory* inv) {
    int count = 0;
    for (int i = 0; i < inv->trophy_count; i++) {
        if (inv->trophies[i].quantity > 0) {
            if (count > 0) printf(", ");
            printf("%d %s", inv->trophies[i].quantity, inv->trophies[i].monster_name);
            count++;
        }
    }
    if (count == 0) printf("0");
    printf("\n");
}

void print_monster_effectiveness(const Inventory* inv, const char* monster) {
    int found = 0;
    for (int i = 0; i < inv->bestiary_count; i++) {
        if (strcmp(inv->bestiary[i].monster_name, monster) == 0) {
            found = 1;
            int count = 0;
            for (int j = 0; j < inv->bestiary[i].counter_count; j++) {
                if (count > 0) printf(", ");
                printf("%s", inv->bestiary[i].counters[j]);
                count++;
            }
            printf("\n");
            break;
        }
    }
    if (!found) printf("No knowledge of %s\n", monster);
}

void print_potion_formula(const Inventory* inv, const char* potion) {
    int found = 0;
    for (int i = 0; i < inv->formula_count; i++) {
        if (strcmp(inv->formulas[i].name, potion) == 0) {
            found = 1;
            
            // Create a temporary array for sorting
            Ingredient sorted[MAX_INGREDIENTS];
            int count = inv->formulas[i].ingredient_count;
            
            // Copy ingredients to temporary array
            for (int j = 0; j < count; j++) {
                sorted[j] = inv->formulas[i].ingredients[j];
            }
            
            // Sort ingredients by quantity (bubble sort)
            for (int j = 0; j < count - 1; j++) {
                for (int k = 0; k < count - j - 1; k++) {
                    if (sorted[k].quantity < sorted[k + 1].quantity ||
                        (sorted[k].quantity == sorted[k + 1].quantity &&
                         strcmp(sorted[k].name, sorted[k + 1].name) > 0)) {
                        // Swap
                        Ingredient temp = sorted[k];
                        sorted[k] = sorted[k + 1];
                        sorted[k + 1] = temp;
                    }
                }
            }
            
            // Print sorted ingredients
            for (int j = 0; j < count; j++) {
                if (j > 0) printf(", ");
                printf("%d %s", sorted[j].quantity, sorted[j].name);
            }
            printf("\n");
            break;
        }
    }
    if (!found) printf("No formula for %s\n", potion);
}

int has_effective_counter(const Inventory* inv, const char* monster) {
    // Find monster in bestiary
    for (int i = 0; i < inv->bestiary_count; i++) {
        if (strcmp(inv->bestiary[i].monster_name, monster) == 0) {
            // Check if we have any of the effective counters
            for (int j = 0; j < inv->bestiary[i].counter_count; j++) {
                const char* counter = inv->bestiary[i].counters[j];
                // Check if it's a sign (always available) or if we have the potion
                if (strcmp(counter, "Aard") == 0 || strcmp(counter, "Igni") == 0 ||
                    strcmp(counter, "Yrden") == 0 || strcmp(counter, "Quen") == 0 ||
                    strcmp(counter, "Axii") == 0 || get_potion_quantity(inv, counter) > 0) {
                    return 1;
                }
            }
            return 0;
        }
    }
    return 0;
}

void consume_effective_potions(Inventory* inv, const char* monster) {
    // Find monster in bestiary
    for (int i = 0; i < inv->bestiary_count; i++) {
        if (strcmp(inv->bestiary[i].monster_name, monster) == 0) {
            // Consume one of each effective potion we have
            for (int j = 0; j < inv->bestiary[i].counter_count; j++) {
                const char* counter = inv->bestiary[i].counters[j];
                // Skip signs and only consume potions
                if (strcmp(counter, "Aard") != 0 && strcmp(counter, "Igni") != 0 &&
                    strcmp(counter, "Yrden") != 0 && strcmp(counter, "Quen") != 0 &&
                    strcmp(counter, "Axii") != 0 && get_potion_quantity(inv, counter) > 0) {
                    add_potion(inv, counter, -1);
                }
            }
            break;
        }
    }
}
