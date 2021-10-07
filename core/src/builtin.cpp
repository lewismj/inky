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

        either<error,value*> builtin_op(std::shared_ptr<environment> e, value* v, char op) {
        }

        either<error,value*> builtin_add(std::shared_ptr<environment> e, value* v) {
        }

        either<error,value*> builtin_subtract(std::shared_ptr<environment> e, value* v) {
        }

        either<error,value*> builtin_divide(std::shared_ptr<environment> e, value* v) {
        }

        either<error,value*> builtin_multiply(std::shared_ptr<environment> e, value* v) {
        }

    }

    namespace list {

    }


}
