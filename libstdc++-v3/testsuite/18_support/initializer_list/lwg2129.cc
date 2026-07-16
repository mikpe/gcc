// { dg-do compile { target c++11 } }

// LWG 2129. User specializations of std::initializer_list

#include <initializer_list>

template<class T>
class std::initializer_list<T*> {	// { dg-error "cannot be specialized" }
private:
  void* array;
  decltype(sizeof(0)) len;
};
