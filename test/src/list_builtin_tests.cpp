#include <catch2/catch.hpp>

/* This 'list' primitives are used for SExpression (And quoted SExpression, QExpression)
 * manipulation, required to support lambdas and functions. */

/* n.b. in these operators we return errors on head or tail of empty lists etc.
 * they're used for argument calling; list types can be fully implemented in a prelude,
 * which may offer different semantics. */


#include "test_util.h"
#include "builtin.h"

TEST_CASE("builtin list primitives","[basic-list-1]") {
    using namespace Inky::Lisp;

    EnvironmentPtr e(new Environment());
    addBuiltinFunctions(e);

    std::initializer_list<TestCase> tests  = {
            { "eval (head [10 9 8 7])", Type::Integer, 10 },
            { "eval [+ 10 1]", Type::Integer, 11},
            { "eval (head (tail [1 2 3 4]))", Type::Integer, 2},
            { "eval (head (list 1 2 3 4))", Type::Integer, 1}
    };

    verifyTestCases(e, tests);
}