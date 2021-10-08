#pragma once

#include <cstdlib>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include "types.h"

/*
 * Defines a custom lexer/parser for the S-Expression based grammar.
 */
namespace inky {

    /*
     * Define simple S-expression grammar as follows:
     *
     * <P>  ::= <E>
     * <E>  ::= atom | ' <E> | ( <E> <Es> )
     * <Es> := <E> <Es> | ε
     *
     * FIRST:
     * <P>  -> { atom, ', ( }
     * <E>  -> { atom, ', ( }
     * <Es> -> { atom, ', (, ε }
     *
     * FOLLOW:
     * <P>  -> { ε }
     * <E>  -> { ε, atom, ', (, ) }
     * <Es> -> { ) }
     * atom -> { ε, atom, ', (, ) }
     * ' -> { atom, ', ( }
     * ( -> { atom, ', ( }
     * ) -> { ε, atom, ', (, ) }
     *
     * PREDICT:
     * <P> ::= <E> -> { atom, ', ( }
     * <E> ::= atom -> { atom }
     * <E> ::= ' <E> -> { ' }
     * <E> ::= ( <E> <Es> ) -> { ( }
     * <Es> ::= <E> <Es> -> { atom, ', ( }
     * <Es> ::= ε -> { ) }
     *
     * EPS: All non terminals that could expand to ε, the empty string.
     * FIRST(A)   : The set of tokens that could appear as the first token in an expansion of A.
     * FOLLOW(A)  : The set of tokens that could appear immediately after A, in an expansion of
     * S the start symbol.
     * PREDICT(A) : The set of tokens that could appear next in a valid parse of a string in the
     *              language, when the next non-terminal in the parse tree is A.
     *
     *  Notes:
     *      atom = symbol | string_literal | ... etc...
     */

    either<error,value*> parse(std::string_view input);

}