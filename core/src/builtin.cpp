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
     * Need to check type and perform floating point calc. if needed.
     */
    using integer_op = std::function<long(const long&, const long&)>;
    using double_op  = std::function<long(const double& d, const double&)>;
    struct numeric_operators {
        integer_op  f;
        double_op   g;
    };


    either<error, value_ptr> builtin_op(environment_ptr e, value_ptr v, numeric_operators nop) {
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
        accumulator = is_double ? 0.0 : 0l;

        for (int i = 1; i < v->cells.size(); i++) {
            value_ptr cell = v->cells[i];

            try {
                if (is_double) {
                    /* either value_ptr is double or int, cast to double if int; accumulate double. */
                    double cell_val =
                            cell->kind == value::type::Integer ? (double) std::get<long>(cell->var)
                                                               : std::get<double>(cell->var);
                    accumulator = nop.g(std::get<double>(accumulator), cell_val);

                } else {
                    accumulator = nop.f(std::get<long>(accumulator), std::get<long>(cell->var));
                }
            } catch ( const std::exception& e) {
                return error { fmt::format("eval error, caught exception: {}", e.what())};
            }

        }

        /* erase v->cells ??? */
        v->cells.resize(0);

        if (is_double) return value_ptr(new value(std::get<double>(accumulator)));
        return value_ptr(new value(std::get<long>(accumulator)));
    }


    either<error, value_ptr> builtin_add(environment_ptr e, value_ptr v) {
        numeric_operators ops {
                [](const long &a, const long &b) { return a + b; },
                [](const double& a, const double& b) { return a + b; }
        };
        return builtin_op(e,v,ops);
    }

    either<error, value_ptr> builtin_subtract(environment_ptr e, value_ptr v) {
        numeric_operators ops {
                [](const long &a, const long &b) { return a - b; },
                [](const double& a, const double& b) { return a - b; }
        };
        return builtin_op(e,v, ops);
    }

    either<error, value_ptr> builtin_divide(environment_ptr e, value_ptr v) {
        numeric_operators ops {
                [](const long &a, const long &b) { return a / b; },
                [](const double& a, const double& b) { return a / b; }
        };
        return builtin_op(e,v, ops);
    }

    either<error, value_ptr> builtin_multiply(environment_ptr e, value_ptr v) {
        numeric_operators ops {
                [](const long &a, const long &b) { return a * b; },
                [](const double& a, const double& b) { return a * b; }
        };
        return builtin_op(e,v, ops);
    }


    void add_builtin_functions(environment_ptr e) {
        /* basic numerics. */
        e->insert("+",value_ptr(new value(builtin_add)));
        e->insert("-", value_ptr(new value(builtin_subtract)));
        e->insert("*", value_ptr(new value(builtin_multiply)));
        e->insert("/",value_ptr(new value(builtin_divide)));

        /* basic list. */

    }

}
