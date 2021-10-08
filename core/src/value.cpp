#include <algorithm>
#include <iterator>
#include "value.h"


namespace inky {

    void value::insert(value_ptr v) {
        cells.push_back(v);
    }

    void value::move(value_ptr v) {
        std::move(v->cells.begin(),v->cells.end(),std::back_inserter(cells));
    }

};