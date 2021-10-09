#include <algorithm>
#include <fmt/core.h>

#include "types.h"
#include "value.h"
#include "builtin.h"


namespace inky::builtin {

    /*
    * Provide basic built-in functions. n.b; the Lisp prelude itself will 'bootstrap'
    * full set of functions.
    */


    /*
     * If we are computing (+,-,/,*) we reduce the cells of 'v' to the accumulated value
     * (i.e. these functions are not for unary '-' etc.
     * Need to check type and perform floating point calc. if needed).
     *
     * No type-classes/semi-group ...
     */

    using integer_op = std::function<long(const long&, const long&)>;
    using double_op  = std::function<double(const double& d, const double&)>;

    struct numeric_operators { integer_op  f; double_op   g; };


    either<error, value_ptr> builtin_op(environment_ptr ignore, value_ptr v, numeric_operators nop) {
        /*
         * 1. check that we have cells to reduce.
         * 2. runtime type check:
         *    Check that all cells are numeric.
         *    If any cell is double, we set the accumulator to double
         *    and cast any long to double as we accumulate.
         *    Otherwise, we treat as integer.
         * 3. On complete, we remove/delete the cells
         *    and return the value (type will be double or integer).
         */

        if (v->cells.empty()) { /* check for unary minus? */
            return error{"runtime error, no cells to reduce"};
        }
        bool is_double = false;
        for (const auto &c : v->cells) {
            if (!c->is_numeric()) return error{"runtime_error, +,-,/,* reduce non numeric."};
            if (c->kind == value::type::Double) is_double = true;
        }

        /* If we have a double then any integer value should be cast to double. */
        std::variant<long, double> accumulator;

        /* make sure variant has correct type. */
        //accumulator = is_double ? static_cast<double>(0.0) : static_cast<long>(0);

        value_ptr cell = v->cells[0];
        if ( is_double ) {
            double cell_val =
                    cell->kind == value::type::Integer ? (double) std::get<long>(cell->var)
                                                       : std::get<double>(cell->var);

            accumulator =  cell_val;
        } else {
            accumulator = std::get<long>(cell->var);
        }

        for (int i = 1; i < v->cells.size(); i++) {
            value_ptr c= v->cells[i];

            try {
                if (is_double) {
                    /* either value_ptr is double or int, cast to double if int; accumulate double. */
                    double cell_val =
                            c->kind == value::type::Integer ? (double) std::get<long>(c->var) : std::get<double>(c->var);
                    accumulator = (double) nop.g(std::get<double>(accumulator), cell_val);
                } else {
                    accumulator = nop.f(std::get<long>(accumulator), std::get<long>(c->var));
                }
            } catch ( const std::runtime_error& e) {
                return error { fmt::format("eval error, caught exception: {}", e.what())};
            }
        }

        /* erase v->cells ??? */
        v->cells.resize(0);

        if (is_double) return value_ptr(new value(std::get<double>(accumulator)));
        return value_ptr(new value(std::get<long>(accumulator)));
    }

    template<typename T> T add(const T& a, const T& b) { return a+b; }
    template<typename T> T subtract(const T& a, const T& b) { return a-b; }
    template<typename T> T multiply(const T& a, const T& b) { return a*b; }
    template<typename T> T divide(const T& a, const T& b) {
        if ( b == 0 ) {
           throw std::runtime_error("eval error, divide by zero.");
        }
        return a/b;
    }

    either<error, value_ptr> builtin_add(environment_ptr e, value_ptr v) {
        return builtin_op(e,v, { add<long>, add<double> });
    }

    either<error, value_ptr> builtin_subtract(environment_ptr e, value_ptr v) {
        return builtin_op(e,v, { subtract<long>, subtract<double> });
    }

    either<error, value_ptr> builtin_divide(environment_ptr e, value_ptr v) {
        return builtin_op(e,v,{ divide<long>, divide<double> });
    }

    either<error, value_ptr> builtin_multiply(environment_ptr e, value_ptr v) {
        return builtin_op(e,v,{multiply<long>,multiply<double>});
    }


    void add_builtin_functions(environment_ptr e) {
        /* basic numerics. */
        e->insert("+",value_ptr(new value(builtin_add, true)));
        e->insert("-",value_ptr(new value(builtin_subtract, true)));
        e->insert("*",value_ptr(new value(builtin_multiply, true)));
        e->insert("/",value_ptr(new value(builtin_divide, true)));

        /* basic list. */

    }

}
