#include <initializer_list>
#include <catch2/catch.hpp>

#include "test_util.h"


TEST_CASE("evaluating numerical expressions","[basic-eval-1]") {
    using namespace inky;

    environment_ptr e(new environment());
    builtin::add_builtin_functions(e);

    std::initializer_list<test_case> tests  = {
            { "486", value::type::Integer, 486L },
            { "(+ 137 349)", value::type::Integer, 486L},
            { "(* 5 99)", value::type::Integer, 495L},
            { "(/ 10 2)", value::type::Integer, 5L},
            { "(+ 2.7 10)", value::type::Double, 12.7},
            { "(* 25 4 12)", value::type::Integer, 1200L},
            { "(+ 21 35 12  7)", value::type::Integer, 75L},
            { "(+ (* 3 (+ (* 2 4) (+ 3 5))) (+ (- 10 7) 6))", value::type::Integer, 57}
    };

    verify_test_cases(e,tests);
 }

TEST_CASE("min/max on numeric s-expressions.","[basic-eval-2]") {
    using namespace inky;

    environment_ptr e(new environment());
    builtin::add_builtin_functions(e);

    std::initializer_list<test_case> tests  = {
            { "(min (* 6 -6) 2 3 4)", value::type::Integer, -36 },
            { "(max (* 6 6) 2 3 4)", value::type::Integer, 36 },
            { "(max 1 2 30.2 4)", value::type::Double, 30.2 },
            { "(min 1 2 -30.2 4)", value::type::Double, -30.2 }
    };

    verify_test_cases(e,tests);
}