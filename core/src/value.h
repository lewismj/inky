#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <vector>
#include "types.h"


namespace inky {

    /* Forward declarations. */
    class value;

    class environment;

    /* A value (Lisp value in this context) is a dynamically-typed hierarchical data structure
     * that represents ast node for s-expression(s).
     */
    struct value {
        enum class type { /* primitives, n.b. prelude itself will bootstrap other types. */
            Integer,
            Double,
            String,
            Symbol,
            Function,
            SExpression,
            QExpression /* placeholder for quoted s-expression. */
        };

        explicit value(const long l) : kind(type::Integer) { var = l; }
        explicit value(const double d) : kind(type::Double) { var = d; }
        explicit value(const std::string &s) : kind(type::Symbol) { var = s; }
        explicit value(const std::string &s, bool literal) {
            kind = literal ? type::String : type::Symbol;
            var = s;
        }
        explicit value(const function &f) : kind(type::Function) { var = f; }
        explicit value(type t) { kind = t; }

        ~value();

        void insert(value *v); /* insert a value into this (cell). */
        void move(value *v); /* move cells of v into this & delete v. */

        bool is_numeric() const { return kind == type::Double || kind == type::Integer; }

        type kind; /* The type of the value, basic type. */

        /* TODO - Support for lambda expressions. */

        /* 'stack' values. */
        std::variant<long, double, std::string, function> var;

        std::vector<value *> cells;       /* s-expression values. */
        std::shared_ptr<environment> env; /* lambda environment. */
    };
}
