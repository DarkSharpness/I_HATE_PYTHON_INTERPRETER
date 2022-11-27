#ifndef UTILS_H_
#define UTILS_H_

#include "support/Any.h"
#include "Python3BaseVisitor.h"
#include <string>


enum AugAssignOP {
    ADD_ASSIGN_OP,
    SUB_ASSIGN_OP,
    MULT_ASSIGN_OP,
    DIV_ASSIGN_OP,
    IDIV_ASSIGN_OP,
    MOD_ASSIGN_OP
};


struct Para_Type {
    std::string name;
    antlrcpp::Any val;
    Para_Type() = default;
    Para_Type(const std::string &str,const antlrcpp::Any &v):name(str),val(v){}
};

// struct None_Type {};

// Basic Function type.
struct function {
    Python3Parser::SuiteContext *ptr;
    std::vector  <Para_Type> paraVec;
};

struct Return_Type {
    antlrcpp::Any data;
};

// Data of Keyword Postioning
struct Keyword_Position {
    std::string keyword;
    antlrcpp::Any value;
};

struct No_Return {};

/// @brief Record all the keywords.
const std::string KeyWords[] = {
    "None","True","False",
    "def",
    "return","break","continue",
    "if","elif","else","while",
    "or","and","not",
    "int","float","str","bool",
    "print",
};

// break and continue
enum Flow_Type {
    BREAK_CASE,
    CONTINUE_CASE,
};


antlrcpp::Any operator +(const antlrcpp::Any &X,const antlrcpp::Any &Y);
antlrcpp::Any& operator +=(antlrcpp::Any &X,const antlrcpp::Any &Y);

antlrcpp::Any operator -(const antlrcpp::Any &X,const antlrcpp::Any &Y);
antlrcpp::Any& operator -=(antlrcpp::Any &X,const antlrcpp::Any &Y);

antlrcpp::Any operator *(const antlrcpp::Any &X,const antlrcpp::Any &Y);
antlrcpp::Any& operator *=(antlrcpp::Any &X,const antlrcpp::Any &Y);


antlrcpp::Any operator /(const antlrcpp::Any &X,const antlrcpp::Any &Y);
antlrcpp::Any& operator /=(antlrcpp::Any &X,const antlrcpp::Any &Y);


antlrcpp::Any operator |(const antlrcpp::Any &X,const antlrcpp::Any &Y);
antlrcpp::Any& operator |=(antlrcpp::Any &X,const antlrcpp::Any &Y);


antlrcpp::Any operator %(const antlrcpp::Any &X,const antlrcpp::Any &Y);
antlrcpp::Any& operator %=(antlrcpp::Any &X,const antlrcpp::Any &Y);

antlrcpp::Any operator -(antlrcpp::Any &&X);

bool operator < (const antlrcpp::Any &X,const antlrcpp::Any &Y);
bool operator > (const antlrcpp::Any &X,const antlrcpp::Any &Y);
bool operator <=(const antlrcpp::Any &X,const antlrcpp::Any &Y);
bool operator >=(const antlrcpp::Any &X,const antlrcpp::Any &Y);
bool operator !=(const antlrcpp::Any &X,const antlrcpp::Any &Y);
bool operator ==(const antlrcpp::Any &X,const antlrcpp::Any &Y);



std::ostream& operator <<(std::ostream& os,const antlrcpp::Any &X);

#endif