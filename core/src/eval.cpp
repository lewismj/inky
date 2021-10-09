#include <fmt/core.h>

#include "value.h"
#include "environment.h"
#include "eval.h"


namespace inky {

    class eval_impl {
    public:

        explicit eval_impl(environment_ptr e): env(e) {}
        ~eval_impl() = default;


        either<error,value_ptr> eval_sexpression(value_ptr v) {
            if ( v->cells.empty() ) return v;

            /* First evaluate all the sub-expressions. */
            for (auto & cell : v->cells) {
                auto maybe = eval(cell);
                if ( maybe ) cell = maybe.right_value();
                else return maybe.left_value();
            }

            /* First cell should be a function type (builtin or defined). */
            if ( v->cells[0]->is_function() ) {
                if ( v->cells[0]->kind == value::type::BuiltinFunction ) {

                } else {
                    return { "not yet implemented."};
                }

            } else
                return error { fmt::format("eval error, s-expression function type expected, actual: {}",v->cells[0]->kind) };
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

                /* If v is a literal, return it. */
                case value::type::Integer:
                case value::type::Double:
                case value::type::String:
                    return v;

               /* If v is an s-expression, evaluate sub-expressions then the expression itself.*/
                case value::type::SExpression:
                    return eval_sexpression(v);

                default:
                    return error { "eval error, not yet implemented."};
            }
        }

    private:
        environment_ptr env;
    };


    either<error,value_ptr> eval(environment_ptr e, value_ptr v) {
        eval_impl i(e);
        return i.eval(v);
    }

}
