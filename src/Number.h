#ifndef NUMBER_H_
#define NUMBER_H_

#include <string>
#include <vector>

class Int {
  private:
    bool sign;
    using unit = uint64_t;
    std::vector <unit> vec;
  public:
    friend Int operator +(const Int &X,const Int &Y);
    friend Int operator -(const Int &X,const Int &Y);
    friend Int operator *(const Int &X,const Int &Y);
    friend Int operator /(const Int &X,const Int &Y);
    friend Int operator %(const Int &X,const Int &Y);
    friend bool operator ==(const Int &X,const Int &Y);
    friend bool operator !=(const Int &X,const Int &Y);
    friend bool operator <(const Int &X,const Int &Y);
    friend bool operator <=(const Int &X,const Int &Y);
    friend bool operator >(const Int &X,const Int &Y);
    friend bool operator >=(const Int &X,const Int &Y);
    friend bool operator !(const Int &X);
    friend Int& operator +=(Int &X,const Int &Y);
    friend Int& operator -=(Int &X,const Int &Y);
    friend Int& operator *=(Int &X,const Int &Y);
    friend Int& operator /=(Int &X,const Int &Y);
    friend Int& operator %=(Int &X,const Int &Y);
    const Int& operator =(const Int &Y);
    
    explicit operator bool() const;
    explicit operator double() const;
    explicit operator int() const;
    explicit operator long long() const;
    Int(int x);
    Int(long long x);    
    Int(double x);
    Int();
    ~Int();
};

using int2048_t = int;

#endif