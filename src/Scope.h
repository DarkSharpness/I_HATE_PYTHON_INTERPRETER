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
  public:
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
        // TODO : stack version.
    }

    /// @brief Find Variable and change it.
    /// If not found,add one in the current scope.
    variable &operator[](const std::string &Name) {
        auto iter = find(Name);
        if(iter) return *iter;
        else     return scope.back()[Name];
    }
    /// @brief Force to insert a pair in the last scope 
    void insert(const std::string &Name,const variable &var) {
        scope.back()[Name] = var;
    }

    // allocate one new space.
    void newSpace() {
        scope.emplace_back();
    }

    // remove the current space.
    void deleteSpace() {
        scope.pop_back();
    }

    NameSpace() {
        scope.resize(1);
        scope[0]["print"] = nullptr;
    }

    ~NameSpace() {
    }
};

#endif