#include "eval.h"

namespace inky::eval {

    either<error,value*> eval(std::shared_ptr<environment> e, value* v) {
        return error {"not yet implemented."};
    }

}
