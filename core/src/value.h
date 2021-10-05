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

        enum class type { /* primitives, n.b. prelude itself will bootstrap other types. */
            Integer,
            Double,
            String,
            Symbol,
            Function,
            SExpression,
            QExpression /* placeholder for quoted s-expression. */
        };

        explicit value(const long l)   : kind(type::Integer) { var = l; }
        explicit value(const double d) : kind(type::Double) { var = d; }
        explicit value(const std::string& s) : kind(type::Symbol) { var = s; }
        explicit value(const std::string& s, bool literal) {
            kind = literal ? type::String : type::Symbol;
            var = s;
        }
        explicit value(const function& f): kind(type::Function) { var = f;}
        explicit value(type t) { kind = t;}

        /* destructor. */
        ~value();

        void insert(value* v); /* insert a value into this (cell). */
        void move(value* v); /* move cells of v into this & delete v. */

    private:


        type kind; /* The type of the value, basic type. */

        /* 'stack' values. */
        std::variant<long,double,std::string,function> var;

        std::vector<value*> cells;       /* s-expression values. */
        std::shared_ptr<environment> env; /* lambda environment. */
    };

}