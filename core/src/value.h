#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <deque>
#include "types.h"


namespace inky {

    /* Forward declarations. */
    class environment;

    /* A value (Lisp value in this context) is a dynamically-typed hierarchical data structure
     * that represents ast node for s-expression(s). */
    struct value {
        struct lambda;

        /* primitives, n.b. prelude should bootstrap proper types;  */
        enum class type {
            Integer,            /* Integer primitive type.                              */
            Double,             /* Double primitive type.                               */
            String,             /* String primitive type.                               */
            Symbol,             /* Symbol, e.g. variable or function name, etc.         */
            BuiltinFunction,    /* Builtin function.                                    */
            Function,           /* Lambda function, can be named using builtin 'defn'   */
            SExpression,        /* SExpression.                                         */
            QExpression         /* Quoted SExpression.                                  */
        };

        explicit value(const long l) : kind(type::Integer) { var = l; }
        explicit value(const double d) : kind(type::Double) { var = d; }
        explicit value(const std::string &s) : kind(type::Symbol) { var = s; }
        explicit value(const std::string &s, bool literal) { kind = literal ? type::String : type::Symbol; var = s; }
        explicit value(const builtin_function &f) : kind(type::BuiltinFunction) { var = f; }
        explicit value(std::shared_ptr<lambda>& l): kind(type::Function) { var = l;}
        explicit value(type t) { kind = t; }

        ~value() = default;

        void insert(value_ptr v);               /* insert a value into this (cell). */
        void move(value_ptr v);                 /* move cells of v into this & delete v. */

        value_ptr clone();                      /* deep copy, used for functions. */

        [[nodiscard]] bool is_numeric() const { return kind == type::Double || kind == type::Integer; }
        [[nodiscard]] bool is_function() const  { return kind == type::Function || kind == type::BuiltinFunction; }

        type kind; /* The type of the value, basic type. */

        struct lambda { /* structure for defining functions. */
           std::shared_ptr<value> formals;     /* arguments, expr.              */
           std::shared_ptr<value> body;        /* definition of the function.   */
           std::shared_ptr<environment> env;   /* environment of the lambda.    */
        };
        typedef std::shared_ptr<lambda> lambda_ptr;

        /* s or q expression cells;
         * todo: possibly this could be within the variant.
         *       we would want a cell_ptr (i.e. shared_ptr),
         *       having it here (on each value which may be empty)
         *       is convenient (no, std::get<> or checking kind type).
         */
        std::deque<value_ptr> cells;
        std::variant<long, double, std::string, builtin_function, lambda_ptr > var;
    };

    std::ostream& operator<<(std::ostream& os, value_ptr value);
    std::ostream& operator<<(std::ostream& os, value::type kind);
    std::ostream& operator<<(std::ostream& os, value::lambda_ptr);
}
