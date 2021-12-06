#include "Filter.h"
#include <iostream>

Filter::Filter(int _dim)
{
  divisor = 1;
  dim = _dim;
  data = new int[dim * dim];
}

void Filter::set(int r, int c, int value)
{
  data[ r * dim + c ] = value;
}

int Filter::getDivisor()
{
  return divisor;
}

void Filter::setDivisor(int value)
{
  divisor = value;
}

int Filter::getSize()
{
  return dim;
}

void Filter::info()
{
  cout << "Filter is.." << endl;
  for (int col = 0; col < dim; col++) {
    for (int row = 0; row < dim; row++) {
      int v = data[row*dim+col];
      cout << v << " ";
    }
    cout << endl;
  }
}
