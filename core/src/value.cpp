#include <iterator>
#include "value.h"
#include "environment.h"

namespace inky {

    void value::insert(value_ptr v) {
        cells.push_back(v);
    }

    void value::move(value_ptr v) {
        std::move(v->cells.begin(),v->cells.end(),std::back_inserter(cells));
    }

    value_ptr value::clone() {
        value_ptr rtn(new value(kind));             /* copy the kind. */

        /* clone the cells. */
        for (const auto& cell: cells) rtn->cells.push_back(cell->clone());

        /* clone the var... */
        if ( kind != value::type::Function )  { /* if not a lambda, just copy. */
            rtn->var = var;
        } else {
            /* clone the lambda. */
            lambda_ptr lambda = std::get<lambda_ptr>(var);
            lambda_ptr copy(new value::lambda());

            copy->formals = lambda->formals->clone();
            copy->body = lambda->body->clone();
            //copy->env = lambda->env; /* don't copy, refer to same scope. */
            copy->env->set_outer_scope(lambda->env->get_outer_scope());

            rtn->var = copy;
        }

        return rtn;
    }

    /* Utility, useful for debugging etc. */
    std::ostream& to_expression_str(std::ostream& os, const value_ptr& v) {
        if (v->kind == value::type::QExpression) os << "[";
        else os << "(";
        for (const auto& cell: v->cells) {
            if (&cell != &v->cells[0]) os << " ";
            os << cell;
        }
        if (v->kind == value::type::QExpression) os << "]";
        else os << ")";
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
            case value::type::SExpression:
            case value::type::QExpression:
                return to_expression_str(os, value);
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

    std::ostream& operator<<(std::ostream& os, value::lambda_ptr lambda) {
       os << "lambda:\n";
       os << "\tformals:" << lambda->formals << "\n";
       os << "\tbody:" << lambda->body << "\n";
       return os;
    }

};