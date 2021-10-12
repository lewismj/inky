#include <algorithm>
#include "environment.h"
#include "value.h"


namespace inky {


    [[maybe_unused]] value_ptr environment::lookup(const std::string &name) {
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

    void environment::insert(const std::string &name, value_ptr v) {
        expressions[name] = v;
    }

    void environment::insert_global(const std::string& name, value_ptr v) {
        if (outer == nullptr) insert(name,v);
        else {
           auto i = outer;
           while (i != nullptr) { i = i->outer; }
           i->insert(name, v);
        }
    }

    void environment::set_outer_scope(environment_ptr e) {
        outer = e;
    }

    std::ostream& operator<<(std::ostream& os, environment_ptr env) {

         os << "environment:\n";
         for (const auto& kv: env->expressions) {
             os << ":" << kv.first << " " << kv.second << "\n";
         }

         if ( env->outer != nullptr) {
             os << "parent ..\n";
             os << env->outer << "\n";
         }

        return os;
    }

}