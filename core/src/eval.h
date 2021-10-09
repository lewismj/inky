#pragma once

#include "types.h"

namespace inky {

    /* Main evaluate,
     * for each expression; evaluate sub-expressions
     * evaluate the top-level expression; given an environment.
     */
    either<error,value_ptr> eval(environment_ptr e, value_ptr v);

    // utility; todo- implement right_map, left_map on the either type.
    either<error,value_ptr> eval(environment_ptr e, either<error, value_ptr> v);
}