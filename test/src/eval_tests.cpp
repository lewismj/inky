#include <initializer_list>
#include <catch2/catch.hpp>

#include "parser.h"
#include "eval.h"
#include "value.h"
#include "types.h"
#include "builtin.h"


TEST_CASE("basic eval checks","[basic-eval]") {
    using namespace inky;

    environment_ptr e(new environment());
    builtin::add_builtin_functions(e);

    /* define struct for basic numerical expression tests. */
    struct expression_test {
        std::string expression;             /* the input lisp expression. */
        value::type kind;                   /* the expected output type. */
        std::variant<long,double> result;   /* variant holding the expected result. */
    };

    std::initializer_list<expression_test> tests  = {
            { "486", value::type::Integer, 486L },
            { "(+ 137 349)", value::type::Integer, 486L},
            { "(* 5 99)", value::type::Integer, 495L},
            { "(/ 10 2)", value::type::Integer, 5L},
            { "(+ 2.7 10)", value::type::Double, 12.7},
            { "(* 25 4 12)", value::type::Integer, 1200L},
            { "(+ 21 35 12  7)", value::type::Integer, 75L},
            { "(+ (* 3 (+ (* 2 4) (+ 3 5))) (+ (- 10 7) 6))", value::type::Integer, 57}
    };

    for (const auto& test: tests) {
        auto result = eval(e, inky::parse(test.expression));
        REQUIRE(result.is_right());
        if ( result.is_right() ) {
            value_ptr val = result.right_value();
            REQUIRE(val->is_numeric());
            REQUIRE(val->kind == test.kind);
            if ( val->kind == value::type::Integer) {
               REQUIRE(std::get<long>(val->var) == std::get<long>(test.result));
            } else {
               REQUIRE(std::get<double>(val->var) == std::get<double>(test.result));
            }
        }
    }


 }