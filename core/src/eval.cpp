#include "value.h"
#include "environment.h"
#include "eval.h"


namespace inky {

    class eval_impl {
    public:

        explicit eval_impl(environment_ptr e): env(e) {}
        ~eval_impl() = default;

        either<error,value_ptr> call_function(value_ptr fn, value_ptr arg) {
            if ( !fn->is_function() ) return error { "eval error, not a function."};
            if ( fn->kind == value::type::BuiltinFunction ) {
                return error { "not yet implemented."};
            } else {
               return error { " not yet implemented."} ;
            }
        }


        either<error,value_ptr> eval_sexpr(value_ptr v) {
            for (auto & cell : v->cells)  {
                auto maybe = eval(cell);
                if (maybe.is_right()) { /* check cleanup of old cell. */
                    cell = maybe.right_value();
                }
                else {
                    return maybe.left_value();
                }
            }
            return error { "not yet implemented."};
        }


        either<error,value_ptr> eval(value_ptr v) {
            switch (v->kind) {
                case value::type::Symbol:
                    std::cout << "evaluate symbol\n";
                    /* lookup symbol from environment & remove it & return it. */
               case value::type::Integer:
                    std::cout << "evaluate integer\n";
                    /* remove v from environment & return it. */
                case value::type::Double:
                    std::cout << "evaluate double\n";
                    /* remove v from environment & return it. */
                    return v;
                case value::type::SExpression:
                    std::cout << "evaluate s-expression\n";
                    return eval_sexpr(v);
                default:
                    return error {" not yet implemented."};
            };
            return error {"not yet implemented."};
        }

    private:
        environment_ptr env;
    };


    either<error,value_ptr> eval(environment_ptr e, value_ptr v) {
        eval_impl i(e);
        return i.eval(v);
    }

}
