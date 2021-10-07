#include "builtin.h"

#include <utility>
#include "types.h"
#include "value.h"

namespace inky::builtin {

    /*
     * Provide basic built-in functions. n.b; the Lisp prelude itself will 'bootstrap'
     * full set of functions.
     */

    namespace numeric {

        template<char op> either<error,value*> builtin_op(std::shared_ptr<environment> e, value* v) {


        }


        either<error,value*> builtin_add(std::shared_ptr<environment> e, value* v) {
            return builtin_op<'+'>(std::move(e),v);
        }

        either<error,value*> builtin_subtract(std::shared_ptr<environment> e, value* v) {
                if ( v->cells.size() == 0) {


                } else {
                    return builtin_op<'-'>(std::move(e), v);
                }
        }

        either<error,value*> builtin_divide(std::shared_ptr<environment> e, value* v) {
            return builtin_op<'/'>(std::move(e),v);
        }

        either<error,value*> builtin_multiply(std::shared_ptr<environment> e, value* v) {
            return builtin_op<'*'>(std::move(e),v);
        }


    }

    namespace list {

    }


}
