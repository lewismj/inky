#include <optional>
#include <regex>
#include <string>

#include "parser_t.h"
#include "unicode.h"
#include "value.h"


namespace inky::parser {

    using iter_type = std::string_view::const_iterator;

    /* Given an iterator, skip over whitespace/comments. */
    void skip_whitespace(iter_type i) {
        while ( isspace(*i) && *i != '\0') {
           if ( *i == ';')  { /* lisp comment ... */
               while ( *i != '\n' && *i != '\0') i++;
           }
        }
        i++;
    }

    either<error,value*> read_expr(iter_type i, bool is_quoted = false) {

        return left( error {"not implemented. "});
    }


    either<error,value*> parse_str(std::string_view input, iter_type i) {

        skip_whitespace(i);
        if ( i == input.end() ) return right(&null_opt);

        if ( *i == '(') { /* s-expression. */
            i++;
            return read_expr(i);
        }
        else if ( *i == '\'') { /* q-expression, quoted expression?. */
            ++i;
            if ( *i == '(') {
                i++;
                return read_expr(i, true);
            }
            else { /* syntax error, expecting '(' */

            }
        } /* other clauses must be atoms = literal | symbol */


        skip_whitespace(i);
        return left(error {"fatal error parsing input."});
    }


    either<error,value*> parse(std::string_view input) {
        /* Loop until null_opt?? */
        iter_type i = input.begin();
        return parse_str(input,i);
    }



}