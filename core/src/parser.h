#pragma  once

#include <string>

#include "either.h"
#include "value.h"

namespace Inky::Lisp {

    /* Returns either an error or AST from the input string. */
    Either<ParseError,ValuePtr> parse(std::string_view in);

}