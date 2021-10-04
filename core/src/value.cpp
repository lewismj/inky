#include <iterator>
#include <vector>
#include "value.h"

namespace inky {

    /** Define value ops; builtins etc. */

    void value::set_kind(value::type t) {
       kind = t;
    }

    value* value::s_expression() {
        auto* v = new value();
        v->set_kind(type::SExpression);
        return v;
    }

    value* value::q_expression() {
        auto* v = new value();
        v->set_kind(type::QExpression);
        return v;
    }

    void value::insert(value* v) {
        cells.push_back(v);
    }

    void value::move(value* v) {
        std::move(v->cells.begin(),v->cells.end(),std::back_inserter(cells));
        delete v;
    }


};