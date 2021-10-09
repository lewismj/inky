#include <cmath>
#include <iterator>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

#include "value.h"
#include "parser.h"


namespace inky {

    /*
     * Notes:
     *   1. Flex/Bison introduce some build complexity for simple s-expression grammar?
     *   2. Haskell's Parsec style combinator would be ideal.
     *   3. C++ parser combinator like Spirit; not suitable - cumbersome, template/macro obfuscation etc.
     *      Offers nothing better than Flex/Yacc in terms of maintenance.
     *   4. Use utf8proc to make unicode compatible.
     *
     *  todo:
     *   Next step; either convert to Flex/Bison? (can handle 'any' CFG, does -not- have to be LR(1).
     *            ; find simpler (or at least smaller!) parser combinator than boost spirit.
     *
     *  todo:
     *   Or, since the 'programs' are very small, just have two steps 'generate lexemes' and 'parse tokens'.
     *   the 'parse tokens' becomes more readable (See header file) at the cost of reading all the input in
     *   one go, rather that via the iterator...
     */

    /* Implements simple recursive descent parser (&lexer). */
    class parser {
    public:
        explicit parser(std::string_view input) : b(input.begin()), i(input.begin()), e(input.end()) {}
        ~parser() = default;

        either<error,value_ptr> parse() { return read_expr(false,'\0'); }

    private:

        void skip_whitespace() { /* skip over any whitespace or comments. */
            while (std::isspace(*i) &&  *i != '\0' && i != e) {
                if (*i == ';') {
                    while (*i != '\n' && *i != '\0') i++;
                    return;
                }
                ++i;
            }
        }

        either<error,value_ptr> read_value() {  /* read the next value. */
            skip_whitespace();

            /* when we read the expression, value, etc.. we don't return
             * immediately, since we want to remove whitespace after the read too. */
            either<error,value_ptr > rtn = value_ptr(nullptr);

            if ( i == e) {
                size_t position = std::distance(b,i-1);
                return error { "parse error, expecting token, reached end of input:",
                               location { position, 1} };
            }
            if ( *i == '(') { /* s-expression. */
                ++i;
                rtn = read_expr();
            }
            else if ( *i == '\'') { /* q-expression, quoted expression?. */
                //++i;
                skip_whitespace();
                if ( *i == '(') {
                    ++i;
                    rtn = read_expr(true);
                }
                else { /* syntax error, expecting '(' */
                    size_t position = std::distance(b,i-1);
                    return left(error {"parse error, expecting '(' after quote:",
                                       location { position, 2 }});
                }
            } /* ATOMS - string_literal | integer | double | ... | symbol */
            else if ( *i == '\"')  {
                rtn = read_string_literal();
            } /* +-[0-9] is number; + n ; symbol number.; number is int if atoi = atof; i.e. modf = 0.0 */
            else if (std::isdigit(*i)  || ( (*i == '+'|| *i=='-') && std::isdigit(*(i+1)))) {
                /* ? could check that number literal isn't in position of operator; or allow e.g. '+2' as an
                 * symbol/operator that could be bound itself... ? */
                double sign = 1;
                if ( (*i == '+' ||  *i == '-')) {
                    sign = *i == '+' ? 1 : -1;
                    ++i;
                }
                double val = 0.0;
                double power = 0.0;

                /* Only read valid number; i.e 2a is considered two tokens 2 and 'a';
                 * can't have identifier 2a etc.. */
                for (val = 0.0; std::isdigit(*i);i++) val = 10.0 * val + ( *i - '0');
                if (*i == '.') i++;
                for (power = 1.0; std::isdigit(*i);i++) {
                    val = 10.0 * val + (*i  - '0');
                    power *= 10.0;
                }
                val = sign * val/power;
                double n = 0;
                double frac = std::modf(val,&n);
                if (frac == 0.0) { /* exactly zero => long. */
                    rtn = value_ptr(new value ( (long) val));
                } else {
                    rtn = value_ptr(new value(val));
                }
            }
            else if (strchr( "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "0123456789_+-*\\/=<>!&", *i)) {
                rtn = read_symbol();
            }
            else {
                size_t position = std::distance(b, i);
                return error{"parse error, mismatched expression:", location{position, 1}};
            }

            skip_whitespace();

            return rtn;
        }


        either<error,value_ptr> read_expr(bool is_quoted = false, char end_ch =')') { /* read an s-expression or quoted s-expression. */
            value* val = is_quoted ? new value(value::type::QExpression) : new value(value::type::SExpression);
            while (*i != end_ch) { /* keep reading values ... */
                auto j = read_value();
                if ( j.is_right() ) {
                    val->insert(j.right_value());
                } else {
                    delete val;
                    return j.left_value();
                }
            }
            ++i;
            return value_ptr(val);
        }

        either<error,value_ptr> read_symbol() {
            std::ostringstream os;
            while (strchr( "abcdefghijklmnopqrstuvwxyz"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "0123456789_+-*\\/=<>!&", *i) && *i != '\0') {
               os << *i;
               ++i;
            }
            i++;
            return value_ptr(new value(os.str()));
        }

        either <error,value_ptr> read_string_literal() { /* read string literal. */
            ++i; /* move onto the string. */
            std::string_view::const_iterator s = i; /* mark start of string, if we error. */
            std::ostringstream os;
            while ( *i != '"') {
                if ( *i == '\0' ) { /* reached eol before end quote. */
                    size_t start = std::distance(b,s);
                    size_t distance = std::distance(b,i) - start;
                    location loc { start, distance };
                    return error { "parse error, string literal not terminated:", loc };
                }
                os << *i;
                ++i;
            }
            std::string literal(os.str());
            return value_ptr(new value(literal,true));
        }

    private:
        std::string_view::const_iterator b;  /* the beginning of the input. */
        std::string_view::const_iterator i;  /* current position in the input view. */
        std::string_view::const_iterator e;  /* the end of the input view. */
    };


    /* Invoke the parser; recursive descent. */
    either<error,value_ptr> parse(std::string_view input) {
        parser p(input);
        return p.parse();
    }

}