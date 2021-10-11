#pragma once

#include "types.h"

namespace inky {

    /* Main evaluate,
     * for each expression; evaluate sub-expressions
     * evaluate the top-level expression; given an environment.
     */
    either<error,value_ptr> eval(environment_ptr e, value_ptr v);
    either<error,value_ptr> eval(environment_ptr e, either<error, value_ptr> v);
}