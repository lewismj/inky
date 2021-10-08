#include <algorithm>

#include "types.h"
#include "value.h"
#include "builtin.h"


namespace inky::builtin {

    /*
    * Provide basic built-in functions. n.b; the Lisp prelude itself will 'bootstrap'
    * full set of functions.
    */

    enum class opcode { /* switch on the op-code rather than have lots of templates for long, double etc... */
        Add,
        Subtract,
        Multiply,
        Divide
    };
       // TODO -- error reporting.


    namespace core {
        // TODO - support 'eval' for quoted expressions.
    }

    namespace list {

    }

    namespace numeric {

        /*
         * If we are computing (+,-,/,*) we reduce the cells of 'v' to the accumulated value
         * (i.e. these functions are not for unary '-' etc.
         * Need to check type and perform floating point calc. if needed.
         */
        either<error, value_ptr> builtin_op(environment_ptr e, value_ptr v, opcode op) {

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
                if (is_double) {
                    /* either value_ptr is double or int, cast to double if int; accumulate double. */
                    double cell_val =
                            cell->kind == value::type::Integer ? (double) std::get<long>(cell->var)
                                                               : std::get<double>(cell->var);
                    switch (op) {
                        case opcode::Add:
                            accumulator = std::get<double>(accumulator) + cell_val;
                            break;
                        case opcode::Subtract:
                            accumulator = std::get<double>(accumulator) - cell_val;
                            break;
                        case opcode::Multiply:
                            accumulator = std::get<double>(accumulator) * cell_val;
                            break;
                        case opcode::Divide:
                            if (cell_val == 0) return error{"runtime error, divide by zero."};
                            accumulator = std::get<double>(accumulator) / cell_val;
                            break;
                    }
                } else {
                    switch (op) {
                        case opcode::Add:
                            accumulator = std::get<long>(accumulator) + std::get<long>(cell->var);
                            break;
                        case opcode::Subtract:
                            accumulator = std::get<long>(accumulator) - std::get<long>(cell->var);
                            break;
                        case opcode::Multiply:
                            accumulator = std::get<long>(accumulator) * std::get<long>(cell->var);
                            break;
                        case opcode::Divide:
                            if (std::get<long>(cell->var) == 0) return error{"runtime error, divide by zero."};
                            accumulator = std::get<long>(accumulator) / std::get<long>(cell->var);
                            break;
                    }
                }
            }

            /* erase v->cells ??? */

            if (is_double) return value_ptr( new value (std::get<double>(accumulator)));
            return value_ptr( new value(std::get<long>(accumulator)));
        }


        either<error, value_ptr> builtin_add(environment_ptr e, value_ptr v) {
            return builtin_op(e, v, opcode::Add);
        }

        either<error, value_ptr> builtin_subtract(environment_ptr e, value_ptr v) {
            return builtin_op(e, v, opcode::Subtract);
        }

        either<error, value_ptr> builtin_divide(environment_ptr e, value_ptr v) {
            return builtin_op(e, v, opcode::Divide);
        }

        either<error, value_ptr> builtin_multiply(environment_ptr e, value_ptr v) {
            return builtin_op(e, v, opcode::Multiply);
        }

    }

}
