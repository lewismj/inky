#pragma once

#include "either.h"
#include "value.h"

namespace Inky::Lisp {

    ValuePtr eval(EnvironmentPtr env, ValuePtr val);

}