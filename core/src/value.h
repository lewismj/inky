#pragma once

#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <variant>

#include "either.h"


namespace Inky::Lisp {

    /* Forward declarations. */
    struct Environment;
    struct Value;

    struct Error {
        std::string message;
        struct Location {
            size_t begin;
            size_t length;
        };
        Location location;
    };

    /* Type definitions . */
    typedef std::shared_ptr<Environment> EnvironmentPtr;
    typedef std::shared_ptr<Value> ValuePtr;

    /* Builtin function type. */
    typedef std::function<Either<Error,ValuePtr>(EnvironmentPtr,ValuePtr)> BuiltinFunction;

    struct Expression {
        /* Insert a value into this expression. */
        void insert(ValuePtr value);

        /*
         * Move the cell of the value (if it is an expression/list) into this one,
         * otherwise do nothing.
         */
        void move(ValuePtr value);

        std::deque<ValuePtr> cells; /* An S-Expression is a list of cells, that contain values. */
    };
    typedef std::shared_ptr<Expression> ExpressionPtr;

    /* Lambda function. */
    struct Lambda {
        ValuePtr        formals;    /* arguments of an expression.          */
        ValuePtr        body;       /* definition of the function itself.   */
        EnvironmentPtr  env;      /* environment of the lambda.           */
    };
    typedef std::shared_ptr<Lambda> LambdaPtr;


    enum class Type { /* Used for type checking value of Lisp element. */
        Integer,
        Double,
        String,
        Symbol,
        BuiltinFunction,
        Function,
        SExpression,
        QExpression
    };

    /*
     * Value is a cyclical type, it is a variant of the types that a LISP expression can hold, or
     * an expression itself. Alternative is class hierarchy etc. Essentially this represents a
     * Union type.
     */
    struct Value {
        /*
         * Make a deep copy of the Value any Environment references, ony the
         * global scope is copied.
         */
        ValuePtr clone();

        bool isNumeric() const { return kind == Type::Integer || kind == Type::Double; }
        bool isExpression() const  { return kind == Type::SExpression || kind == Type::QExpression; }

        Type kind; /* Convenient flag for type checking. */

        /* The variant that the value can hold. */
        std::variant<long,double,std::string,BuiltinFunction,LambdaPtr,ExpressionPtr> var;
    };

    namespace Ops { /* Define utilities for constructing Values. */

        ValuePtr makeInteger(const long& l);
        ValuePtr makeDouble(const double& d);
        ValuePtr makeString(const std::string& s);
        ValuePtr makeSymbol(const std::string& s);
        ValuePtr makeBuiltin(const BuiltinFunction& f);
        ValuePtr makeFunction(LambdaPtr lambda);
        ValuePtr makeSExpression(ExpressionPtr expression);
        ValuePtr makeSExpression();
        ValuePtr makeQExpression(ExpressionPtr expression);
        ValuePtr makeQExpression();
    }

    /* Output operators. */
    std::ostream& operator<<(std::ostream& os, ValuePtr value);
    std::ostream& operator<<(std::ostream& os, Type kind);
    std::ostream& operator<<(std::ostream& os, LambdaPtr lambda);
    std::ostream& operator<<(std::ostream& os, ExpressionPtr expression);
    std::ostream& operator<<(std::ostream& os, Error error);
}