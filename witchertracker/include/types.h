// types.h
#ifndef TYPES_H
#define TYPES_H

#define MAX_NAME_LEN 64
#define MAX_INGREDIENTS 32
#define MAX_POTIONS 32
#define MAX_TROPHIES 32
#define MAX_SIGNS 10
#define MAX_EFFECTIVE_ITEMS 10
#define MAX_FORMULAS 32
#define MAX_BESTIARY 32
#define MAX_COUNTERS 10

// Ingredient
typedef struct {
    char name[MAX_NAME_LEN];
    int quantity;
} Ingredient;

// Potion
typedef struct {
    char name[MAX_NAME_LEN];
    int quantity;
} Potion;

// Trophy (Monster adıyla aynı)
typedef struct {
    char monster_name[MAX_NAME_LEN];
    int quantity;
} Trophy;

// Potion Formula
typedef struct {
    char name[MAX_NAME_LEN];
    Ingredient ingredients[MAX_INGREDIENTS];
    int ingredient_count;
} PotionFormula;

// Monster Counter
typedef struct {
    char monster_name[MAX_NAME_LEN];
    char counters[MAX_COUNTERS][MAX_NAME_LEN];
    int counter_count;
} MonsterCounter;

// Bestiary entry
typedef struct {
    char monster[MAX_NAME_LEN];
    char effective_potions[MAX_EFFECTIVE_ITEMS][MAX_NAME_LEN];
    int potion_count;
    char effective_signs[MAX_EFFECTIVE_ITEMS][MAX_NAME_LEN];
    int sign_count;
} BestiaryEntry;

// Envanter
typedef struct {
    Ingredient ingredients[MAX_INGREDIENTS];
    int ingredient_count;

    Potion potions[MAX_POTIONS];
    int potion_count;

    Trophy trophies[MAX_TROPHIES];
    int trophy_count;

    PotionFormula formulas[MAX_FORMULAS];
    int formula_count;

    MonsterCounter bestiary[MAX_BESTIARY];
    int bestiary_count;
} Inventory;

// Komut tipi (parsing sonucu lazım olacak)
typedef enum {
    CMD_INVALID,
    CMD_LOOT,
    CMD_TRADE,
    CMD_BREW,
    CMD_LEARN,
    CMD_ENCOUNTER,
    CMD_QUERY,
    CMD_EXIT
} CommandType;

#endif
