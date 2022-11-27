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



struct None_Type {};

struct function {
    Python3Parser::SuiteContext *ptr;
    std::vector <Para_Type> paraVec;
};

struct Return_List : public std::vector <antlrcpp::Any> {};

struct Return_Type {
    Return_List data;
    // antlrcpp::Any data;
};

// Data of Keyword Postioning
struct Argument {
    std::string keyword;
    antlrcpp::Any value;
};


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

antlrcpp::Any operator -(const antlrcpp::Any &X);
antlrcpp::Any operator -(antlrcpp::Any &&X);

bool operator <(const antlrcpp::Any &X,const antlrcpp::Any &Y);
bool operator >(const antlrcpp::Any &X,const antlrcpp::Any &Y);
bool operator <=(const antlrcpp::Any &X,const antlrcpp::Any &Y);
bool operator >=(const antlrcpp::Any &X,const antlrcpp::Any &Y);
bool operator !=(const antlrcpp::Any &X,const antlrcpp::Any &Y);
bool operator ==(const antlrcpp::Any &X,const antlrcpp::Any &Y);



std::ostream& operator <<(std::ostream& os,const antlrcpp::Any &X);

#endif