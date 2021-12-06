//-*-c++-*-
#ifndef _Filter_h_
#define _Filter_h_

using namespace std;

class Filter {
  short divisor;
  short dim;

public:
  int * data; //made public to access in FilterMain.cpp
  Filter(int _dim);
  void set(int r, int c, int value);

  int getDivisor();
  void setDivisor(int value);

  int getSize();
  void info();
};

#endif
