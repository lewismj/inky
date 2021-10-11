#include <catch2/catch.hpp>
#include "test_util.h"


void verify_test_cases(inky::environment_ptr e, std::initializer_list<test_case> &tests) {
    for (const auto &test: tests) {
        auto result = eval(e, inky::parse(test.expression));
        REQUIRE(result.is_right());
        if (result.is_right()) {
            inky::value_ptr val = result.right_value();
            REQUIRE(val->is_numeric());
            REQUIRE(val->kind == test.kind);
            if (val->kind == inky::value::type::Integer) {
                REQUIRE(std::get<long>(val->var) == std::get<long>(test.result));
            } else {
                REQUIRE(std::get<double>(val->var) == std::get<double>(test.result));
            }
        }
    }
}

