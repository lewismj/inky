#include <algorithm>
#include <initializer_list>
#include <sstream>
#include <variant>
#include <fmt/core.h>

#include "eval.h"
#include "value.h"
#include "builtin.h"


namespace Inky::Lisp {


    Either<Error,ValuePtr> builtin_lambda(EnvironmentPtr env, ValuePtr p) {
        if ( !p->isExpression() ) return Error { "lambda fn, formals & body must be [] expression."};
        ExpressionPtr expression = std::get<ExpressionPtr>(p->var);

        /*
         * 'p' the result of the parse "(x) (+ 1 x)", lambda is the symbol matched, so p should contain
         * the formals and the body of the function.
         */
        if ( expression->cells.size() != 2 ) {
            std::string message = fmt::format("lambda expects 2 arguments formals & body, received: {}",expression->cells.size());
            return Error {message };
        }

        /* Formals must be expression containing symbols. */
        ValuePtr formals = expression->cells[0]; expression->cells.pop_front();
        ValuePtr body = expression->cells[0];  expression->cells.pop_front();

        if ( !formals->isExpression() ) return Error { "formals must be an expression containing symbols for arguments."};
        ExpressionPtr formalExp = std::get<ExpressionPtr>(formals->var);
        /* Do type check that they are all symbols. */
        for (const auto& c: formalExp->cells) {
            if (c->kind != Type::Symbol) return Error {"lambda function formal must be symbol."};
        }

        EnvironmentPtr e(new Environment()); /* Eval should set the outer scope? */

        return Ops::makeFunction(std::make_shared<Lambda>(Lambda{ formals, body, e }));
    }



    Either<Error,ValuePtr> builtin_define(EnvironmentPtr e, ValuePtr a, bool insertIntoOuterScope) {
        if ( !a->isExpression() ) return Error { "define parameters must be [] expressions." };
        ExpressionPtr expression = std::get<ExpressionPtr>(a->var);

        if (expression->cells.empty()) return Error { "define missing arguments."};
        ValuePtr first = expression->cells[0];
        if ( ! first->isExpression()) return Error {"define argument must be an expression."};
        ExpressionPtr symbols = std::get<ExpressionPtr>(first->var) ;

        if ( symbols->cells.size() != expression->cells.size() -1 ) {
                std::string message = fmt::format("unexpected number of arguments for symbols, received {}, expected {}. ",
                                                  symbols->cells.size(), expression->cells.size() - 1);
                return Error { message };
        };

        for (size_t i=0; i<symbols->cells.size(); i++) {
            auto key = symbols->cells[i];
            if ( key->kind != Type::Symbol ) {
                auto tmp = eval(e,key);
                if (tmp.isLeft()) return tmp.left();
            }
            auto val = expression->cells[i+1] ;


            if (insertIntoOuterScope) e->insertGlobal(std::get<std::string>(key->var),val);
            else e->insert(std::get<std::string>(key->var),val) ;
        }

        /* value defined, return empty s-expression. */
        return Ops::makeSExpression();
    }

    Either<Error,ValuePtr> builtin_def(EnvironmentPtr e, ValuePtr a) {
        return builtin_define(e,a,true);
    }


    Either<Error,ValuePtr> builtin_put(EnvironmentPtr e, ValuePtr a) {
        return builtin_define(e,a,false);
    }



    Either<Error,ValuePtr> builtin_list(EnvironmentPtr , ValuePtr a) {
        a->kind = Type::QExpression;
        return a;
    }

    Either<Error,ValuePtr> builtin_head(EnvironmentPtr , ValuePtr a) {
        if ( !a->isExpression() )  return Error {"argument to head function must be list expression." };
        ExpressionPtr expression = std::get<ExpressionPtr>(a->var);
        if (expression->cells.size() != 1) return Error { "head function passed more than one argument."};

        ValuePtr list = expression->cells[0];
        ExpressionPtr xs = std::get<ExpressionPtr>(list->var);

        ValuePtr head = xs->cells[0];
        ExpressionPtr result(new Expression());
        result->insert(head);

        return Ops::makeQExpression(result);
    }

    Either<Error,ValuePtr> builtin_tail(EnvironmentPtr , ValuePtr a) {
        if ( !a->isExpression() )  return Error { "argument to tail function must be list expression." };
        ExpressionPtr expression = std::get<ExpressionPtr>(a->var);
        if (expression->cells.size() != 1) return Error {"tail function passed more than one argument."};

        ValuePtr list = expression->cells[0];
        ExpressionPtr xs = std::get<ExpressionPtr>(list->var);

        xs->cells.pop_front(); /* Remove the head. */

        return Ops::makeQExpression(xs);
    }

    Either<Error,ValuePtr> builtin_eval(EnvironmentPtr e, ValuePtr a) {
        if (!a->isExpression()) return Error {"argument to eval function must be list expression."};
        ExpressionPtr expression = std::get<ExpressionPtr>(a->var);
        if (expression->cells.size() != 1) return Error { "eval function passed more than one argument."};

        ValuePtr xs = expression->cells[0];
        xs->kind = Type::SExpression;
        return eval(e, xs);
    }

    Either<Error,ValuePtr> builtin_join(EnvironmentPtr e, ValuePtr a) {
        if (!a->isExpression()) return Error {"argument to join function must be list expression."};
        ExpressionPtr expression = std::get<ExpressionPtr>(a->var);
        if (expression->cells.empty()) return a;

        ExpressionPtr xs(new Expression());

        while (!expression->cells.empty()) {
            ValuePtr ys = expression->cells[0]; expression->cells.pop_front();
            if ( ys->isExpression() ) {
                ExpressionPtr zs = std::get<ExpressionPtr>(ys->var);
                while ( !zs->cells.empty())  {
                   xs->cells.push_back(zs->cells[0]);
                   zs->cells.pop_front();
                }
            }
        }

        return Ops::makeQExpression(xs);
    }

    /* Define some primitive numerical operations, enough so that Prelude can boostrap more... */
    using integer_op = std::function<long(const long&, const long&)>;
    using double_op  = std::function<double(const double&, const double&)>;

    struct numeric_operators { integer_op  f; double_op   g; };

    template<typename T> T add(const T& a, const T& b) { return a+b; }
    template<typename T> T subtract(const T& a, const T& b) { return a-b; }
    template<typename T> T multiply(const T& a, const T& b) { return a*b; }
    template<typename T> T divide(const T& a, const T& b) {
        if ( b == 0 ) {
            throw std::runtime_error("divide by zero.");
        }
        return a/b;
    }


    Either<Error, ValuePtr> builtin_op(EnvironmentPtr /* ignore. */, ValuePtr vp, numeric_operators nop) {
        /*
         * 1. check that we have cells to reduce.
         * 2. runtime type check:
         *    Check that all cells are numeric.
         *    If any cell is double, we set the accumulator to double
         *    and cast any long to double as we accumulate.
         *    Otherwise, we treat as integer.
         * 3. On complete, we remove/delete the cells
         *    and return the value (type will be double or integer).
         */

        ExpressionPtr v = std::get<ExpressionPtr>(vp->var);

        if (v->cells.empty()) {
            return Error{"runtime error, no cells to reduce"};
        }

        bool is_double = false;
        for (const auto &c : v->cells) {
            if (!c->isNumeric()) return Error{"runtime_error, +,-,/,* reduce non numeric."};
            if (c->kind == Type::Double) is_double = true;
        }

        /* If we have a double then any integer value should be cast to double. */
        std::variant<long, double> accumulator;

        ValuePtr cell = v->cells[0];
        if ( is_double ) {
            double cell_val =
                    cell->kind == Type::Integer ? (double) std::get<long>(cell->var)
                                                       : std::get<double>(cell->var);
            accumulator =  cell_val;
        } else {
            accumulator = std::get<long>(cell->var);
        }

        for (int i = 1; i < v->cells.size(); i++) {
            ValuePtr c= v->cells[i];

            try {
                if (is_double) {
                    double cell_val =
                            c->kind == Type::Integer ? (double) std::get<long>(c->var) : std::get<double>(c->var);
                    accumulator = (double) nop.g(std::get<double>(accumulator), cell_val);
                } else {
                    accumulator = nop.f(std::get<long>(accumulator), std::get<long>(c->var));
                }
            } catch ( const std::runtime_error& e) {
                return Error {e.what()} ;
            }
        }

        v->cells.resize(0); /* erase the cells, they've been evaluated. */

        if (is_double) return Ops::makeDouble(std::get<double>(accumulator));
        return Ops::makeInteger(std::get<long>(accumulator));
    }

    using integer_cmp = std::function<bool(const long&, const long&)>;
    using double_cmp  = std::function<bool(const double&, const double&)>;

    struct numeric_cmp { integer_op  f; double_op   g; };

    Either<Error,ValuePtr> builtin_cmp(EnvironmentPtr, ValuePtr a, numeric_cmp op) {
        if (! a->isExpression() ) return Error { "expected expression  'cmp a b'"};
        ExpressionPtr xs = std::get<ExpressionPtr>(a->var);
        if ( xs->cells.size() != 2) return Error { "error cmp operator, expected 2 arguments."};

        bool isDouble = false;
        for (const auto& arg: xs->cells) {
            if ( !arg->isNumeric() ) return Error { "error, cmp argument non-numeric."};
            if ( arg->kind == Type::Double) isDouble = true;
        }

        bool result = false;
        ValuePtr x = xs->cells[0];
        ValuePtr y = xs->cells[1];
        if (isDouble) {  /* one or more arguments is double, so cast any args to double. */
            double fst = x->kind == Type::Integer ? (double) std::get<long>(x->var) : std::get<double>(x->var);
            double snd = y->kind == Type::Integer ? (double) std::get<long>(y->var) : std::get<double>(y->var);
            result = op.g(fst,snd);
        } else {
            long fst = std::get<long>(x->var);
            long snd = std::get<long>(y->var);
            result = op.f(fst,snd);
        }

        return result ? Ops::makeInteger(1) : Ops::makeInteger(0);
    }

    template<typename T> bool lt(const T& a, const T& b) { return a < b;}
    template<typename T> bool lte(const T& a, const T& b) { return a <= b;}
    template<typename T> bool gt(const T& a, const T& b) { return a > b;}
    template<typename T> bool gte(const T& a, const T& b) { return a >= b;}

    /* numeric only. */
    template<typename T> bool eq(const T& a, const T& b) { return a == b;}
    template<typename T> bool neq(const T& a, const T& b)  { return a != b; }

    auto builtin_lt = [](EnvironmentPtr e, ValuePtr v) { return builtin_cmp(e,v, {lt<long>,lt<double>});};
    auto builtin_lte = [](EnvironmentPtr e, ValuePtr v) { return builtin_cmp(e,v, {lte<long>,lte<double>});};
    auto builtin_gt = [](EnvironmentPtr e, ValuePtr v) { return builtin_cmp(e,v, {gt<long>,gt<double>});};
    auto builtin_gte = [](EnvironmentPtr e, ValuePtr v) { return builtin_cmp(e,v, {gte<long>,gte<double>});};

    /* test, numeric only. */
    auto builtin_eq= [](EnvironmentPtr e, ValuePtr v) { return builtin_cmp(e,v, {eq<long>,eq<double>});};
    auto builtin_neq= [](EnvironmentPtr e, ValuePtr v) { return builtin_cmp(e,v, {neq<long>,neq<double>});};


    auto builtin_add = [](EnvironmentPtr e, ValuePtr v)      { return builtin_op(e,v, { add<long>, add<double> }); };
    auto builtin_subtract = [](EnvironmentPtr e, ValuePtr v) { return builtin_op(e,v, { subtract<long>, subtract<double> }); };
    auto builtin_divide = [](EnvironmentPtr e, ValuePtr v)   { return builtin_op(e,v, { divide<long>, divide<double> }); };
    auto builtin_multiply = [](EnvironmentPtr e, ValuePtr v) { return builtin_op(e,v, { multiply<long>, multiply<double> }); };

    /* If. */
    Either<Error,ValuePtr> builtin_if(EnvironmentPtr e, ValuePtr v) {
        if ( !v->isExpression() ) return Error {"if statement not of form 'if (exp) [exp1] [exp2]'"};
        ExpressionPtr xs = std::get<ExpressionPtr>(v->var);
        if ( xs->cells.size() != 3) return Error {" if statement missing argument."};

        ValuePtr cond = xs->cells[0];
        ValuePtr exp1 = xs->cells[1];
        ValuePtr exp2 = xs->cells[2];

        if (cond->kind != Type::Integer) return Error {"if condition must return true or false."};
        if (exp1->kind != Type::QExpression) return Error {"true condition of if, not list expression type."};
        if (exp2->kind != Type::QExpression) return Error {"false condition of it, not list expression type."};

        /* Now set them to SExpression for eval. */
        exp1->kind = Type::SExpression;
        exp2->kind = Type::SExpression;

        long result = std::get<long>(cond->var);
        if (result) return eval(e,exp1);
        else return eval(e,exp2);
    }

    void addBuiltinFunctions(EnvironmentPtr env) {
        std::initializer_list<std::pair<std::string,BuiltinFunction>> builtins = {
                { "lambda", builtin_lambda},
                { "define", builtin_def},
                { "def", builtin_def},
                {"=",builtin_put},
                {"\\", builtin_lambda},
                { "list", builtin_list },
                { "head", builtin_head},
                {"tail", builtin_tail},
                { "eval", builtin_eval},
                {"join", builtin_join},
                { "+",builtin_add}, /* Basic numeric, useful only with bootstrapping in Prelude. */
                { "-",builtin_subtract},
                { "/",builtin_divide},
                {"*",builtin_multiply},
                {"<",builtin_lt},
                {"<=",builtin_lte},
                { ">",builtin_gt},
                {">=",builtin_gte},
                {"==",builtin_eq}, /* Should change to recur on types, compare Q Expression values etc. */
                {"!=",builtin_neq}, /* As above. */
                {"if",builtin_if}
        };

        for (const auto& kv: builtins ) {
            env->insert(kv.first, Ops::makeBuiltin(kv.second));
        }
    }

}