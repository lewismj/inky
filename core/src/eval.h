#pragma once

#include "types.h"

namespace inky::eval {

    /* Main evaluate,
     * for each expression; evaluate sub-expressions
     * evaluate the top-level expression; given an environment.
     */
    either<error,value*> eval(std::shared_ptr<environment> e, value* v);

}