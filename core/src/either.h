#pragma once

#include <variant>


namespace Inky::Lisp {

    /*
     * A Simple C++ equivalent of Haskell's data.Either.
     * Encapsulates the default of Left (Error) or Right(Result); all exceptions
     * are lifted into Either.
     */

    template<typename T> struct Left {
        Left (T t) : value(t) {}
        ~Left() = default;

        T value;
    };


    template<typename T> struct Right {
        Right(T t) : value(t) {}
        ~Right() = default;

        T value;
    };

    /*
     * Template for return types Either<Error,Result>;
     * Not yet added:  RightMap, LeftMap functions.
     */

    template<typename L, typename R> struct Either {
        Either(const R& result): var(result) {}
        Either(const L& error): var(error) {}
        Either(const Right<R>& result): var(result.value) {}
        Either(const Left<L>& result): var(result.value) {}

        [[nodiscard]] bool isLeft() const  { return std::holds_alternative<L>(var);  }
        [[nodiscard]] bool isRight()  const { return std::holds_alternative<R>(var); }

        operator bool() const { return isRight(); }

        L left() const { return std::get<L>(var); }
        R right() const { return std::get<R>(var); }

        std::variant<L,R> var;
    };

}