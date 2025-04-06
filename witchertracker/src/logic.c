#include "logic.h"
#include "data.h"
#include <stdio.h>
#include <string.h>

// Global inventory
Inventory inventory;

// Dummy formula & bestiary veri yapılarını projeye yayacağız ama basitçe buraya gömüyoruz
PotionFormula alchemy[32];
int formula_count = 0;

BestiaryEntry bestiary[32];
int bestiary_count = 0;

// --- LOOT ---
void handle_loot(Ingredient* ingredients, int count) {
    for (int i = 0; i < count; i++) {
        add_ingredient(&inventory, ingredients[i].name, ingredients[i].quantity);
    }
    printf("Alchemy ingredients obtained\n");
}

// --- BREW ---
void handle_brew(const char* potion_name) {
    // Check if we have the formula
    PotionFormula* formula = NULL;
    for (int i = 0; i < inventory.formula_count; i++) {
        if (strcmp(inventory.formulas[i].name, potion_name) == 0) {
            formula = &inventory.formulas[i];
            break;
        }
    }
    if (!formula) {
        printf("No formula for %s\n", potion_name);
        return;
    }

    // Check if we have enough ingredients
    for (int i = 0; i < formula->ingredient_count; i++) {
        int have = get_ingredient_quantity(&inventory, formula->ingredients[i].name);
        int need = formula->ingredients[i].quantity;
        printf("DEBUG: Checking %s - have: %d, need: %d\n", 
               formula->ingredients[i].name, have, need);
        if (have < need) {
            printf("Not enough ingredients (have %d %s, need %d)\n", 
                  have, formula->ingredients[i].name, need);
            return;
        }
    }

    // Consume ingredients
    for (int i = 0; i < formula->ingredient_count; i++) {
        add_ingredient(&inventory, formula->ingredients[i].name, -formula->ingredients[i].quantity);
        printf("DEBUG: Consumed %d %s\n", 
               formula->ingredients[i].quantity, formula->ingredients[i].name);
    }

    // Create potion
    add_potion(&inventory, potion_name, 1);
    printf("Alchemy item created: %s\n", potion_name);
}

// --- LEARN: formula ---
void handle_learn_formula(PotionFormula* formula) {
    for (int i = 0; i < inventory.formula_count; i++) {
        if (strcmp(inventory.formulas[i].name, formula->name) == 0) {
            printf("Already known formula\n");
            return;
        }
    }

    inventory.formulas[inventory.formula_count++] = *formula;
    printf("New alchemy formula obtained: %s\n", formula->name);
}

// --- LEARN: effectiveness ---
void handle_learn_effectiveness(const char* item, const char* monster, int is_sign) {
    MonsterCounter* entry = NULL;
    for (int i = 0; i < inventory.bestiary_count; i++) {
        if (strcmp(inventory.bestiary[i].monster_name, monster) == 0) {
            entry = &inventory.bestiary[i];
            break;
        }
    }

    if (!entry) {
        strcpy(inventory.bestiary[inventory.bestiary_count].monster_name, monster);
        entry = &inventory.bestiary[inventory.bestiary_count++];
        strcpy(entry->counters[entry->counter_count++], item);
        printf("New bestiary entry added: %s\n", monster);
        return;
    }

    // Zaten biliniyor mu?
    for (int i = 0; i < entry->counter_count; i++) {
        if (strcmp(entry->counters[i], item) == 0) {
            printf("Already known effectiveness\n");
            return;
        }
    }

    strcpy(entry->counters[entry->counter_count++], item);
    printf("Bestiary entry updated: %s\n", monster);
}

void handle_total_specific_query(const char* category, const char* name) {
    if (strcmp(category, "ingredient") == 0) {
        int quantity = get_ingredient_quantity(&inventory, name);
        printf("%d\n", quantity);
    } else if (strcmp(category, "potion") == 0) {
        int quantity = get_potion_quantity(&inventory, name);
        printf("%d\n", quantity);
    } else if (strcmp(category, "trophy") == 0) {
        int quantity = get_trophy_quantity(&inventory, name);
        printf("%d\n", quantity);
    } else {
        printf("INVALID\n");
    }
}

void handle_total_all_query(const char* category) {
    if (strcmp(category, "ingredient") == 0) {
        if (inventory.ingredient_count == 0) {
            printf("0\n");
        } else {
            print_all_ingredients(&inventory);
        }
    } else if (strcmp(category, "potion") == 0) {
        if (inventory.potion_count == 0) {
            printf("0\n");
        } else {
            print_all_potions(&inventory);
        }
    } else if (strcmp(category, "trophy") == 0) {
        if (inventory.trophy_count == 0) {
            printf("0\n");
        } else {
            print_all_trophies(&inventory);
        }
    } else {
        printf("INVALID\n");
    }
}

void handle_effectiveness_query(const char* monster) {
    print_monster_effectiveness(&inventory, monster);
}

void handle_formula_query(const char* potion) {
    print_potion_formula(&inventory, potion);
}

void handle_encounter(const char* monster) {
    if (has_effective_counter(&inventory, monster)) {
        printf("Geralt defeats %s\n", monster);
        add_trophy(&inventory, monster, 1);
        consume_effective_potions(&inventory, monster);
    } else {
        printf("Geralt is unprepared and barely escapes with his life\n");
    }
}

void handle_trade(Trophy* trophies, int trophy_count, Ingredient* ingredients, int ingredient_count) {
    // Check if we have enough trophies
    for (int i = 0; i < trophy_count; i++) {
        int have = get_trophy_quantity(&inventory, trophies[i].monster_name);
        int need = trophies[i].quantity;
        if (have < need) {
            printf("Not enough trophies\n");
            return;
        }
    }
    
    // Remove trophies and add ingredients
    for (int i = 0; i < trophy_count; i++) {
        add_trophy(&inventory, trophies[i].monster_name, -trophies[i].quantity);
    }
    
    for (int i = 0; i < ingredient_count; i++) {
        add_ingredient(&inventory, ingredients[i].name, ingredients[i].quantity);
    }
    
    printf("Trade successful\n");
}

// Diğer fonksiyonları istersen sonra ekleriz (trade, encounter, queries)

