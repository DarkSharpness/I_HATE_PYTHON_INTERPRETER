#ifndef UTILS_CC_
#define UTILS_CC_

#include "Exception.h"
#include "Scope.h"
#include "support/Any.h"
#include "utils.h"
#include <ostream>

NameSpace scope;




/// @brief Judge whether it's a valid first char.
bool isFirstChar(char _ch) {
    return (_ch >= 'a' && _ch <= 'z') ||
           (_ch >= 'A' && _ch <= 'Z');
}

/// @brief Judge whether it's a valid char.
bool isValidChar(char _ch) {
    return (_ch >= 'a' && _ch <= 'z') ||
           (_ch >= 'A' && _ch <= 'Z') ||
           (_ch == '_') || isdigit(_ch);
}

/// @brief Judge whether it's a valid name. 
bool validateVarName(const std::string& Name) {
    if(!isFirstChar(Name.front())) return false;
    for(const auto &iter : KeyWords) {
        if(iter == Name) return false;
    }
    for(size_t i = 1 ; i < Name.length() ; ++i) {
        if(!isValidChar(Name[i])) return false;
    }
    return true;
}

/// @brief Change ANY variable into bool type
bool AnyToBool(const antlrcpp::Any &Var) {
    if(Var.is<bool>()) {
        return Var.as<bool>();
    } else if(Var.is<int2048_t>()) {
        return bool(Var.as<int2048_t>());
    } else if(Var.is<double>()) {
        return bool(Var.as<double>());
    } else {/*Var.as<std::string>*/
        return !bool(Var.as<std::string>().empty());
    }
}

/// @brief Change ANY variable into double type
double AnyToDouble(const antlrcpp::Any &Var) {
    if(Var.is<double>()) {
        return Var.as<double>();
    } else if(Var.is<int2048_t>()) {
        return double(Var.as<int2048_t>());
    } else if(Var.is<bool>()) {
        return double(Var.as<bool>());
    } else {/*Var.as<std::string>*/
        return StringToFloat(Var.as<std::string>());
    }
}

/// @brief Change ANY variable into int2048_t type
int2048_t AnyToInt(const antlrcpp::Any &Var) {
    if(Var.is<int2048_t>()) {
        return Var.as<int2048_t>();
    } else if(Var.is<double>()) {
        return int2048_t(Var.as<double>());
    } else if(Var.is<bool>()) {
        return int2048_t(Var.as<bool>());
    } else {/*Var.as<std::string>*/
        return StringToInt(Var.as<std::string>());
    }
}

/// @brief Change ANY variable into std::string
std::string AnyToString(const antlrcpp::Any &Var) {
    if(Var.is<std::string>()) {
        return Var.as<std::string>();
    } else if(Var.is<int2048_t>()) {
        return IntToString(Var.as<int2048_t>());
    } else if(Var.is<double>()) {
        return std::to_string(Var.as<double>());
    } else {
        return Var.as<bool>() ? "True" : "False";
    }
}


/// @brief Change string into double type.
double StringToFloat(const std::string &str) {
    return std::stod(str);
}

/// @brief Change string into int2048_t type. 
int2048_t StringToInt(const std::string &str) {
    // TODO : RE-write in int2048
    // int2048 have built in functions

    //std::cout << str << "INT_CONVERSION\n";
    int2048_t ans = 0;
    bool flag = false;
    for(char _ch : str) {
        if(_ch == '-') flag ^= true; 
        if(!isdigit(_ch)) continue;
        ans = ans * 10 + (_ch ^ '0');
    }
    return flag ? -ans : ans;
}

/// @brief Change string into bool type
bool StringToBool(const std::string &str) {
    return !str.empty();
}


std::string IntToString(int2048_t tmp) {
    if(tmp == 0) {return "0";}
    std::string str;
    if(tmp < 0) {
        tmp = -tmp; // can be optimized
        str.push_back('-');
    }
    while(tmp) {
        str.push_back((tmp % 10) ^ '0');
        tmp /= 10;
    }
    std::reverse(str.begin() + (str[0] == '-'),str.end());
    return str;
}



antlrcpp::Any operator +(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<std::string>() || Y.is<std::string>()) {
        return X.as<std::string>() + Y.as<std::string>();
    } else if(X.is<double>() || Y.is<double>()) {
        return AnyToDouble(X) + AnyToDouble(Y);
    } else {
        return AnyToInt(X) + AnyToInt(Y);
    }
}
// TODO
antlrcpp::Any& operator +=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return X = X + Y;
}

/*
antlrcpp::Any& operator +=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<std::string>() || Y.is<std::string>()) {
        X.as<std::string>() += Y.as<std::string>();
    } else if(X.is<double>()) {
        X.as<double>() += AnyToDouble(Y);
    } else if(Y.is<double>()) {
        X = AnyToDouble(X) + Y.as<double>();
    } else if(X.is<int2048_t>()) {
        X.as<int2048_t>() += AnyToInt(Y);
    } else {
        X = X.as<bool>() + AnyToInt(Y);
    }
    return X;
}
*/


antlrcpp::Any operator -(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<double>() || Y.is<double>()) {
        return AnyToDouble(X) - AnyToDouble(Y);
    } else {
        return AnyToInt(X) - AnyToInt(Y);
    }
}

antlrcpp::Any& operator -=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return X = X - Y;
}
/*
antlrcpp::Any& operator -=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<double>()) {
        X.as<double>() -= AnyToDouble(Y);
    } else if(Y.is<double>()){
        X = AnyToDouble(X) - Y.as<double>();
    } else if(X.is<int2048_t>()){
        X.as<int2048_t>() -= AnyToInt(Y);
    } else {
        X = X.as<bool>() - AnyToInt(Y);
    }
    return X;
}
*/


antlrcpp::Any operator *(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<std::string>() || Y.is<std::string>()) {
        const std::string &str = X.is<std::string>() ? X.as<std::string>(): 
                                                       Y.as<std::string>();
        int len = int(X.is<std::string>() ? Y.as<int2048_t>():
                                            X.as<int2048_t>());
        std::string ans;
        ans.reserve(str.size() * len);
        while(len--) {ans.append(str);}
        return ans;
    } else if(X.is<double>() || Y.is<double>()) {
        return AnyToDouble(X) * AnyToDouble(Y);
    } else {
        return AnyToInt(X) * AnyToInt(Y);
    }
}

antlrcpp::Any& operator *=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return X = X * Y;
}


/*
antlrcpp::Any& operator *=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<std::string>() || Y.is<std::string>()) {
        const std::string &str = X.is<std::string>() ? X.as<std::string>(): 
                                                       Y.as<std::string>();
        int len = int(X.is<std::string>() ? Y.as<int2048_t>():
                                            X.as<int2048_t>());
        std::string ans;
        ans.reserve(str.size() * len);
        while(len--) {ans.append(str);}
        X = ans;        
    } else if(X.is<double>()) {
        X.as<double>() *= AnyToDouble(Y);
    } else if(Y.is<double>()) {
        X = AnyToDouble(X) * Y.as<double>(); 
    } else if(X.is<int2048_t>()) {
        X.as<int2048_t>() *= AnyToInt(Y);
    } else {
        X = X.as<bool>() * AnyToInt(Y); 
    }
    return X;
}
*/


antlrcpp::Any operator /(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return AnyToDouble(X) / AnyToDouble(Y);
}


antlrcpp::Any& operator /=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return X = X / Y;
}


/*
antlrcpp::Any& operator /=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<double>()) {
        X.as<double>() /= AnyToDouble(Y);
    } else {
        X = X / Y;
    }
    return X;
}
*/


antlrcpp::Any operator |(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return AnyToInt(X) / AnyToInt(Y);
}

antlrcpp::Any& operator |=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return X = X | Y;
}


/*
antlrcpp::Any& operator |=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<int2048_t>()) {
        X.as<int2048_t>() /= AnyToInt(Y);    
    } else {
        X = X | Y;
    }
    return X;
}
*/


antlrcpp::Any operator %(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return AnyToInt(X) % AnyToInt(Y);
}


antlrcpp::Any& operator %=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return X = X % Y;
}

/*
antlrcpp::Any& operator %=(antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<int2048_t>()) {
        X.as<int2048_t>() %= AnyToInt(Y);    
    } else {
        X = X % Y;
    }
    return X;
}
*/


antlrcpp::Any operator -(const antlrcpp::Any &X) {
    if(X.is<double>()) return -X.as<double>();
    else return -AnyToInt(X);
}
antlrcpp::Any operator -(antlrcpp::Any &&X) {
    if(X.is<double>()) return -X.as<double>();
    else return -AnyToInt(X);
}

bool operator <(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<std::string>() || Y.is<std::string>()) {
        // std::cout <<"COMP_STR\n";
        return X.as<std::string>() < Y.as<std::string>();
    } else if(X.is<double>() || Y.is<double>()) {
        // std::cout <<"COMP_DB\n";
        return AnyToDouble(X) < AnyToDouble(Y);
    } else {
        // std::cout <<"COMP_INT\n";
        return AnyToInt(X) < AnyToInt(Y);
    }
}


bool operator >(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return Y < X;
}
bool operator <=(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return !(Y < X);
}
bool operator >=(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return !(X < Y);
}
bool operator !=(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    return !(X == Y);
}
bool operator ==(const antlrcpp::Any &X,const antlrcpp::Any &Y) {
    if(X.is<std::string>() || Y.is<std::string>()) {
        if(!Y.is<std::string>() || ! X.is<std::string>()) {
            return false;
        }
        return X.as<std::string>() == Y.as<std::string>();
    } else if(X.is<double>() || Y.is<double>()) {
        return AnyToDouble(X) == AnyToDouble(Y);
    } else {
        return AnyToInt(X) == AnyToInt(Y);
    }
}



std::ostream& operator <<(std::ostream& os,const antlrcpp::Any &X) {
    if(X.is<int2048_t>())        {os << X.as<int2048_t>();}
    else if(X.is<std::string>()) {os << X.as<std::string>();}
    else if(X.is<double>())      {os << X.as<double>();}
    else if(X.is<bool>()) {
        os << (X.as<bool>() ? "True" : "False");
    } else {os << "WTF is it!!!!!!!!!!!";} // UNEXPECTED ERROR
    return os;
}


#endif