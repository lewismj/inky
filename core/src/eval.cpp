#include <fmt/core.h>
#include <sstream>

#include "environment.h"
#include "value.h"

#include "eval.h"

namespace Inky::Lisp {

    class Eval {
    public:

        explicit Eval(EnvironmentPtr e): env(e) {}

        ~Eval() = default;


        Either<Error,ValuePtr> eval(ValuePtr v) {
            switch (v->kind) {

                case Type::Symbol: {
                    auto key = std::get<std::string>(v->var);
                    auto lookup = env->lookup(key);
                    if (lookup) {
                        return lookup;
                    }
                    else {
                        return Error {fmt::format("unbound symbol: {}",key)};
                    }
                }

                case Type::SExpression:
                    return evalSExpression(v);

                case Type::Integer:
                case Type::Double:
                case Type::String:
                case Type::QExpression:
                case Type::BuiltinFunction:
                case Type::Function:
                    return v;
            }
        }

        Either<Error,ValuePtr> evalSExpression(ValuePtr vp) {
            ExpressionPtr v = std::get<ExpressionPtr>(vp->var);

            if ( v->cells.empty() ) return vp;
            if ( v->cells.size() == 1) return eval(v->cells[0]);

            for (auto & cell : v->cells) {
                auto maybe = eval(cell);
                if ( maybe ) cell = maybe.right();
                else return maybe.left();
            }

            /* First cell should be a function type (builtin or defined). */
            if ( v->cells[0]->kind == Type::BuiltinFunction ) {
                ValuePtr fn = v->cells[0];
                v->cells.pop_front();
                return evalBuiltinFunction(fn,vp);
            }
            else if ( v->cells[0]->kind == Type::Function )  {
                ValuePtr lambda = v->cells[0];
                v->cells.pop_front();
                return evalLambdaFunction(lambda,vp);

            }

            return Error {fmt::format("expression evaluation failed, unknown type.")};
        }

        Either<Error,ValuePtr> evalLambdaFunction(ValuePtr f, ValuePtr ar) {
            /* f contains:
             * the struct 'lambda':
             *  formals (Argument specification).
             *  body (Body of the function itself).
             *  evaluation environment.
             *
             *  a sexpression:
             *   contains the arguments to pass to the function.
             */
            LambdaPtr fn = std::get<LambdaPtr>(f->var);

            /* copy arguments to formals;
             * i)   if too many arguments - return an Error.
             * ii)  too few, then partially apply.
             * iii) allow variable number of args.
             */

            ExpressionPtr a = std::get<ExpressionPtr>(ar->var);

            size_t arg_count = a->cells.size();
            ValuePtr formalsV = fn->formals->clone(); /* clone the formals for each execution. */
            ExpressionPtr formals = std::get<ExpressionPtr>(formalsV->var);

            size_t formals_count = formals->cells.size();


            while ( ! a->cells.empty() ) {
                if (formals->cells.empty()) {
                    std::string str = fmt::format("function passed too many arguments {} , expected {}", arg_count, formals_count);
                    return Error { str };
                }

                ValuePtr symbol = formals->cells[0]; formals->cells.pop_front();
                if ( symbol->kind != Type::Symbol ) return Error {"function eval failed formal not a symbol."};
                std::string symbol_name = std::get<std::string>(symbol->var);

                if ( symbol_name == "&") {
                    if ( formals->cells.size() != 1) {
                        return Error {"function formal signature invalid, varargs '&' must have 1 following symbol."} ;
                    }
                    /* we have something of the form "x & xs"; bind the 'xs' symbol to list of supplied args. */
                    ValuePtr tmp = formals->cells[0]; formals->cells.pop_front();
                    if ( tmp->kind != Type::Symbol) return Error { "function formal not symbol."};
                    auto f = Ops::makeSymbol("list");
                    auto xs = evalBuiltinFunction(f,ar) ;
                    //auto xs = inky::builtin::builtin_list(env,a);
                    if ( xs ) fn->env->insert(std::get<std::string>(tmp->var),xs.right());
                    else return xs.left(); /* error */
                }

                ValuePtr argument = a->cells[0]; a->cells.pop_front();
                fn->env->insert(symbol_name,argument);
            }

            if (!formals->cells.empty()) {
                ValuePtr tmp = formals->cells[0];
                if ( tmp->kind == Type::Symbol && std::get<std::string>(tmp->var) == "&") {
                    if ( formals->cells.size() != 2 ) {
                        return Error {"function signature should be of the form: x & xs, for variable arguments."};
                    }
                    formals->cells.pop_front();
                    ValuePtr xs = formals->cells[0]; formals->cells.pop_front();
                    if ( xs->kind != Type::Symbol) return Error { "function formal must be symbol."};
                    ValuePtr q_expr = Ops::makeQExpression();
                    fn->env->insert(std::get<std::string>(xs->var),q_expr);
                }
            }

            if (formals->cells.empty()) { /* all arguments have been supplied. */
                fn->env->setOuterScope(env);
                ValuePtr body = fn->body->clone();
                body->kind = Type::SExpression;
                return Inky::Lisp::eval(fn->env, body);
            }

            return f->clone(); /* partially applied function. */
        }

        Either<Error, ValuePtr> evalBuiltinFunction(ValuePtr f, ValuePtr a) {
            auto fn = std::get<BuiltinFunction>(f->var);
            return fn(env, a);
        }

    private:
        EnvironmentPtr env;
    };


    Either<Error,ValuePtr> eval(EnvironmentPtr env, ValuePtr val) {
        Eval ev(env);
        return ev.eval(val);
    }
}