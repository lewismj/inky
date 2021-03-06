#include <iterator>

#include "environment.h"
#include "value.h"


namespace Inky::Lisp {


    void Expression::insert(ValuePtr value) {
        cells.push_back(value);
    }

    ValuePtr Value::clone() {
        switch (kind) {
            case Type::Function: {
                LambdaPtr copy(new Lambda());
                LambdaPtr lambda = std::get<LambdaPtr>(var);

                copy->formals = lambda->formals->clone();
                copy->body = lambda->body->clone();
                copy->env = lambda->env->clone();

                return Ops::makeFunction(copy);
            }

            case Type::SExpression:
            case Type::QExpression: {
                ExpressionPtr copy = std::make_shared<Expression>(Expression());
                ExpressionPtr expression = std::get<ExpressionPtr>(var);
                for (const auto& cell: expression->cells) copy->cells.push_back(cell->clone());

                ValuePtr v = kind == Type::QExpression ?
                            Ops::makeQExpression(copy) : Ops::makeSExpression(copy);
                return v;
            }

            case Type::Error:
            case Type::Integer:
            case Type::Double:
            case Type::String:
            case Type::Symbol:
            case Type::BuiltinFunction:
                return std::make_shared<Value>(Value{kind, var});
        }
    }

    std::ostream& operator<<(std::ostream& os, ValuePtr value) {
        switch (value->kind) {
            case Type::Integer:
                os << std::get<long>(value->var);
                break;
            case Type::Double:
                os << std::get<double>(value->var);
                break;
            case Type::String:
                os << '\"' << std::get<std::string>(value->var) << '\"';
                break;
            case Type::Symbol:
                os << std::get<std::string>(value->var);
                break;
            case Type::BuiltinFunction:
                os << "<builtin>";
                break;
            case Type::Function:
                os << std::get<LambdaPtr>(value->var);
                break;
            case Type::QExpression:
                os << "[" << std::get<ExpressionPtr>(value->var) << "]";
                break;
            case Type::SExpression:
                os << "(" << std::get<ExpressionPtr>(value->var) << ")";
                break;
            case Type::Error:
                os << std::get<LispErrorPtr>(value->var) << "\n";
                break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, Type kind) {
        switch (kind) {
            case Type::Integer:
                os << "<integer>";
                break;
            case Type::Double:
                os << "<double>";
                break;
            case Type::String:
                os << "<string>";
                break;
            case Type::Symbol:
                os << "<symbol>";
                break;
            case Type::QExpression:
                os << "<QExpression>";
                break;
            case Type::BuiltinFunction:
                os << "<builtin>";
                break;
            case Type::Function:
                os << "<function>";
                break;
            case Type::SExpression:
                os << "<sexpression>";
                break;
            case Type::Error:
                os << "<error>";
                break;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, LambdaPtr l) {
        os << "lambda:\n";
        os << "\tformals:" << l->formals << "\n";
        os << "\tbody:" << l->body << "\n";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, ExpressionPtr e) {
        for (const auto& cell: e->cells) {
            if (& cell != &e->cells[0]) os << " ";
            os << cell;
        }
        return os;
    }

    /* Useful for debug, but use fmt in REPL. */
    std::ostream& operator<<(std::ostream& os, ParseError e) {
        os << e.message << "\n";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, LispErrorPtr e) {
        os << e->message << "\n";
        return os;
    }

    namespace Ops {

        ValuePtr makeInteger(const long& l) {
            return std::make_shared<Value>(Value { Type::Integer, l});
        }

        ValuePtr makeDouble(const double& d) {
            return std::make_shared<Value>(Value {Type::Double, d}) ;
        }

        ValuePtr makeString(const std::string& s) {
            return std::make_shared<Value>(Value { Type::String, s});
        }

        ValuePtr makeSymbol(const std::string& s) {
            return std::make_shared<Value>(Value { Type::Symbol, s});
        }

        ValuePtr makeBuiltin(const BuiltinFunction& f) {
            return std::make_shared<Value>(Value {Type::BuiltinFunction, f});
        }

        ValuePtr makeFunction(LambdaPtr lambda) {
            return std::make_shared<Value>(Value {Type::Function, lambda});
        }

        ValuePtr makeSExpression(ExpressionPtr expression) {
            return std::make_shared<Value>(Value { Type::SExpression, expression});
        }

        ValuePtr makeSExpression() {
            ExpressionPtr expression = std::make_shared<Expression>(Expression());
            return std::make_shared<Value>(Value { Type::SExpression, expression});
        }

        ValuePtr makeQExpression(ExpressionPtr expression) {
            return std::make_shared<Value>(Value { Type::QExpression, expression});
        }

        ValuePtr makeQExpression() {
            ExpressionPtr expression = std::make_shared<Expression>(Expression());
            return std::make_shared<Value>(Value { Type::QExpression, expression});
        }

        ValuePtr makeError(const std::string& error)  {
            LispErrorPtr lispError = std::make_shared<LispError>(LispError());
            lispError->message = error;
            return std::make_shared<Value>(Value { Type::Error, lispError });
        }

        bool isError(ValuePtr a) { return a->kind == Type::Error; }

        bool isNumeric(ValuePtr a) { return a->kind == Type::Integer || a->kind == Type::Double; }

        bool isExpression(ValuePtr a) { return a->kind == Type::SExpression || a->kind == Type::QExpression; }

        bool isEmptyExpression(ValuePtr a) {
            if (a->kind == Type::SExpression || a->kind == Type::QExpression) {
               ExpressionPtr e = std::get<ExpressionPtr>(a->var);
               return e->cells.empty();
            }
            return false;
        }

        bool hasSymbolName(ValuePtr a, std::string_view s) {
            if ( a->kind != Type::Symbol ) return false;
            else return std::get<std::string>(a->var) == s;
        }

    }
}