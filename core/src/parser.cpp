#include <optional>
#include <regex>
#include <sstream>
#include <string>

#include "parser_t.h"
#include "unicode.h"
#include "value.h"


namespace inky::parser {

    /*
     * Notes:
     *   1. Flex/Bison introduce some build complexity for simple s-expression grammar?
     *   2. Haskell's Parsec style combinator would be ideal.
     *   3. C++ parser combinator like Spirit; not suitable - cumbersome, template/macro obfuscation etc.
     *      Offers nothing better than Flex/Yacc in terms of maintenance.
     *   4. Use utf8proc to make unicode compatible.
     *
     *   Next step; either convert to Flex/Bison? (can handle 'any' CFG, does -not- have to be LR(1).
     *            ; find simpler/better combinator than boost spirit.
     */

    /* Implements simple recursive descent parser (&lexer). */
    class parser {
    public:
        explicit parser(std::string_view input) : i(input.begin()), e(input.end()) {}
        ~parser() = default;

        either<error,value*> parse() { return read_value(); }

    private:

        void skip_whitespace() { /* skip over any whitespace or comments. */
            while (std::isspace(*i) &&  *i != '\0' && i != e) {
                if (*i == ';') {
                    while (*i != '\n' && *i != '\0') i++;
                    return;
                }
                i++;
            }
        }

        either<error,value*> read_value() {  /* read the next value. */
            skip_whitespace();
            if ( i == e) return error { "parse error, eoi."};

            if ( *i == '(') { /* s-expression. */
                i++;
                return read_expr();
            }
            else if ( *i == '\'') { /* q-expression, quoted expression?. */
                ++i;
                if ( *i == '(') {
                    i++;
                    return read_expr(true);
                }
                else { /* syntax error, expecting '(' */
                    /* TODO - need to enrich with location information, we have the view_iterator i. */
                    return left(error {"parse error, expecting '(' after quote"});
                }
            } /* ATOMS - string_literal | integer | double | ... | symbol */
            else if ( *i == '\"')  {
                return read_string_literal();
            }

            skip_whitespace();
            return error {"not yet implemented."};
        }


        either<error,value*> read_expr(bool is_quoted = false) { /* read an s-expression or quoted s-expression. */
            value* val = is_quoted ? new value(value::type::QExpression) : new value(value::type::SExpression);
            while (*i != ')') { /* keep reading values ... */
                auto j = read_value();
                if ( j.is_right() ) {
                    val->insert(j.right_value());
                } else {
                    delete val;
                    return j.left_value();
                }
            }
            return val;
        }

        either <error,value*> read_string_literal() { /* read string literal. */
            ++i; /* move onto the string. */
            std::ostringstream os;
            while ( *i != '"') {
                if ( *i == '\0' ) { /* reached eol before end quote. */
                    return error { "parser error, string literal not terminated."};
                }
                os << *i;
                i++;
            }
            std::string literal(os.str());
            return new value(literal,true);
        }

    private:
        std::string_view::const_iterator i;  /* current position in the input view. */
        std::string_view::const_iterator e;  /* the end of the input view. */
    };


    /* Invoke the parser; recursive descent. */
    either<error,value*> parse(std::string_view input) {
        parser p(input);
        return p.parse();
    }

}