#ifndef UTILS_H_
#define UTILS_H_

#include "Number.cc"
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


// Basic Function type.
struct function {
    Python3Parser::SuiteContext *ptr;
    std::vector <Para_Type> paraVec;
};

struct Return_Type {
    antlrcpp::Any data;
};

// Data of Keyword Postioning
struct Keyword_Position {
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



bool isFirstChar(char _ch);

void getVariable(antlrcpp::Any &tmp);

bool isValidChar(char _ch);

bool validateVarName(const std::string& name);

double StringToFloat(const std::string &str);

int2048_t StringToInt(const std::string &str);

bool StringToBool(const std::string &str);

std::string IntToString(int2048_t tmp);

bool AnyToBool(const antlrcpp::Any &Var);

double AnyToDouble(const antlrcpp::Any &Var);

int2048_t AnyToInt(const antlrcpp::Any &Var);

std::string AnyToString(const antlrcpp::Any &Var);


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