#pragma once

#include <string>
#include <variant>

namespace inky::parser {

    /*
     * Types used by the parser:
     *  1. Haskell data.Either replacement; TODO - implement right/left map
     *  2. location - a position (start position and length) in input string.
     *  3. error - a concrete error type for parser.
     *  4. token_type - different types of allowable tokens.
     *  5. token - a parser token.
     */

    /* left .. part of either. */
    template<typename T> struct left {
        left(T t) : value(t) {}
        ~left() = default;

        T value;
    };

    template<typename L> bool operator==(const left<L>& a, const left<L>& b) { return a.value == b.value; }
    template<typename L> bool operator!=(const left<L>& a, const left<L>& b) { return ! (a==b); }
    template<typename L> bool operator==(const left<L>& a, const L& b) { return a.value == b; }
    template<typename L> bool operator!=(const left<L>& a, const L& b) { return ! (a==b); }
    template<typename L> bool operator==(const L& a, const left<L>& b) { return a == b.value; }
    template<typename L> bool operator!=(const L& a, const left<L>& b) { return ! (a==b); }

    /* right.. part of either. */
    template<typename T> struct right {
        right(T t) : value(t) {}
        ~right() = default;

        T value;
    };

    template<typename L> bool operator==(const right<L>& a, const right<L>& b) { return a.value == b.value; }
    template<typename L> bool operator!=(const right<L>& a, const right<L>& b) { return ! (a==b); }
    template<typename L> bool operator==(const right<L>& a, const L& b) { return a.value == b; }
    template<typename L> bool operator!=(const right<L>& a, const L& b) { return ! (a==b); }
    template<typename L> bool operator==(const L& a, const right<L>& b) { return a == b.value; }
    template<typename L> bool operator!=(const L& a, const right<L>& b) { return ! (a==b); }


    template<typename L, typename R> struct either {
        using error_type = L;
        using result_type = R;

        either(const R& result) { var = result;}
        either(const L& error) { var = error;}
        either(const right<R>& result) { var = result.value; }
        either(const left<L>& result) { var = result.value; }

        [[nodiscard]] bool is_left() const  { return std::holds_alternative<L>(var);  }
        [[nodiscard]] bool is_right()  const { return std::holds_alternative<R>(var); }

        operator bool() const { return is_right(); }

        L left_value() const { return std::get<L>(var); }
        R right_value() const { return std::get<R>(var); }

        std::variant<L,R> var;
    };

    template<typename L, typename R> bool operator==(const either<L,R>& a, const either<L,R>& b) {
        return a.var == b.var;
    }

    template<typename L, typename R> bool operator!=(const either<L,R>& a, const either<L,R>& b) {
        return a.var != b.var;
    }

    struct location {
        size_t begin;
        size_t length;
    };

    /* parser error message; can replace the placeholder 'error' used by value? */
    struct error {
        std::string message;
        location loc;
    };

    enum class token_type {
        Invalid,
        EndOfInput
    };

    struct token {
        token() = default;
        ~token() = default;

        token(token_type t,  std::string_view txt) : type(t),  text(txt) {}
        size_t size() const { return text.size(); }
        explicit operator token_type() const { return type; }

        std::string_view text;
        token_type type = token_type::Invalid;
    };

}