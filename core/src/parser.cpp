#include <cmath>
#include <iterator>
#include <sstream>
#include <string>

#include "either.h"
#include "value.h"
#include "parser.h"


namespace Inky::Lisp {

    /*
     * Simple recursive descent parser; the parse errors could be enriched with
     * location information (since we know the position in the input string
     * where the error occurs). This could be done with use of a 'proper' repl:
     * readline/edit-line library.
     */
    class Parser {
    public:
       explicit Parser(std::string_view in) : input(in), i(in.begin()) {}
       ~Parser() = default;

       Either<Error,ValuePtr> parse() { return readExpressionType(Type::SExpression,'\0'); }

    private:

        /* Move one character along the input, provided we're not at the end of the input. */
        void advance() {
           if ( i != input.end() ) ++i;
        }

        bool isNumeric() {
            return (std::isdigit(*i)  || ( (*i == '+'|| *i=='-') && std::isdigit(*(i+1))));
        }

        bool isSymbol() {
            return std::strchr( "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "0123456789_+-*\\/=<>!&", *i);
        }

        void skipWhitespace() {
            while (i != input.end() && *i != '\0' && std::isspace(*i))  {
                if (*i == ';') {
                    while (i != input.end() && *i != '\n' && *i != '\0') advance();
                    return;
                }
                advance();
            }
        }

        Either<Error,ValuePtr> readValue() {
            skipWhitespace();

            if (i == input.end()) {
                size_t position = std::distance(input.begin(),i-1);
                return Error {"expecting token but reach end of input.", Error::Location { position, 1}};
            }

            Either<Error,ValuePtr> rtn(nullptr);

            if (*i == '(') {
                advance();
                rtn = readExpressionType(Type::SExpression,')');
            }
            else if (*i =='[') {
                advance();
                rtn = readExpressionType(Type::QExpression,']');
            }
            else if (*i == '\"') {
                advance();
                rtn = readStringLiteral();
            }
            else if (isNumeric()) {
                rtn = readNumericValue();
            }
            else if (isSymbol()) {
                rtn = readSymbol();
            }
            else {
                size_t position =  std::distance(input.begin(),i);
                return Error { "mismatched expression.", Error::Location { position , 1}};
            }

            skipWhitespace();

            return rtn;
        }

        Either<Error,ValuePtr> readExpressionType(Type kind, char end_ch) {
            ExpressionPtr expression = std::make_shared<Expression>(Expression());
            while ( *i != end_ch) {
                auto j = readValue();
                if (j) expression->insert(j.right()); else return j.left();
            }
            advance();
            return kind == Type::SExpression ?
                Ops::makeSExpression(expression) : Ops::makeQExpression(expression);
        }

        Either<Error,ValuePtr> readSymbol() {
            std::ostringstream os;
            while (strchr( "abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "0123456789_+-*\\/=<>!&", *i) && *i != '\0') {
                os << *i;
                advance();
            }
            return Ops::makeSymbol(os.str());
        }

        /* Read string literal. */
        Either <Error,ValuePtr> readStringLiteral() {
            std::string_view::const_iterator s = i;
            std::ostringstream os;
            while ( i!= input.end() && *i != '\"') {
                if ( *i == '\0' ) {
                    size_t start = std::distance(input.begin(), s);
                    size_t distance = std::distance(input.begin(), i) - start;
                    Error::Location l { start, distance };
                    return Error { "string literal not terminated", l };
                }
                os << *i;
                advance();
            }
            advance();
            return Ops::makeString(os.str());
        }

        /* Read an integer or double. */
        Either<Error,ValuePtr> readNumericValue() {
            /* Simplistic, we should hold 'val' as an unsigned long until (or if) we reach '.' or exponent etc.. */
            double sign = 1;
            if ((*i == '+' || *i == '-')) {
                sign = *i == '+' ? 1 : -1;
                advance();
            }
            double val = 0.0;
            double power = 0.0;

            for (val = 0.0; std::isdigit(*i); advance()) val = 10.0 * val + (*i - '0');
            if (*i == '.') advance();
            for (power = 1.0; std::isdigit(*i); advance()) {
                val = 10.0 * val + (*i - '0');
                power *= 10.0;
            }
            val = sign * val / power;
            double n = 0;
            double frac = std::modf(val, &n);
            if (frac == 0.0) { /* exactly zero => long. */
                return Ops::makeInteger((long) val);
            } else {
                return Ops::makeDouble(val);
            }
        }

    private:
        std::string_view input;
        std::string_view::const_iterator i;
    };


    Either<Error,ValuePtr> parse(std::string_view input) {
        Parser p(input);
        return p.parse();
    }

}
