// The MIT License (MIT)
//
// Copyright (c) 2014 CantTouchDis <bauschp@informatik.uni-freiburg.de>
// Copyright (c) 2014 brio1009 <christoph1009@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef TESTCLASSES_H_
#define TESTCLASSES_H_

#include <GenericFactory/LiteralStringList.h>

#include <string>

#include "./ExampleHeader.h"

struct A {
 public:
  static constexpr literal_str_list name = "A";
  static void registerProperties();
  virtual void printMe() {
    printf("me A\n");
  }
  virtual void test() = 0;
  virtual ~A() { }
  virtual A* create() const = 0;
};
struct B : public A {
 public:
  static constexpr literal_str_list name = "B";
  static void registerProperties();
  virtual void printMe() {
    printf("me B\n");
  }
  virtual void test() { }
  virtual B* create() const {
    return new B();
  }

  void basicSet(std::string s) {
    printf("Got called with %s.\n", s.c_str());
  }
  std::string basicGet() const { return "Get Called"; }
};
struct C {
 public:
  static constexpr literal_str_list name = "C";
  virtual void printMe() {
    printf("me C\n");
  }
/*  virtual C* create() const {
    return new C();
  }*/
};

template<typename T>
struct D : public T {
 public:
  static constexpr literal_str_list name = T::name + "_ofD";
  virtual void printMe() override;
  virtual D<T>* create() const;
  virtual void test() { }
  void setTest(float f) {
    printf("set in D works\n");
  }
  float getTest() const { return 0.0f; }
};
template<typename T>
constexpr literal_str_list D<T>::name;

template<typename T>
D<T>* D<T>::create() const {
  return new D<T>();
}

template<typename T>
void D<T>::printMe() {
  printf("me %s\n", convert_to_string(D<T>::name).c_str());
}

#endif  // TESTCLASSES_H_
