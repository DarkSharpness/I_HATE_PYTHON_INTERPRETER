#ifndef UTILS_CC_
#define UTILS_CC_

#include "Exception.h"
#include "Scope.h"
#include "support/Any.h"
#include "utils.h"
#include <cmath>
#include "Number.cc"
#include <ostream>


NameSpace scope;

/// @brief Change string into int2048_t type. 
int2048_t StringToInt(const std::string &str) {
    return int2048_t(str);
}

/// @brief Change string into double type.
double StringToFloat(const std::string &str) {
    return std::stod(str);
}


/// @brief Change string into bool type
bool StringToBool(const std::string &str) {
    return !str.empty();
}

std::string IntToString(const int2048_t &tmp) {
    return tmp._TOSTRING();
}

double IntToFloat(const int2048_t &tmp) {
    return double(tmp);
}

bool IntToBool(const int2048_t &tmp) {
    return bool(tmp);
}

std::string FloatToString(double tmp) {
    return std::to_string(tmp);
}

int2048_t FloatToInt(double tmp) {
    return sjtu::dto2048(tmp);
}

bool FloatToBool(double tmp) {
    return tmp;
}


std::string boolToString(bool tmp) {
    static const std::string str[2] = {"True","False"};
    return str[tmp];
}

int2048_t boolToInt(bool tmp) {
    return int2048_t(tmp);
}

double boolToFloat(bool tmp) {
    return tmp;
}






/// @brief Change ANY variable into bool type
bool AnyToBool(const antlrcpp::Any &Var) {
    if(Var.is<bool>()) {
        return Var.as<bool>();
    } else if(Var.is<int2048_t>()) {
        return IntToBool(Var.as<int2048_t>());
    } else if(Var.is<double>()) {
        return FloatToBool(Var.as<double>());
    } else {/*Var.as<std::string>*/
        return StringToBool(Var.as<std::string>());
    }
}

/// @brief Change ANY variable into double type
double AnyToDouble(const antlrcpp::Any &Var) {
    if(Var.is<double>()) {
        return Var.as<double>();
    } else if(Var.is<int2048_t>()) {
        return IntToFloat(Var.as<int2048_t>());
    } else if(Var.is<bool>()) {
        return boolToFloat(Var.as<bool>());
    } else {/*Var.as<std::string>*/
        return StringToFloat(Var.as<std::string>());
    }
}

/// @brief Change ANY variable into int2048_t type
int2048_t AnyToInt(const antlrcpp::Any &Var) {
    if(Var.is<int2048_t>()) {
        return Var.as<int2048_t>();
    } else if(Var.is<double>()) {
        return FloatToInt(Var.as<double>());
    } else if(Var.is<bool>()) {
        return boolToInt(Var.as<bool>());
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
        return FloatToString(Var.as<double>());
    } else {
        return boolToString(Var.as<bool>());
    }
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
        size_t len = size_t(X.is<std::string>() ? Y.as<int2048_t>():
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
    } else {/*os << "WTF is it!!!!!!!!!!!";*/} // UNEXPECTED ERROR
    return os;
}


#endif