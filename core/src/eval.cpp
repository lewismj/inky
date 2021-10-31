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


        ValuePtr eval(ValuePtr v) {
            switch (v->kind) {

                case Type::SExpression:
                    return evalSExpression(v);

                case Type::Symbol: {
                    auto key = std::get<std::string>(v->var);
                    auto lookup = env->lookup(key);
                    if (lookup) return lookup;
                    else return Ops::makeError(fmt::format("unbound symbol: {}",key));
                }

                case Type::Error:
                case Type::Integer:
                case Type::Double:
                case Type::String:
                case Type::QExpression:
                case Type::BuiltinFunction:
                case Type::Function:
                    return v;
            }
        }

        ValuePtr evalSExpression(ValuePtr vp) {
            ExpressionPtr v = std::get<ExpressionPtr>(vp->var);

            if ( v->cells.empty() ) return vp;
            if ( v->cells.size() == 1) return eval(v->cells[0]);

            /*
             *
             *  This small section of code can be used if you rely on []
             *  syntax for argument and function definitions.
             *  The [] syntax tags something as 'don't eagerly evaluate'.
             *
                    for (auto & cell : v->cells) {
                        auto maybe = eval(cell);
                        if ( maybe ) cell = maybe.right();
                        else return maybe.left();
                    }
             *
             * The while loop beneath is the 'special terms' required to
             * tweak the eval so that you can use the more regular ()
             * syntax, i.e.
             * We want to write
             *  (lambda (x) (+1 x)) 10
             *  instead of ( lambda [x] [+ 1 x]) 10
             */

            size_t k = 0;
            while ( k < v->cells.size() )  {
              if ( Ops::hasSymbolName(v->cells[k],"defun"))   {
                if ( k + 2 >= v->cells.size() ) {
                    return Ops::makeError("defun must contain formals and body arguments.");
                }

                /* defun (foo x y) (+ x y) => define (foo) (lambda (x y) (+ x y) */
                /* defun (args) (body) = define (head(args)) (lambda (tail args) (body)) */

                auto formals = v->cells[k+1];
                auto body = v->cells[k+2];
                /*
                 * we could doOps::makeSymbol("head") and Ops::makeSymbol("tail")
                 * and evaluate these functions to get the head and tail of the
                 * formals.
                 * It is quicker to just short-circuit that and manipulate the
                 * lists directly.
                 *
                 * Similarly, we could just invoke builtin functions here,
                 * but we can directly construct and put the function name into
                 * the environment etc.
                 *
                 */

                if ( !Ops::isExpression(formals) ) {
                    return Ops::makeError("formals to defun should be expression.");
                }
                ExpressionPtr xs = std::get<ExpressionPtr>(formals->var);
                if (xs->cells.size() < 2) {
                    return Ops::makeError("function must have name and at least one argument.");
                }
                ValuePtr functionName= xs->cells[0];
                if ( functionName->kind != Type::Symbol ) {
                   return Ops::makeError("function name must be a symbol.");
                }
                std::string name = std::get<std::string>(functionName->var);
                xs->cells.pop_front(); /* remove the function name from the formals. */
                EnvironmentPtr e(new Environment());
                ValuePtr lambda = Ops::makeFunction(std::make_shared<Lambda>(Lambda{ formals, body, e }));

                env->insert(name,lambda);

                v->cells.pop_front(); // defun.
                v->cells.pop_front(); // formals
                v->cells.pop_front(); // args.
                v->cells.push_front(lambda); // lambda.
              }
              else {
                  auto maybe = eval(v->cells[k]);
                  if ( ! Ops::isError(maybe)) {
                      /*
                       * The base implementation has a syntax [] for 'quoted expressions',
                       * eval([x]) = [x],
                       * This is useful for evaluation, e.g.
                       *
                       *  (lambda [x] [+ 1 x])  (+ 10 10)
                       *
                       *  Eval can just - evaluate all sub expressions, run the function.
                       *  Since [x] and [+1 x] return themselves.
                       *
                       *  To allow the more regular syntax we need to deal with lambda, define, and defun
                       *  in the eval:
                       */
                      if (Ops::hasSymbolName(v->cells[k], "lambda") || Ops::hasSymbolName(v->cells[k], "\\")) {
                          if (k + 2 >= v->cells.size()) {
                              return Ops::makeError("lambda definition must contain formals and body.");
                          }
                          v->cells[k] = maybe;

                          /*  Simply skip over the eval of the lambda formals and body defn;
                           * This allows:
                           *  lambda (x) (+ 1 x) to be written, rather than explicitly  lambda [x] [+1 x]
                           *  The [] syntax specified the type as 'q-expression' meaning just return self.
                           *  which is convenient, but 'not standard'.
                           */
                          k += 3; /* don't eval lambda function arguments on defn. */
                      } else if (Ops::hasSymbolName(v->cells[k], "def")
                                || Ops::hasSymbolName(v->cells[k], "define")
                                || Ops::hasSymbolName(v->cells[k],"="))
                        {
                          if (k + 2 >= v->cells.size()) {
                              return Ops::makeError("define must have two arguments.");
                          }
                          v->cells[k] = maybe;
                          if ( v->cells[k+1]->kind == Type::SExpression) v->cells[k+1]->kind = Type::QExpression;
                          else {
                              ExpressionPtr ys(new Expression());
                              ys->insert(v->cells[k+1]);
                              v->cells[k + 1] = Ops::makeQExpression(ys);
                          }

                          k += 2;
                      } else if (Ops::hasSymbolName(v->cells[k], "if")) {
                          /* if (condition) (then) (else) */
                          if (k + 3 >= v->cells.size()) {
                              return Ops::makeError("if statement must be of form if (condition) (then) (else).");
                          }
                          v->cells[k] = maybe;
                          auto cond = eval(v->cells[k + 1]);
                          if (!Ops::isError(cond)) {
                              v->cells[k + 1] = cond;
                          } else {
                              return cond; /* Just return the error immediately if the condition failed to eval. */
                          }
                          k += 4;
                      } else {
                          v->cells[k] = maybe;
                          ++k;
                      }
                  } else {
                      return maybe; /* Return the error, don't try to eval further? */
                  }
              }
            }

            /* applicative order eval, reduced the arguments, call the fn. */

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
            else {
                /* Make a list of the results, if one result return head. */
                ExpressionPtr xs(new Expression());
                for (size_t i=0; i<v->cells.size(); i++) {
                    if ( !Ops::isEmptyExpression(v->cells[i]))
                        xs->insert(v->cells[i]);
                }
                if (xs->cells.size()==1) return xs->cells[0];
                else return Ops::makeQExpression(xs);
            }
        }

        ValuePtr evalLambdaFunction(ValuePtr f, ValuePtr ar) {
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
            ValuePtr formalsValuePtr = fn->formals->clone(); /* clone the formals for each execution. */

            ExpressionPtr formals = std::get<ExpressionPtr>(formalsValuePtr->var);
            size_t formals_count = formals->cells.size();

            /* this flag is used to check if some args. initially supplied,
             * so that we can differentiate between lambda defn. and partial application.
             * i.e. both will have formals unspecified. */

            bool hasSomeArgs = !a->cells.empty();
            while ( ! a->cells.empty() ) {
                if (formals->cells.empty()) {
                    std::string str = fmt::format("function passed too many arguments {} , expected {}", arg_count, formals_count);
                    return Ops::makeError(str);
                }

                ValuePtr symbol = formals->cells[0];
                formals->cells.pop_front();
                if ( symbol->kind != Type::Symbol ) return Ops::makeError("function eval failed formal not a symbol.");
                std::string symbol_name = std::get<std::string>(symbol->var);

                if (symbol_name == "&") { /* variable arguments. */
                    if (formals->cells.size() != 1) {
                        return Ops::makeError("function signature invalid, varargs '&' must have one following symbol.");
                    }
                    /* We have something of the form "x & xs"; bind the 'xs' symbol to list of supplied args. */
                    ValuePtr tmp = formals->cells[0];
                    formals->cells.pop_front();
                    if ( tmp->kind != Type::Symbol ) return Ops::makeError("function formal not a symbol.");
                    ar->kind = Type::QExpression; /* make list expression. */
                    std::string xs_name = std::get<std::string>(tmp->var);
                    fn->env->insert(xs_name,ar);
                    break;
                }

                ValuePtr argument = a->cells[0]; a->cells.pop_front();
                fn->env->insert(symbol_name,argument);
            }

            if (!formals->cells.empty() && Ops::hasSymbolName(formals->cells[0],"&")) {
                /* Case deals with variable arguments where nothing supplied for the variable
                 * args, in this case, we can just assign the formal an empty expression. */
                if ( formals->cells.size() != 2 )
                    return Ops::makeError("function signature should be of form: x & xs when passing variable args.");
                formals->cells.pop_front();
                ValuePtr xs = formals->cells[0]; formals->cells.pop_front();
                ValuePtr exp = Ops::makeQExpression();
                if (xs->kind != Type::Symbol) return Ops::makeError("formal must be symbol.");
                fn->env->insert(std::get<std::string>(xs->var) ,exp);
            }

            if (formals->cells.empty()) { /* Fully supplied args. */
                fn->env->setOuterScope(env);
                ValuePtr body = fn->body->clone();
                body->kind = Type::SExpression;
                return Inky::Lisp::eval(fn->env->clone(), body);
            }
            else if ( hasSomeArgs && !formals->cells.empty() ) { /* Partially supplied args. */
                LambdaPtr lambda(new Lambda());
                lambda->formals = formalsValuePtr->clone();
                lambda->body = fn->body->clone();
                lambda->env = fn->env->clone();

                return Ops::makeFunction(lambda);
            }

            return f->clone(); // Return lambda, no args called yet, just return an empty instance for use.
        }

        ValuePtr evalBuiltinFunction(ValuePtr f, ValuePtr a) {
            auto fn = std::get<BuiltinFunction>(f->var);
            return fn(env, a);
        }

    private:
        EnvironmentPtr env;
    };


    ValuePtr eval(EnvironmentPtr env, ValuePtr val) {
        Eval ev(env);
        return ev.eval(val);
    }
}