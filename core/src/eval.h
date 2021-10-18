#pragma once

#include "either.h"
#include "value.h"

namespace Inky::Lisp {

    Either<Error,ValuePtr> eval(EnvironmentPtr env, ValuePtr val);

}