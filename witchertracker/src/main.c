/**
 * @file main.c
 * @brief Main program entry point and command processing
 * 
 * This file contains the main function and command processing logic
 * for the Witcher Tracker program. It handles user input, command parsing,
 * and execution of game actions.
 */

#include <stdio.h>
#include <string.h>

#include "types.h"
#include "parser.h"
#include "logic.h"
#include "data.h"

/**
 * @brief Main function that processes game commands
 * 
 * Reads commands from stdin, parses them, and executes the appropriate
 * game actions. Continues until an exit command is received.
 * 
 * @return 0 on successful execution
 */
int main() {
    char input[1024];

    // Inventory başlangıcı
    init_inventory(&inventory);

    while (1) {
        printf(">> ");
        if (!fgets(input, sizeof(input), stdin)) break;

        input[strcspn(input, "\n")] = 0; // \n temizle

        CommandType type = get_command_type(input);

        if (type == CMD_EXIT) break;

        switch (type) {
            case CMD_LOOT: {
                Ingredient parsed[32];
                int count;
                if (!parse_loot_command(input, parsed, &count)) {
                    printf("INVALID\n");
                    continue;
                }
                handle_loot(parsed, count);
                break;
            }

            case CMD_TRADE: {
                Trophy trophies[MAX_TROPHIES];
                Ingredient ingredients[MAX_INGREDIENTS];
                int trophy_count, ingredient_count;
                if (!parse_trade_command(input, trophies, &trophy_count, ingredients, &ingredient_count)) {
                    printf("INVALID\n");
                    continue;
                }
                handle_trade(trophies, trophy_count, ingredients, ingredient_count);
                break;
            }

            case CMD_BREW: {
                char potion_name[MAX_NAME_LEN];
                if (!parse_brew_command(input, potion_name)) {
                    printf("INVALID\n");
                    continue;
                }
                handle_brew(potion_name);
                break;
            }

            case CMD_LEARN: {
                // Try formula first
                PotionFormula formula;
                if (parse_learn_formula(input, &formula)) {
                    handle_learn_formula(&formula);
                    break;
                }

                // Then try effectiveness
                char item[MAX_NAME_LEN], monster[MAX_NAME_LEN];
                int is_sign;
                if (parse_learn_effectiveness(input, item, monster, &is_sign)) {
                    handle_learn_effectiveness(item, monster, is_sign);
                    break;
                }

                printf("INVALID\n");
                break;
            }

            case CMD_QUERY: {
                // Try total specific query first
                char category[MAX_NAME_LEN], name[MAX_NAME_LEN];
                if (parse_total_specific_query(input, category, name)) {
                    handle_total_specific_query(category, name);
                    break;
                }

                // Then try total all query
                if (parse_total_all_query(input, category)) {
                    handle_total_all_query(category);
                    break;
                }

                // Then try formula query
                char potion[MAX_NAME_LEN];
                if (parse_formula_query(input, potion)) {
                    handle_formula_query(potion);
                    break;
                }

                // Finally try effectiveness query
                char monster[MAX_NAME_LEN];
                if (parse_effectiveness_query(input, monster)) {
                    handle_effectiveness_query(monster);
                    break;
                }

                printf("INVALID\n");
                break;
            }

            case CMD_ENCOUNTER: {
                char monster[MAX_NAME_LEN];
                if (!parse_encounter_command(input, monster)) {
                    printf("INVALID\n");
                    continue;
                }
                handle_encounter(monster);
                break;
            }

            default:
                printf("INVALID\n");
        }
    }

    return 0;
}
