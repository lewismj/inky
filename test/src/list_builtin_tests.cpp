#include <catch2/catch.hpp>

/* This 'list' primitives are used for SExpression (And quoted SExpression, QExpression)
 * manipulation, required to support lambdas and functions. */

/* n.b. in these operators we return errors on head or tail of empty lists etc.
 * they're used for argument calling; list types can be fully implemented in a prelude,
 * which may offer different semantics. */


TEST_CASE("builtin list primitives","[basic-list-1]") {

}