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

        enum class Type { /* WIP */
            NulOp,
            Error,
            Integer,
            Double,
            Symbol, /* need to differentiate between symbol and string literal. */
            Function,
            SExpression,
            QExpression /* placeholder for quoted s-expression. */
        };

        explicit value() : kind(Type::NulOp) {} /* Perform no operation. */
        explicit value(const error& e) : kind(Type::Error) { var = e; }
        explicit value(const long l)   : kind(Type::Integer) { var = l; }
        explicit value(const double d) : kind(Type::Double) { var = d; }
        explicit value(const std::string& s) : kind(Type::Symbol) { var = s; }
        explicit value(const function& f): kind(Type::Function) { var = f;}

    private:
        Type kind; /* The type of the value, basic type. */

        /* WIP ... */
        std::variant<error,long,double,std::string,function,std::vector<value*> > var;

        /* Only need fix point combinator, if vector<T> T;
        template<typename T> using var_t = std::variant< error,
                long,
                double,
                std::string,
                function,
                std::vector<T>>;

        template <template<class> class K> struct Fix : K<Fix<K>> { using K<Fix>::K; };
        using fixed_variant = Fix<var_t>;

        fixed_variant var; */
    };

    /* static to represent null_opt, do nothing. */
    static value null_opt;

}