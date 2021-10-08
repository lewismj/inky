#include <catch2/catch.hpp>

#include "parser.h"
#include "value.h"

/*
 * Note, for convenience, the REPL will encase the input in an S-Expression:
 * So, that, for example:
 *    + 2 2
 * Is valid, i.e. turned into (+ 2 2) and evaluated,
 *
 * So, (+2 2)  Would be parsed as, ( (+2 2) )
 */
TEST_CASE("basic parser checks","[basic-parser]") {
}