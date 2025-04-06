#include <stdio.h>
#include <string.h>

#include "types.h"
#include "parser.h"
#include "logic.h"
#include "data.h"

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
                // Try formula query first
                char potion[MAX_NAME_LEN];
                if (parse_formula_query(input, potion)) {
                    handle_formula_query(potion);
                    break;
                }

                // Try total specific query
                char category[MAX_NAME_LEN], name[MAX_NAME_LEN];
                if (parse_total_specific_query(input, category, name)) {
                    handle_total_specific_query(category, name);
                    break;
                }

                // Try total all query
                if (parse_total_all_query(input, category)) {
                    handle_total_all_query(category);
                    break;
                }

                // Try effectiveness query
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
