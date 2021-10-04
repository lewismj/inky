#include <optional>
#include <regex>
#include <string>
#include <sstream>

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

    using view_iterator = std::string_view::const_iterator;

    /* Given an iterator, skip over whitespace/comments. */
    void skip_whitespace(view_iterator i) {
        while ( std::isspace(*i) && *i != '\0') {
            if ( *i == ';')  while ( *i != '\n' && *i != '\0') i++;
            i++;
        }
    }


    /* Declaration of core fn, read a value from the view iterator i position. */
    either<error,value*> read_value(view_iterator i, view_iterator end);

    /* Read an expression, s-expression or quoted s-expression. */
    either<error,value*> read_expr(view_iterator i, view_iterator end, bool is_quoted = false) {
        value* val = is_quoted ? value::q_expression() : value::s_expression();
        while ( i != end ) { /* keep reading values ... */
          auto j = read_value(i,end);
          if ( j.is_right() ) {
             val->insert(j.right_value());
          } else {
              delete val;
              return j.left_value();
          }
        }
        return val;
    }

   /* Read a string literal. */
   either <error,value*> read_string_literal(view_iterator i, view_iterator end) {
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


    either<error,value*> read_value(view_iterator i, view_iterator end) {
        skip_whitespace(i);
        if ( i == end) return right(&null_opt);

        if ( *i == '(') { /* s-expression. */
            i++;
            return read_expr(i,end);
        }
        else if ( *i == '\'') { /* q-expression, quoted expression?. */
            ++i;
            if ( *i == '(') {
                i++;
                return read_expr(i, end, true);
            }
            else { /* syntax error, expecting '(' */
                /* TODO - need to enrich with location information, we have the view_iterator i. */
                return left(error {"parse error, expecting '(' after quote"});
            }
        } /* TODO- other clauses must be atoms = literal | symbol */


        skip_whitespace(i);
        return error {"not yet implemented."};
    }


    either<error,value*> parse(std::string_view input) {
        view_iterator i = input.begin();
        return read_value(i, input.end());
    }


}