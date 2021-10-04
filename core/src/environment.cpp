#include <algorithm>
#include "environment.h"
#include "value.h"

namespace inky {

    environment::~environment() {
        /* if storing raw pointer, we can't just erase the map, need to call delete. */
        std::for_each(expressions.begin(),expressions.end(),[](const std::pair<std::string,value*>& kv) {
            delete kv.second;
        });
    }

    value* environment::lookup(const std::string &name) {
        auto i = expressions.find(name);
        if (i != expressions.end()) return i->second;
        else { // iteratively move up the outer environment scopes.
            for (auto j = outer; j != nullptr; j = j->outer) {
                i = outer->expressions.find(name);
                if (i != expressions.end()) return i->second;
            }
        }
        return nullptr;
    }

    void environment::insert(const std::string &name, value* v) {
        expressions[name] = v;
    }

}