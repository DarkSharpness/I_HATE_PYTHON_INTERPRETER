#ifndef SCOPE_H_
#define SCOPE_H_

#include <unordered_map>
#include <string>
#include "support/Any.h"

#define variable antlrcpp::Any


/// @brief Now it has only global variables.
struct NameSpace {
    using Scope = std::unordered_map <std::string,variable>;
    std::vector <Scope> scope;
  public:
    variable *find(const std::string &Name) {
        auto /*For similar form*/
        iter = scope.back().find(Name);
        if(iter != scope.back().end()) {
            return &(iter->second);
        }

        iter = scope.front().find(Name);
        if(iter != scope.front().end()) {
            return &(iter->second);
        }
        return nullptr;
    }

    variable &operator[](const std::string &Name) {
        auto iter = find(Name);
        if(iter != nullptr) return *iter;
        else     return scope.back()[Name];
    }

    void insert(const std::string &Name,const variable &var) {
        scope.back()[Name] = var;
    }

    // allocate one new space.
    Scope &newSpace() {
        scope.emplace_back();
        return scope.back();
    }

    // remove the current space.
    void deleteSpace() {
        scope.pop_back();
    }

    NameSpace() {
        scope.resize(1);
    }

    ~NameSpace() {
    }
};


#undef variable
#endif