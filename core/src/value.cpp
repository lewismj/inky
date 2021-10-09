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

    std::ostream& operator<<(std::ostream& os, value_ptr value) {
        switch (value->kind) {
            case value::type::String:
            case value::type::Symbol:
                os << std::get<std::string>(value->var);
                break;
            case value::type::Integer:
                os << std::get<long>(value->var);
                break;
            case value::type::Double:
                os << std::get<double>(value->var);
                break;
            case value::type::BuiltinFunction:
                os << "<builtin function>";
                break;
            case value::type::Lambda:
            case value::type::Function:
            case value::type::SExpression:
            case value::type::QExpression:
                os << "<unevaluated expression>";
                break;
        }
        os << "\n";
        return os;
    }

};