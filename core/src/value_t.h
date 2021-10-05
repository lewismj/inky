#pragma once

namespace inky {

    struct location { /* Location in string_view of the error. */
        size_t begin;
        size_t length;
    };

    struct error {
        std::string message; /* Error message. */
        location loc;        /* Position in the code that generated the error. */
    };

    /* Forward declarations. */
    class value;
    class environment;

    /* function; * as: f: environment . value -> value. */
    typedef std::function<value*(std::shared_ptr<environment>,value*)> function;

}