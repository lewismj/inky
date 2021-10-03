#pragma once

#include <cstdlib>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include "parser_t.h"


namespace inky::parser {

    class value; /* forward declaration of value. */

    /* The lexer, take the input string and create a vector of token. */
    either<error,std::vector<value*>> parse(std::string_view input);


}