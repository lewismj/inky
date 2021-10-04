#include <optional>
#include <regex>
#include <string>

#include "parser_t.h"
#include "unicode.h"
#include "value.h"


namespace inky::parser {

    /*
     * Notes:
     *   1. Flex/Bison introduce some build complexity for simple s-expression grammar;
     *   2. Haskell's Parsec style combinator would be ideal.
     *   3. C++ parser combinator like Spirit; not suitable - cumbersome, template/macro obfuscation etc.
     *      (Don't want the boost bloat).
     */

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

    either<error,value*> read_expr(iter_type i, iter_type end, bool is_quoted = false) {

        return left( error {"not implemented. "});
    }


    either<error,value*> read_str(iter_type i, iter_type end) {

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

            }
        } /* other clauses must be atoms = literal | symbol */


        skip_whitespace(i);
        return left(error {"not yet implementd."});
    }


    either<error,value*> parse(std::string_view input) {
        iter_type i = input.begin();
        return read_str(i, input.end());
    }


}