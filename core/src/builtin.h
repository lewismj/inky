#pragma once

#include "environment.h"
#include "types.h"

namespace inky::builtin {

    /* Used on start-up, to push builtin functions into the root environment. */
    void add_builtin_functions(environment_ptr e) ;

    /* Used by eval itself. */
    either<error,value_ptr> builtin_eval(environment_ptr e, value_ptr a);
    either<error,value_ptr> builtin_list(environment_ptr e, value_ptr a);
}