#pragma once

#include <unordered_map>
#include <string>


namespace inky {

    class value; /* forward declaration of value, lisp value. */

    /* An environment represents the set of expressions/definitions
     * within a scope. */
    class environment {
    public:
        environment() = default;

        /* remove an environment and cleanup the values associated with it. */
        ~environment();

        /* returns the value* associated with the name, or nullptr if not exists. */
        value* lookup(const std::string &name);

        /* insert a value for a given name. */
        void insert(const std::string &name, value* value);

    private:
        /* hash map of values in the environment; the environment manages the ownership of values. */
        std::unordered_map<std::string, value* > expressions;

        /* the outer environment. */
        std::shared_ptr <environment> outer;
    };

}