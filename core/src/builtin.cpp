#include <algorithm>
#include <fmt/core.h>

#include "types.h"
#include "value.h"
#include "builtin.h"
#include "eval.h"

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
    using double_op  = std::function<double(const double&, const double&)>;

    struct numeric_operators { integer_op  f; double_op   g; };

    using integer_cmp = std::function<bool(const long&, const long&)>;
    using double_cmp = std::function<bool(const double&, const double&)>;

    struct numeric_cmp { integer_cmp f; double_cmp g; };

    /* here we're adding two s-expressions, no need to lookup anything from outer scope */
    either<error, value_ptr> builtin_op(environment_ptr /* ignore. */, value_ptr v, numeric_operators nop) {
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
                    double cell_val =
                            c->kind == value::type::Integer ? (double) std::get<long>(c->var) : std::get<double>(c->var);
                    accumulator = (double) nop.g(std::get<double>(accumulator), cell_val);
                } else {
                    accumulator = nop.f(std::get<long>(accumulator), std::get<long>(c->var));
                }
            } catch ( const std::runtime_error& e) {
                return error { fmt::format("caught exception: {}", e.what())};
            }
        }

        v->cells.resize(0); /* erase the cells, they've been evaluated. */

        if (is_double) return value_ptr(new value(std::get<double>(accumulator)));
        return value_ptr(new value(std::get<long>(accumulator)));
    }


    /*
     * min/max; we don't use *minelement, or *maxelement from std:: here;
     * (we could ignore non-numeric fields in cells); want to preserve
     * if possible the types and cast where necessary (long to double).
     * Also need the type check; restrictive at the moment to numeric only
     * fields.
     */
    either<error,value_ptr> builtin_cmp(environment_ptr /*ignore */, value_ptr v, numeric_cmp cmp) {
        if (v->cells.empty()) { /* check for unary minus? */
            return error{"runtime error, no cells to reduce"};
        }

        bool is_double = false;
        for (const auto &c : v->cells) {
            if (!c->is_numeric()) return error{"runtime_error, +,-,/,* reduce non numeric."};
            if (c->kind == value::type::Double) is_double = true;
        }

        std::variant<long, double> result;

        value_ptr cell = v->cells[0];
        if ( is_double ) {
            double cell_val =
                    cell->kind == value::type::Integer ? (double) std::get<long>(cell->var)
                                                       : std::get<double>(cell->var);

            result =  cell_val;
        } else {
            result = std::get<long>(cell->var);
        }

        for (int i = 1; i < v->cells.size(); i++) {
            value_ptr c= v->cells[i];

            try {
                if (is_double) {
                    double cell_val =
                            c->kind == value::type::Integer ? (double) std::get<long>(c->var) : std::get<double>(c->var);
                    if (cmp.g(cell_val,std::get<double>(result))) result = cell_val;
                } else {
                    long cell_val = std::get<long>(c->var);
                    if (cmp.f(cell_val,std::get<long>(result))) result = cell_val;
                }
            } catch ( const std::runtime_error& e) {
                return error { fmt::format("caught exception: {}", e.what())};
            }
        }

        v->cells.resize(0); /* erase the cells, they've been evaluated. */

        if (is_double) return value_ptr(new value(std::get<double>(result)));
        return value_ptr(new value(std::get<long>(result)));
    }


    /* The purpose of the builtin is to provide very basic functions, the 'prelude' should
     * be used to boostrap and provide a 'standard library'. A standard lib. should not be
     * implemented as builtin functions.
     */


    template<typename T> T add(const T& a, const T& b) { return a+b; }
    template<typename T> T subtract(const T& a, const T& b) { return a-b; }
    template<typename T> T multiply(const T& a, const T& b) { return a*b; }
    template<typename T> T divide(const T& a, const T& b) {
        if ( b == 0 ) {
            throw std::runtime_error("divide by zero.");
        }
        return a/b;
    }
    template<typename T> bool lt(const T& a, const T& b) { return a<b; }
    template<typename T> bool gt(const T& a, const T& b) { return a>b; }


    /* functions for dealing with lambda expressions/lists;
     * list, head, tail, eval, join. */


    /* n.b. in these operators we return errors on head or tail of empty lists etc,
     * they're used for argument calling; list types can be fully implemented in a prelude,
     * which may offer different semantics. */


    either<error,value_ptr> builtin_list(environment_ptr , value_ptr a) {
        a->kind = value::type::QExpression;
        return a;
    }

    either<error,value_ptr> builtin_head(environment_ptr , value_ptr a) {
        if (a->cells.size() != 1) {
            std::string str = fmt::format("head operator expects 1 argument, actual:{}",a->cells.size());
            return error { str };
        } else if ( a->cells[0]->cells.empty() ) {
            return error { "head operator received empty list." };
        }
        value_ptr v = a->cells[0]->cells[0];
        a->cells.erase(a->cells.begin(),a->cells.end());
        return v;
    }

    either<error,value_ptr> builtin_tail(environment_ptr , value_ptr a) {
        if (a->cells.size() != 1) {
            std::string str = fmt::format("tail operator expects 1 argument, actual:{}",a->cells.size());
            return error { str };
        } else if ( a->cells[0]->cells.empty() ) {
            return error { "tail operator received empty list." };
        }

        value_ptr v = a->cells[0];
        a->cells.erase(a->cells.begin(),a->cells.end());
        v->cells.pop_front();
        return v;
    }

    either<error,value_ptr> builtin_join(environment_ptr , value_ptr a) {
        for (const auto& cell: a->cells) {
            if (a->kind != value::type::QExpression ) return error { "join operator on non quoted expression. "};
        }
        value_ptr v = a->cells[0];
        a->cells.pop_front();
        v->move(a);
        return v;
    }

    either<error,value_ptr> builtin_eval(environment_ptr e, value_ptr a) {
        if (a->cells.size() != 1) {
            std::string str = fmt::format("eval operator expects 1 argument, actual:{}",a->cells.size());
            return error { str };
        } else if ( a->cells[0]->kind != value::type::QExpression ) {
            return error {"argument not quoted expression." };
        }

        a->cells[0]->kind = value::type::SExpression;
        return inky::eval(e,a);
    }


    either<error,value_ptr> builtin_define(environment_ptr e, value_ptr a, bool outer_scope) {
        if (a->cells.empty()) {
           return error { "'defn' function missing arguments."} ;
        }  else if ( a->cells[0]->kind != value::type::QExpression) {
               return error{"'defn' expected [] argument list."};
        }
        value_ptr symbols = a->cells[0];
        if ( symbols->cells.size() != a->cells.size() -1 ) {
            std::string message
                    = fmt::format("Unexpected number of arguments for symbols, received {}, expected {}. ",
                                  symbols->cells.size(), a->cells.size() - 1);
            return error { message };
        }

        for (size_t i=0; i<symbols->cells.size(); i++) {
            auto key = symbols->cells[i];
            if ( key->kind != value::type::Symbol ) {
                return error { "'defn' argument list should contain symbols."};
            }
            auto val = a->cells[i+1];
            if (outer_scope) e->insert_global(std::get<std::string>(key->var),val);
            else e->insert(std::get<std::string>(key->var),val) ;
        }

        /* value defined, return empty s-expression. */
        return value_ptr(new value(value::type::SExpression));
    }

    either<error,value_ptr> builtin_defn(environment_ptr e, value_ptr a) {
        return builtin_define(e,a, true);
    }

    either<error,value_ptr> builtin_put(environment_ptr e, value_ptr a) {
        return builtin_define(e,a,false);
    }

    either<error,value_ptr> builtin_lambda(environment_ptr e, value_ptr a) {
        if ( a->cells.size() != 2) {
            std::string message = fmt::format("lambda expects 2 arguments formals & body, received: {}",a->cells.size());
            return error { message };
        }

        for (const auto& c: a->cells[0]->cells) {
            if ( c->kind != value::type::Symbol ) {
                return error { "lambda arguments must be symbols."};
            }
        }

        environment_ptr env(new environment());

        value_ptr formals = a->cells.front(); a->cells.pop_front();
        value_ptr body = a->cells.front(); a->cells.pop_front();

        value::lambda_ptr l (new value::lambda { formals, body, env} );

        return value_ptr( new value (l));
    }

    void add_builtin_functions(environment_ptr e) {
        /* basic numerics. */
        auto builtin_add = [](environment_ptr e, value_ptr v){ return builtin_op(e,v, { add<long>, add<double> }); };
        auto builtin_subtract = [](environment_ptr e,value_ptr v){ return builtin_op(e,v, { subtract<long>, subtract<double> }); };
        auto builtin_divide = [](environment_ptr e,value_ptr v){ return builtin_op(e,v, { divide<long>, divide<double> }); };
        auto builtin_multiply = [](environment_ptr e, value_ptr v){ return builtin_op(e,v, { multiply<long>, multiply<double> }); };

        e->insert("+", value_ptr(new value(builtin_add)));
        e->insert("-",value_ptr(new value(builtin_subtract)));
        e->insert("*",value_ptr(new value(builtin_multiply)));
        e->insert("/",value_ptr(new value(builtin_divide)));

        /* min/max; for numerical values. */
        auto builtin_min = [](environment_ptr e, value_ptr v){ return builtin_cmp(e,v, { lt<long>, lt<double> }); };
        auto builtin_max = [](environment_ptr e, value_ptr v){ return builtin_cmp(e,v, { gt<long>, gt<double> }); };
        e->insert("min",value_ptr(new value(builtin_min)));
        e->insert("max",value_ptr(new value(builtin_max)));

        /* q-expression, lambda primitives. */
        e->insert("list",value_ptr(new value(builtin_list)));
        e->insert("head",value_ptr(new value(builtin_head)));
        e->insert("tail",value_ptr(new value(builtin_tail)));
        e->insert("eval",value_ptr(new value(builtin_eval)));
        e->insert("join",value_ptr(new value(builtin_join)));

        /* defn , =  ; the former puts definition into global context, the later the local environment. */
        e->insert("defn",value_ptr(new value(builtin_defn)));
        e->insert("=",value_ptr(new value(builtin_put)));

        /* lambda function. */
        e->insert("lambda",value_ptr(new value(builtin_lambda)));
        e->insert("\\",value_ptr(new value(builtin_lambda)));
    }

}
