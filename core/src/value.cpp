#include <iterator>
#include "value.h"


namespace inky {

    void value::insert(value_ptr v) {
        cells.push_back(v);
    }

    void value::move(value_ptr v) {
        std::move(v->cells.begin(),v->cells.end(),std::back_inserter(cells));
    }

    /* Utility, useful for debugging etc. */
    std::ostream& to_expression_str(std::ostream& os, const value_ptr& v) {
        os << "(";
        for (const auto& cell: v->cells) {
            if (&cell != &v->cells[0]) os << " ";
            os << cell;
        }
        os << ")";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const value_ptr value) {
        switch (value->kind) {
            case value::type::String:
                os << "\"" << std::get<std::string>(value->var) << "\"";
                break;
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
                os << "<builtin>";
                break;
            case value::type::Function:
                os << "<function>"; /* to-do, for functions, lambda etc. output formals/body. */
            case value::type::SExpression:
                return to_expression_str(os,value);
            case value::type::QExpression:
                return to_expression_str(os, value);
                break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const value::type kind) {
        switch (kind) {
            case value::type::String:
                os << "<string>";
            case value::type::Integer:
                os << "<integer>";
                break;
            case value::type::Double:
                os << "<double>";
                break;
            case value::type::Symbol:
                os << "<symbol>";
                break;
            case value::type::BuiltinFunction:
                os << "<builtin>";
                break;
            case value::type::Function:
                os << "<function>";
                break;
            case value::type::SExpression:
                os << "<sexpression>";
                break;
            case value::type::QExpression:
                os << "<qexpression>";
                break;
        }

        return os;
    }

};