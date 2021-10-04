#pragma once

namespace inky {

    struct location {
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

    /* function, note l_val may be a vector of val (recursively defined) we can define fn
     * as: f: environment . value -> value,
     * value may be a vector of values.
     */
    typedef std::function<value*(std::shared_ptr<environment>,value*)> function;

}