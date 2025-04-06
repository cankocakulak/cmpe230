#ifndef PARSER_H
#define PARSER_H

#include "types.h"

// Komut tipi çözümleyici (loot, brew, vs.)
CommandType get_command_type(const char* line);

// Loot komutu için ingredient'ları ayrıştır
int parse_loot_command(const char* line, Ingredient* ingredients, int* count);

#endif
