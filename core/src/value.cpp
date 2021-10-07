#include <algorithm>
#include <iterator>
#include "value.h"


namespace inky {

    value::~value() {
        for (const auto* c : cells) delete c;
    }

    void value::insert(value* v) {
        cells.push_back(v);
    }

    void value::move(value* v) {
        std::move(v->cells.begin(),v->cells.end(),std::back_inserter(cells));
        delete v;
    }




};