#pragma once

#include <ostream>
#include <unordered_map>
#include <ostream>

#include "either.h"
#include "value.h"

namespace Inky::Lisp {

    /*
     * An environment represents the set of definitions within a given scope.
     * Each environment contains a pointer to the parent outer scope.
     */

   class Environment {
   public:
       Environment() = default;
       ~Environment() = default;

      /* Returns the ValuePtr associated with the name, or nullptr if it doesn't exist. */
      ValuePtr lookup(const std::string& name) const;

      /* Insert a value for a given name. */
      void insert(const std::string& name, ValuePtr value);

      /*
       * Insert into global scope. Insert the symbol into the outermost scope that this
       * environment refers to.
       */
      void insertGlobal(const std::string& name, ValuePtr value);

      /* Set the outer scope of this environment. */
      void setOuterScope(EnvironmentPtr env);

      /* Returns the outer scope of this environment. */
      EnvironmentPtr getOuterScope();

      friend std::ostream& operator<<(std::ostream& os, EnvironmentPtr env);

   private:

       /*
        * Return the outermost scope of this environment;
        * n.b. Does NOT return shared_ptr to this, if 'this' is the global scope;
        * returns nullptr.
        */
       EnvironmentPtr getGlobalScope();

   private:
       /* An unordered map from symbol name to its Value. */
       std::unordered_map<std::string, ValuePtr> definitions;

      /* The outer environment. */
      EnvironmentPtr outer;
   };

   std::ostream& operator<<(std::ostream& os, EnvironmentPtr env);
}