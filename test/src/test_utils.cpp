#include <catch2/catch.hpp>
#include "test_util.h"

#include "parser.h"
#include "eval.h"

using namespace Inky::Lisp;


void verifyTestCases(Inky::Lisp::EnvironmentPtr e, std::initializer_list<TestCase> &tests) {
    for (const auto &test: tests) {
        REQUIRE(parse(test.expression).isRight());
        auto result = eval(e, parse(test.expression).right());
        REQUIRE(result.isRight());
        if (result.isRight()) {
            Inky::Lisp::ValuePtr val = result.right();
            REQUIRE(Ops::isNumeric(val));
            REQUIRE(val->kind == test.kind);
            if (val->kind == Type::Integer) {
                REQUIRE(std::get<long>(val->var) == std::get<long>(test.result));
            } else {
                REQUIRE(std::get<double>(val->var) == std::get<double>(test.result));
            }
        }
    }
}

