#include <initializer_list>
#include <catch2/catch.hpp>

#include "builtin.h"
#include "environment.h"
#include "value.h"
#include "test_util.h"


TEST_CASE("evaluating numerical expressions","[basic-eval-1]") {
    using namespace Inky::Lisp;

    EnvironmentPtr e(new Environment());
    addBuiltinFunctions(e);

    std::initializer_list<TestCase> tests  = {
            { "486", Type::Integer, 486L },
            { "(+ 137 349)", Type::Integer, 486L},
            { "(* 5 99)", Type::Integer, 495L},
            { "(/ 10 2)", Type::Integer, 5L},
            { "(+ 2.7 10)", Type::Double, 12.7},
            { "(* 25 4 12)", Type::Integer, 1200L},
            { "(+ 21 35 12  7)", Type::Integer, 75L},
            { "(+ (* 3 (+ (* 2 4) (+ 3 5))) (+ (- 10 7) 6))", Type::Integer, 57}
    };

    verifyTestCases(e,tests);
 }

TEST_CASE("min/max on numeric s-expressions.","[basic-eval-2]") {
    using namespace Inky::Lisp;

    EnvironmentPtr e(new Environment());
    addBuiltinFunctions(e);

    std::initializer_list<TestCase> tests  = {
            { "(min (* 6 -6) 2 3 4)", Type::Integer, -36 },
            { "(max (* 6 6) 2 3 4)", Type::Integer, 36 },
            { "(max 1 2 30.2 4)", Type::Double, 30.2 },
            { "(min 1 2 -30.2 4)", Type::Double, -30.2 }
    };

    verifyTestCases(e,tests);
}