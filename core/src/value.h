#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <deque>
#include "types.h"


namespace inky {

    /* Forward declarations. */
    class value;

    class environment;

    /* A value (Lisp value in this context) is a dynamically-typed hierarchical data structure
     * that represents ast node for s-expression(s). */
    struct value {
        /* primitives, n.b. prelude should bootstrap proper types;  */
        enum class type {
            Integer,
            Double,
            String,
            Symbol,
            BuiltinFunction,
            Function,
            SExpression,
            QExpression /* quoted s-expression / lambda. */
        };

        explicit value(const long l) : kind(type::Integer) { var = l; }
        explicit value(const double d) : kind(type::Double) { var = d; }
        explicit value(const std::string &s) : kind(type::Symbol) { var = s; }
        explicit value(const std::string &s, bool literal) {
            kind = literal ? type::String : type::Symbol;
            var = s;
        }
        explicit value(const function &f) : kind(type::Function) { var = f; }
        explicit value(const function& f, bool builtin) {
            kind = builtin ? type::BuiltinFunction : type::Function;
            var = f;
        }
        explicit value(type t) { kind = t; }

        ~value() = default;

        void insert(value_ptr v);               /* insert a value into this (cell). */
        void move(value_ptr v);                 /* move cells of v into this & delete v. */

        [[nodiscard]] bool is_numeric() const { return kind == type::Double || kind == type::Integer; }
        [[nodiscard]] bool is_function() const  { return kind == type::Function || kind == type::BuiltinFunction; }

        type kind; /* The type of the value, basic type. */

        std::variant<long, double, std::string, function> var;  /* 'stack' values. */
        std::deque<value_ptr> cells;                            /* s-expression values. */

        struct lambda {
            value_ptr formals;                  /* arguments, expr.              */
            value_ptr body;                     /* definition of the function.   */
            std::shared_ptr<environment> env;   /* environment of the lambda.    */
        };
        std::optional<lambda> lambda_exp;       /* Lambda expression. */
    };

    std::ostream& operator<<(std::ostream& os, const value_ptr value);
    std::ostream& operator<<(std::ostream& os, const value::type kind);
}
