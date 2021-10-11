#include <catch2/catch.hpp>

/* This 'list' primitives are used for SExpression (And quoted SExpression, QExpression)
 * manipulation, required to support lambdas and functions. */

/* n.b. in these operators we return errors on head or tail of empty lists etc.
 * they're used for argument calling; list types can be fully implemented in a prelude,
 * which may offer different semantics. */


#include "test_util.h"


TEST_CASE("builtin list primitives","[basic-list-1]") {
    using namespace inky;

    environment_ptr e(new environment());
    builtin::add_builtin_functions(e);

    std::initializer_list<test_case> tests  = {
            { "head [10 9 8 7]", value::type::Integer, 10 },
            { "eval [+ 10 1]", value::type::Integer, 11},
            { "(head (tail [1 2 3 4]))", value::type::Integer, 2},
            { "(head (list 1 2 3 4))", value::type::Integer, 1}
    };

    verify_test_cases(e,tests);
}