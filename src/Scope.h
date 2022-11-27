#ifndef SCOPE_H_
#define SCOPE_H_

#include <unordered_map>
#include <string>
#include "support/Any.h"

#define variable antlrcpp::Any
//using variable = ; // basic variable_baseype


/// @brief Now it has only global variables.
struct NameSpace {
    /// @brief Custom namespace
    using Scope = std::unordered_map <std::string,variable>;
    std::vector <Scope> scope;

    /// @brief Find the pointer to the variable
    /// @param Name The variable's name
    /// @return nullptr if not found,varptr if found
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

    /// @brief Find Variable and change it.
    /// If not found,add one in the current scope.
    variable &operator[](const std::string &Name) {
        auto iter = find(Name);
        if(iter) return *iter;
        else     return scope.back()[Name];
    }

    // allocate one new space.
    Scope& newSpace() {
        scope.emplace_back();
        return scope.back();
    }

    // remove the current space.
    void deleteSpace() {
        scope.pop_back();
        
    }

    NameSpace() {
        scope.emplace_back();
    }

    ~NameSpace() = default;
};

#endif