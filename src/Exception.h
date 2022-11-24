#define EXCEPTION_H // Don't use it
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>


/// @brief Record All kinds of Exceptions
enum ExceptionType {
    UNDEFINED_VAR,
    UNIMPLEMENTED,
    INVALID_VARNAME,
    INVALID_FUNC_CALL,
    UNKNOWN_TYPE,
    UNKNOWN_AUG_ASSIGN_OP,
    UNKNOWN_ERROR,
    INVALID_ADDITION
};

class Exception {
  private:
    std::string message;

  public:
    /// @brief Deal with all kinds of Exceptions.
    /// @param type The type of the Exception.
    /// @param arg  The additional string of Exception. 
    Exception(ExceptionType type,const std::string &arg = "") {
        switch(type) {
            case UNDEFINED_VAR:
                message = "Undefined Variable: " + arg;
                break;
            case UNIMPLEMENTED:
                message = "Sorry, this hasn't been implemented yet.ORZ.";
                break;
            case INVALID_VARNAME:
                message = "Invalid variable name: " + arg;
                break;
            case INVALID_FUNC_CALL:
                message = "Invalid function call: " + arg;
                break;
            case UNKNOWN_TYPE:
                message = "This isn't a basic type!";
                break;
            case UNKNOWN_AUG_ASSIGN_OP:
                message = "This is an unknown aug-assign operator!.";
            default:
                message = "Unknown Error!";
        }
        
        /*
        if (type == UNIMPLEMENTED) message = "Sorry, Apple Pie do not implement this.";
        else if (type == UNDEFINED) message = "Undefined Variable: " + arg;
        else if (type == INVALID_FUNC_CALL) message = "Invalid function call: " + arg;
        */
    }   

    // NOTHING, which might be completed soon 
    Exception() {

    }
    const std::string& what() const{return message;}

};

#endif //EXCEPTION_H