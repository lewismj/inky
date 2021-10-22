#include <algorithm>

#include "value.h"
#include "environment.h"


namespace Inky::Lisp {

    ValuePtr Environment::lookup(const std::string& name) const {
        auto i = definitions.find(name);
        if ( i != definitions.end()) return i->second;
        else {
            for (auto j=outer; j!=nullptr; j=j->outer) {
                i = j->definitions.find(name);
                if (i != j->definitions.end()) return i->second;
            }
        }
        return nullptr;
    }

   void Environment::insert(const std::string& name, ValuePtr value) {
       definitions[name] = value;
   }

   EnvironmentPtr Environment::getGlobalScope() {
        if (outer == nullptr) return nullptr; /* n.b. we don't return a shared_ptr to this. */
        auto i = outer;
        auto j = outer;
        while ( j != nullptr ) {
            i = j;
            j = i->outer;
        }
        return i;
    }

    void Environment::insertGlobal(const std::string &name, ValuePtr value) {
        if ( outer == nullptr ) {
            insert(name,value);
        } else {
            EnvironmentPtr global = getGlobalScope();
            global->insert(name,value);
        }
    }

    void Environment::setOuterScope(EnvironmentPtr env) {
        if ( env.get() != this) outer = env;
    }

    EnvironmentPtr Environment::getOuterScope() {
        return outer;
    }

    EnvironmentPtr Environment::clone() {
        EnvironmentPtr env (new Environment());
        env->outer = outer;
        for(const auto& kv: definitions) {
            env->insert(kv.first,kv.second->clone());
        }
        return env;
    }


    std::ostream& operator<<(std::ostream& os, EnvironmentPtr env) {
        for (const auto& kv: env->definitions) os << "\t:" << kv.first << " :" << kv.second << "\n";
        if ( env->outer != nullptr ) {
            std::cout << "outer:\n";
            os << env->outer << "\n";
        }
        return os;
    }

}