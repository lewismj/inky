#pragma once

#include <initializer_list>
#include "parser.h"
#include "eval.h"
#include "value.h"
#include "types.h"
#include "builtin.h"


    struct test_case {
        std::string expression;             /* the input lisp expression. */
        inky::value::type kind;             /* the expected output type. */
        std::variant<long, double> result;   /* variant holding the expected result. */
    };

    void verify_test_cases(inky::environment_ptr e, std::initializer_list<test_case> &tests);
