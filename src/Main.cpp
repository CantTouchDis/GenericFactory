// Some Copyright
//
//
#include <string>

#include "core/GenericFactory.h"
struct A {
 public:
  static const std::string name;
  virtual void printMe() {
    printf("me A\n");
  }
  virtual void test() = 0;
  virtual ~A() { }
  virtual A* create() const = 0;
};
struct B : public A {
 public:
  static const std::string name;
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
  static const std::string name;
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
  static const std::string name;
  virtual void printMe() override;
  virtual D<T>* create() const;
  virtual void test() { }
  void setTest(float f) {
    printf("set in D works\n");
  }
  float getTest() const { return 0.0f; }
};
template<typename T>
D<T>* D<T>::create() const {
  return new D<T>();
}
template<typename T>
void D<T>::printMe() {
  printf("me %s\n", D<T>::name.c_str());
}
const std::string C::name = "C";
const std::string A::name = "A";
const std::string B::name = "B";
template<typename T>
const std::string D<T>::name = "D_" + T::name;


// String Helper Definitions.
template<>
std::string StringCastHelper<float>::toString(
      const float&) {
  return "";
}

template<>
float StringCastHelper<float>::fromString(
      const std::string&) {
  return 0.0f;
}
// String Helper Definitions.
template<>
std::string StringCastHelper<std::string>::toString(
      const std::string& value) {
  return value;
}

template<>
std::string StringCastHelper<std::string>::fromString(
      const std::string& value) {
  return value;
}

int main(int, char**) {
  // GenericFactory<A>::registerClass<A>();
  // GenericFactory<A>::registerClass<B>();

  GenericFactory<A>::registerClass<D<B> >();
  GenericFactory<A>::registerClass<D<A> >();
  GenericFactory<A>::registerClass<B>();
  GenericFactory<A>::registerProperty("basic", &B::basicSet, &B::basicGet);
  GenericFactory<A>::registerProperty("test", &D<B>::setTest, &D<B>::getTest);
  A* obj = GenericFactory<A>::create("D_B");
  if (obj) {
    GenericFactory<A>::setProperty("basic", obj, "test");
    GenericFactory<A>::setProperty("test", obj, "test");
    printf("Result: %s\n", GenericFactory<A>::getProperty("basic",
          obj).c_str());
  }
  delete obj;
  obj = GenericFactory<A>::create("D_A");
  if (obj) {
    GenericFactory<A>::setProperty("basic", obj, "test");
    GenericFactory<A>::setProperty("test", obj, "test");
    printf("Result: %s\n", GenericFactory<A>::getProperty("basic",
          obj).c_str());
  }
  delete obj;
  return 0;
}
