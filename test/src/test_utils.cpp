#include <catch2/catch.hpp>
#include "test_util.h"

#include "parser.h"
#include "eval.h"

using namespace Inky::Lisp;


void verifyTestCases(Inky::Lisp::EnvironmentPtr e, std::initializer_list<TestCase> &tests) {
    for (const auto &test: tests) {
        REQUIRE(parse(test.expression).isRight());
        auto result = eval(e, parse(test.expression).right());
        REQUIRE(!Ops::isError(result));
            REQUIRE(Ops::isNumeric(result));
            REQUIRE(result->kind == test.kind);
            if (result->kind == Type::Integer) {
                REQUIRE(std::get<long>(result->var) == std::get<long>(test.result));
            } else {
                REQUIRE(std::get<double>(result->var) == std::get<double>(test.result));
            }
    }
}

