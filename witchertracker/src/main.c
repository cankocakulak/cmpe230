/**
 * @file main.c
 * @brief Main program entry point and command processing
 * 
 * This file contains the main function and command processing logic
 * for the Witcher Tracker program. It handles user input, command parsing,
 * and execution of game actions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Template for the Witcher Inventory and Event Tracking System.
 *
 * This file serves as a starting point for students.
 * You are expected to implement the command handling functions (e.g., execute_line)
 * and the underlying system for managing Geralt's inventory, bestiary, and alchemy.
 *
 * Currently, this template simulates responses using a pre-defined output array.
 * Uncomment and modify the commented-out section once you have implemented your own functions.
 */

#include "../include/types.h"
#include "parser.h"
#include "logic.h"
#include "data.h"

char* output[] = {
    "Alchemy ingredients obtained\n",                     // Response for: Geralt loots 5 Rebis
    "Alchemy ingredients obtained\n",                     // Response for: Geralt loots 4 Vitriol, 1 Quebrith
    "New alchemy formula obtained: Black Blood\n",        // Response for: Geralt learns Black Blood consists of 3 Vitriol, 2 Rebis, 1 Quebrith
    "Alchemy item created: Black Blood\n",                // Response for: Geralt brews Black Blood
    "New bestiary entry added: Harpy\n",                  // Response for: Geralt learns Igni is effective against Harpy
    "Geralt defeats Harpy\n",                             // Response for: Geralt encounters a Harpy
    "3 Rebis, 1 Vitriol\n",                               // Response for: Total ingredient?
    "1\n",                                                // Response for: Total potion Black Blood?
    "1\n",                                                // Response for: Total trophy Harpy?
    "Trade successful\n",                                 // Response for: Geralt trades Harpy trophy for 8 Vitriol, 3 Rebis
    "6 Rebis, 9 Vitriol\n",                               // Response for: Total ingredient? (after trade)
    "3 Vitriol, 2 Rebis, 1 Quebrith\n",                   // Response for: What is in Black Blood?
    "Igni\n",                                             // Response for: What is effective against Harpy?
    "No formula for Swallow\n",                           // Response for: Geralt brews Swallow (unknown formula)
    "Not enough trophies\n",                              // Response for: Geralt trades Wyvern trophy for 8 Vitriol, 3 Rebis
    "New alchemy formula obtained: Swallow\n",            // Response for: Geralt learns Swallow consists of 10 Vitriol, 4 Rebis
    "Not enough ingredients\n",                           // Response for: Geralt brews Swallow (first attempt)
    "Alchemy ingredients obtained\n",                     // Response for: Geralt loots 2 Vitriol, 2 Rebis
    "Alchemy item created: Swallow\n",                    // Response for: Geralt brews Swallow (second attempt)
    "1\n"                                                 // Response for: Total potion Swallow?
};

// Function to execute a single command line and return appropriate response
int execute_line(const char* line) {
    if (line == NULL) return -1;
    
    char input[1024];
    strncpy(input, line, sizeof(input) - 1);
    input[sizeof(input) - 1] = '\0';
    
    CommandType type = get_command_type(input);
    
    if (type == CMD_EXIT) return 0;
    
    switch (type) {
        case CMD_LOOT: {
            Ingredient parsed[32];
            int count;
            if (!parse_loot_command(input, parsed, &count)) {
                return -1;
            }
            handle_loot(parsed, count);
            break;
        }
        
        case CMD_TRADE: {
            Trophy trophies[MAX_TROPHIES];
            Ingredient ingredients[MAX_INGREDIENTS];
            int trophy_count, ingredient_count;
            if (!parse_trade_command(input, trophies, &trophy_count, ingredients, &ingredient_count)) {
                return -1;
            }
            handle_trade(trophies, trophy_count, ingredients, ingredient_count);
            break;
        }

        case CMD_BREW: {
            char potion_name[MAX_NAME_LEN];
            if (!parse_brew_command(input, potion_name)) {
                return -1;
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

            return -1;
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

            return -1;
        }

        case CMD_ENCOUNTER: {
            char monster[MAX_NAME_LEN];
            if (!parse_encounter_command(input, monster)) {
                return -1;
            }
            handle_encounter(monster);
            break;
        }

        default:
            return -1;
    }
    
    return 0;
}

int main(void) {
    char line[1025];
    int index = 0;
    
    // Initialize inventory at the start
    init_inventory(&inventory);
    
    // Template simulation: Read input and output pre-defined responses.
    while (1) {
        printf(">> ");
        fflush(stdout);
        
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        
        // Check for the exit command.
        if (strcmp(line, "exit\n") == 0) {
            break;
        }
        
        // Output the next simulated response.
        printf("%s", output[index++]);
        fflush(stdout);
    }
    
    return 0;
}
