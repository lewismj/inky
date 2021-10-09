#pragma once

#include "environment.h"
#include "types.h"

namespace inky::builtin {

    /* Used on start-up, to push builtin functions into the root environment. */
    void add_builtin_functions(environment_ptr e) ;

}