# Generic Factory

This is a small project that enables you to use pseudo-reflections on the objects of classes registered by the factory.

## How to register a class
1.  Create any hirachy of classes.
2.  Add `static (const char* | std::string | literal_string_list) name;` to the classes. The last one is used to concat string literals while compiling. (C++11) (see How to add this to your project)
3.  Make sure to add a `virtual Base* create() const` to all createable classes.
4.  Register your classes. Choose i. XOR ii.
  1. specialize `char GenericFactory<Base>::registerAllForBase();` (return anything) with your Base class and call  `GenericFactory<Base>::registerClass<SubClass>();` for any SubClass you want to register within.
  2. call `GenericFactory<Base>::registerClass<SubClass>();` for any SubClass you want to register

## How to create a object of a registered class
call `Base* name = GenericFactory<Base>::create("className");` if className is registered you get a object of the corresponding class, otherwise nullptr.

## How to add register properties
1.  provide a function `static void registerProperties()` in registered classes to add them automaticly.
2.  call `GenericFactory<Base>::registerProperty("name", setPtr, getPtr)` for every get-set pair you want to register.

SubClasses will be able to call properties of any class in the hirachy above them.

> NOTE: if you add this in a class all subclasses will call this as well. You might want to add a once lock (see Example).

## How to add this to your project
Just copy the header files to your projects include path. If your compiler can't use constexp you are not able to use `literal_string_list`. Provide `DISABLELITERALSTRING` as compilerflag and you should be good to go. Sadly constructs that combine names as their name have to be specialized individualy. See below.
```
// class Cont.h
template<typename T>
class Cont {
  static constexpr literal_str_list name = T::name + "_Cont";
};
template<typename T>
constexpr literal_str_list Cont<T>::name;
```

## I`m getting a compiler error when I register a property
Yes `Specialize a StringCastHelper for your ValueType!` is a intended error. By default it cannot convert every
string to it's corresponding type and back.
Specialize the template like this:
```
std::string StringCastHelper<float>::toString(
      const float& value) {
  return std::to_string(value);
}
template<>
float StringCastHelper<float>::fromString(
      const std::string& value) {
  return std::stod(value);
}
```

##### Examples:

```
// A.h
class A {
 public:
  // Every class that is createable should have a name
  static const char* name;
  // If you want to call getter and setter on a object of A
  // use registerProperties() to register them automaticly.
  // you can add them later on as well.
  static void registerProperties();
  
  void setSomeValue(float f);
  float getSomeValue() const;
  ...
};

const char* A::name = "A";
void A::registerProperties() {
  static bool lock(true);
  if (!lock)
    return;
  GenericFactory<A>::registerProperty("someName",
        &A::setSomeValue,   // Pointer to getter
        &B::getSomeValue);  // Pointer to setter
  // NOTE: They need the correct signature, to recognice the type.
  // you will get a compiler error otherwise.
}

// Choice 1 to register A and all its subtypes.
template<>
char GenericFactory<A>::registerAllForBase() {
  GenericFactory<A>::registerClass<A>();
  // Add all the subtypes here. Cant find them automaticly (yet?).
  return 'y'; // Return any char but 1!!
}

template<>
std::string StringCastHelper<float>::toString(
      const float& value) {
  return std::to_string(value);
}
template<>
float StringCastHelper<float>::fromString(
      const std::string& value) {
  return std::stod(value);
}

int main(int, char**) {
  // Choice 2 register in a codeblock.
  GenericFactory<A>::registerClass<A>();
  
  // Here is how to create a object of A
  A* obj = GenericFactory<A>::create("A");
  if (obj) {
    // calles setSomeValue(5.5f).
    GenericFactory<A>::setProperty("someName", obj, "5.5");
  }
  delete obj;
  return 0;
}
```
