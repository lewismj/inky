#pragma once

#include <string>
#include <variant>
#include <vector>
#include "value_t.h"


namespace inky {

    /* Forward declarations. */
    class value;
    class environment;

    /* A value (Lisp value in this context) is a dynamically-typed hierarchical data structure
     * that represents ast node for s-expression(s).
     */
    class value {
    public:

        enum class type { /* WIP */
            NulOp,
            Integer,
            Double,
            Symbol, /* need to differentiate between symbol and string literal. */
            Function,
            SExpression,
            QExpression /* placeholder for quoted s-expression. */
        };

        /* constructors for primitives */
        explicit value() : kind(type::NulOp) {} /* 'Special value' eval should skip. */
        explicit value(const long l)   : kind(type::Integer) { var = l; }
        explicit value(const double d) : kind(type::Double) { var = d; }
        explicit value(const std::string& s) : kind(type::Symbol) { var = s; }
        explicit value(const function& f): kind(type::Function) { var = f;}

        void set_kind(type t); /* set the type of the value. */
        value* s_expression(); /* create an empty s-expression. */
        value* q_expression(); /* create an empty quoted s-expression. */
        void insert(value* v); /* insert a value into this (cell). */
        void move(value* v); /* move cells of v into this & delete v. */

    private:
        type kind; /* The type of the value, basic type. */

        /* 'stack' values. */
        std::variant<long,double,std::string,function> var;

        std::vector<value*> cells;       /* s-expression values. */
        std::shared_ptr<environment> env; /* lambda environment. */
    };

    static value null_opt;
}