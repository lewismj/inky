#include <fmt/core.h>

#include "value.h"
#include "environment.h"
#include "eval.h"
#include "builtin.h"

namespace inky {

    class eval_impl {
    public:

        explicit eval_impl(environment_ptr e): env(e) {}
        ~eval_impl() = default;

        either<error,value_ptr> eval_builtin_fn(value_ptr f, value_ptr a) {
                auto fn = std::get<builtin_function>(f->var);
                return fn(env,a);
        }

        either<error,value_ptr> eval_lambda_fn(value_ptr f, value_ptr a) {
            /* f contains:
             * the struct 'lambda':
             *  formals (Argument specification).
             *  body (Body of the function intself).
             *  evaluation environment.
             *
             *  a sexpression:
             *   contains the arguments to pass to the function.
             */
            value::lambda_ptr fn = std::get<value::lambda_ptr>(f->var);

            /* copy arguments to formals;
             * i)   if too many arguments - return an error.
             * ii)  too few, then partially apply.
             * iii) allow variable number of args.
             */

            size_t arg_count = a->cells.size();
            size_t formals_count = fn->formals->cells.size();

            while ( ! a->cells.empty() ) { /* First step is to bind the symbols (variables) with supplied args. */
                if (fn->formals->cells.empty()) {
                    std::string str = fmt::format("Function passed too many arguments {} , expected {}", arg_count,
                                                  formals_count);
                    return error{str};
                }

                value_ptr symbol = fn->formals->cells[0]; fn->formals->cells.pop_front();
                if ( symbol->kind != value::type::Symbol ) return error {"Function eval failed formal not a symbol."};
                std::string symbol_name = std::get<std::string>(symbol->var);

                if ( symbol_name == "&") { /* var args.... */
                    return error { "varargs not yet implement."};
                }

                value_ptr argument = a->cells[0]; a->cells.pop_front();
                fn->env->insert(symbol_name,argument);
            }

            if (fn->formals->cells.empty()) {
                fn->env->set_outer_scope(env);

                value_ptr expr(new value(value::type::SExpression));

                value_ptr body = fn->body->clone();
                body->kind = value::type::SExpression;

                return inky::eval(fn->env, body);
            }

            return f; // should copy?
        }

        either<error,value_ptr> eval_sexpression(value_ptr v) {
            if ( v->cells.empty() ) return v;
            if ( v->cells.size() == 1) return eval(v->cells[0]);

            /* First evaluate all the sub-expressions. */
            for (auto & cell : v->cells) {
                auto maybe = eval(cell);
                if ( maybe ) cell = maybe.right_value();
                else return maybe.left_value();
            }

            /* First cell should be a function type (builtin or defined). */
            if ( v->cells[0]->kind == value::type::BuiltinFunction ) {
                value_ptr fn = v->cells[0];
                v->cells.pop_front();
                return eval_builtin_fn(fn,v);
            }
            else if ( v->cells[0]->kind == value::type::Function )  {
                value_ptr lambda = v->cells[0];
                v->cells.pop_front();
                return eval_lambda_fn(lambda,v);
            }

            return error { fmt::format("S-expression function type expected, actual: {}",v->cells[0]->kind) };
        }

        either<error,value_ptr> eval(value_ptr v) {
            switch (v->kind) {
                /* If v is a symbol, lookup value of v in the environment and return it. */
                case value::type::Symbol: {
                    auto key = std::get<std::string>(v->var);
                    auto lookup = env->lookup(key);
                    if (lookup) return lookup;
                    else return error{fmt::format("Unbound symbol {}", key)};
                }

                /* If v is an s-expression, evaluate sub-expressions then the expression itself.*/
                case value::type::SExpression:
                    return eval_sexpression(v);

                /* Otherwise, return value. */
                case value::type::Integer:
                case value::type::Double:
                case value::type::String:
                case value::type::QExpression:
                case value::type::BuiltinFunction:
                case value::type::Function:
                    return v;
            }
        }

    private:
        environment_ptr env;
    };

    either<error,value_ptr> eval(environment_ptr e, value_ptr v) {
        eval_impl i(e);
        auto result = i.eval(v);
        return result;
    }

    either<error,value_ptr> eval(environment_ptr e, either<error, value_ptr> v) {
        if ( v.is_left()) return v.left_value();
        else return eval(e,v.right_value());
    }

}
