#include "value.h"
#include "environment.h"
#include "eval.h"


namespace inky {

    class eval_impl {
    public:

        explicit eval_impl(std::shared_ptr<environment> e): env(e) {}
        ~eval_impl() = default;

        either<error,value*> call_function(value* fn, value* arg) {
            if ( !fn->is_function() ) return error { "eval error, not a function."};
            if ( fn->kind == value::type::BuiltinFunction ) {

            } else {

            }
        }


        either<error,value*> eval(value* v) {
            return error { "not yet implemented."};
        }

    private:
        std::shared_ptr<environment> env;
    };


    either<error,value*> eval(std::shared_ptr<environment> e, value* v) {
        eval_impl i(e);
        return i.eval(v);
    }

}
