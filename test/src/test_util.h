#pragma once

#include <initializer_list>
#include <variant>

#include "value.h"
#include "environment.h"

using namespace Inky::Lisp;

    struct TestCase {
        std::string expression;             /* the input lisp expression. */
        Type kind;             /* the expected output type. */
        std::variant<long, double> result;   /* variant holding the expected result. */
    };

    void verifyTestCases(EnvironmentPtr e, std::initializer_list<TestCase> &tests);
