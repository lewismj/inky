#pragma once

#include <unordered_map>
#include <string>
#include "types.h"


namespace inky {

    class value; /* forward declaration of value, lisp value. */

    /* An environment represents the set of expressions/definitions
     * within a scope. */
    class environment {
    public:
        environment() = default;
        ~environment() = default;

        /* returns the value* associated with the name, or nullptr if not exists. */
        value_ptr lookup(const std::string &name);

        /* insert a value for a given name. */
        void insert(const std::string& name, value_ptr value);

        /* insert a value for a given name in the outermost environment scope,
         * that this environment has access to. */
        void insert_outer(const std::string& name, value_ptr value);

    private:
        /* hash map of values in the environment; the environment manages the ownership of values. */
        std::unordered_map<std::string, value_ptr > expressions;

        /* the outer environment. */
        environment_ptr outer;
    };

}