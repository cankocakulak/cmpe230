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
    // Formula var mı?
    PotionFormula* formula = NULL;
    for (int i = 0; i < formula_count; i++) {
        if (strcmp(alchemy[i].name, potion_name) == 0) {
            formula = &alchemy[i];
            break;
        }
    }
    if (!formula) {
        printf("No formula for %s\n", potion_name);
        return;
    }

    // Malzemeler yeterli mi?
    for (int i = 0; i < formula->ingredient_count; i++) {
        if (get_ingredient_quantity(&inventory, formula->ingredients[i].name) < formula->ingredients[i].quantity) {
            printf("Not enough ingredients\n");
            return;
        }
    }

    // Malzemeleri tüket
    for (int i = 0; i < formula->ingredient_count; i++) {
        add_ingredient(&inventory, formula->ingredients[i].name, -formula->ingredients[i].quantity);
    }

    add_potion(&inventory, potion_name, 1);
    printf("Alchemy item created: %s\n", potion_name);
}

// --- LEARN: formula ---
void handle_learn_formula(PotionFormula* formula) {
    for (int i = 0; i < formula_count; i++) {
        if (strcmp(alchemy[i].name, formula->name) == 0) {
            printf("Already known formula\n");
            return;
        }
    }

    alchemy[formula_count++] = *formula;
    printf("New alchemy formula obtained: %s\n", formula->name);
}

// --- LEARN: effectiveness ---
void handle_learn_effectiveness(const char* item, const char* monster, int is_sign) {
    BestiaryEntry* entry = NULL;
    for (int i = 0; i < bestiary_count; i++) {
        if (strcmp(bestiary[i].monster, monster) == 0) {
            entry = &bestiary[i];
            break;
        }
    }

    if (!entry) {
        strcpy(bestiary[bestiary_count].monster, monster);
        entry = &bestiary[bestiary_count++];
        if (is_sign) {
            strcpy(entry->effective_signs[entry->sign_count++], item);
        } else {
            strcpy(entry->effective_potions[entry->potion_count++], item);
        }
        printf("New bestiary entry added: %s\n", monster);
        return;
    }

    // Zaten biliniyor mu?
    char (*arr)[MAX_NAME_LEN] = is_sign ? entry->effective_signs : entry->effective_potions;
    int* count = is_sign ? &entry->sign_count : &entry->potion_count;

    for (int i = 0; i < *count; i++) {
        if (strcmp(arr[i], item) == 0) {
            printf("Already known effectiveness\n");
            return;
        }
    }

    strcpy(arr[(*count)++], item);
    printf("Bestiary entry updated: %s\n", monster);
}

// Diğer fonksiyonları istersen sonra ekleriz (trade, encounter, queries)

