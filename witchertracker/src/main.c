#include <stdio.h>
#include <string.h>

#include "types.h"
#include "data.h"
#include "parser.h"

int main() {
    char input[1024];
    Inventory inv;
    init_inventory(&inv);

    while (1) {
        printf(">> ");
        if (!fgets(input, sizeof(input), stdin)) break;

        // satır sonu \n varsa temizle
        input[strcspn(input, "\n")] = 0;

        CommandType type = get_command_type(input);

        if (type == CMD_EXIT) break;

        if (type == CMD_LOOT) {
            Ingredient parsed[32];
            int count;
            if (!parse_loot_command(input, parsed, &count)) {
                printf("INVALID\n");
                continue;
            }

            for (int i = 0; i < count; i++) {
                add_ingredient(&inv, parsed[i].name, parsed[i].quantity);
            }

            printf("Alchemy ingredients obtained\n");
        }
        else {
            printf("INVALID\n");
        }
    }

    return 0;
}
