// The MIT License (MIT)
//
// Copyright (c) 2014 CantTouchDis <bauschp@informatik.uni-freiburg.de>
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

#ifndef GENERICFACTORY_H_
#define GENERICFACTORY_H_

#include <cstdio>
#include <string>
#include <type_traits>
#include <map>

#include "./Property.h"

// These are helper structs to be able to distinquish between existing and non
// existing members. Check out:
// stackoverflow.com/questions/13786888/check-if-member-exists-using-enable-if
struct BasicCase { };
struct SpecialCase : BasicCase { };
template<typename> struct OkCase { typedef int type; };

/// There two are helper functions to check wheter or not the base has a create
/// method. This is important, cause all stored pointers are of this type and
/// create has to exist in order to create objects of subclasses.
template<typename Base, typename OkCase<decltype(&Base::create)>::type = 0>
Base* creationHelper(
      const std::string& name,
      std::map<std::string, Base*> reflectionMap,
      SpecialCase) {
  Base* creator = reflectionMap[name];
  if (!creator)
    return nullptr;
  return creator->create();
}
/// See above. this is the case if Base doesnt have create().
template<typename Base>
Base* creationHelper(
      const std::string&,
      std::map<std::string, Base*>,
      BasicCase) {
  return nullptr;
}

/// This helper class is used to delete the pointers of the static
/// map when the programm terminates. (Composite)
template<typename Key, typename Value>
class HelperPointerMap {
 public:
  ~HelperPointerMap() {
    auto it = m_Map.begin();
    auto endIt = m_Map.end();
    for (; it != endIt; ++it) {
      delete it->second;
    }
  }
  std::map<Key, Value*>& map() {
    return m_Map;
  }
 private:
  std::map<Key, Value*> m_Map;
};

/// This class provides a simple interface to create and modify classes that
/// provide the nessesary components to be modified. Below this definition
/// there will be MACROS to help creating such classes.
template<typename Base>
class GenericFactory {
 public:
  /// Use this method to create a object of class name.
  static Base* create(const std::string& name);

  /// Calls the method with given name on the object to set a property value.
  static void setProperty(
        const std::string& propName,
        Base* const objPtr,
        const std::string& value);

  /// Calls the method with given name on the object to get a property value.
  static std::string getProperty(
        const std::string& propName,
        Base* const objPtr);

  /// Registers the class in GenericFactory<Base> of type C
  /// There will be errors if C is not of type Base,
  /// If C is not DefaultConstructable,
  /// And if C has a pure member. (abstract).
  template<typename C>
  static void registerClass();

  /// Registers a method of C to be called by the name methodName.
  /// Use callMethod(...) to call the method on a object.
  template<typename C, typename Type>
  static void registerProperty(
        const std::string& methodName,
        void (C::*setPtr)(Type),
        Type (C::*getPtr)() const);

 private:
  /// We dont want anyone to create this.
  GenericFactory();
  /// This is the map that holds all the registered classes.
  static std::map<std::string, Base*>& reflectionMap();

  static std::map<std::string, Property<Base>*>& properyMap();

  template<
        typename C,
        typename OkCase<decltype(C::name)>::type = 0,
        typename OkCase<decltype(&C::create)>::type = 0>
  static void helpRegister(SpecialCase s);
  // Dont register if u cant find the name.
  template<typename C>
  static void helpRegister(BasicCase b);

  template<typename C>
  static void registerClassiWithName(const char* const name);
  template<typename C>
  static void registerClassiWithName(const std::string& name);
};

// #########################DEFINITIONS#########################################

template<typename Base>
std::map<std::string, Base*>& GenericFactory<Base>::reflectionMap() {
  static HelperPointerMap<std::string, Base> m_ReflMap;
  return m_ReflMap.map();
}

template<typename Base>
std::map<std::string, Property<Base>*>& GenericFactory<Base>::properyMap() {
  static HelperPointerMap<std::string, Property<Base> > m_PropMap;
  return m_PropMap.map();
}

template<typename Base>
template<typename C, typename Type>
void GenericFactory<Base>::registerProperty(
      const std::string& methodName,
      void (C::*setPtr)(Type),
      Type (C::*getPtr)() const) {
  if (properyMap().find(methodName) != properyMap().end()) {
    perror("There already exists a property with this name\n");
    delete properyMap()[methodName];
  }
  properyMap()[methodName] = new TypeProperty<Base, C, Type>(setPtr, getPtr);
}

template<typename Base>
void GenericFactory<Base>::setProperty(
        const std::string& propName,
        Base* const objPtr,
        const std::string& value) {
  Property<Base>* prop = properyMap()[propName];
  if (prop) {
    prop->set(objPtr, value);
    return;
  }
  perror("There is no propery with this name.");
}

template<typename Base>
std::string GenericFactory<Base>::getProperty(
        const std::string& propName,
        Base* const objPtr) {
  Property<Base>* prop = properyMap()[propName];
  if (prop)
    return prop->get(objPtr);
  perror("There is no propery with this name.");
  return "ERROR";
}
template<typename Base>
template<typename C>
void GenericFactory<Base>::registerClassiWithName(
      const std::string& name) {
  printf("REGISTERING %s.\n", C::name.c_str());
  if (reflectionMap().find(name) != reflectionMap().end()) {
    perror("There already exists a class with this name\n");
    delete reflectionMap()[name];
  }
  reflectionMap()[name] = new C();
}

template<typename Base>
template<typename C>
void GenericFactory<Base>::registerClassiWithName(
      const char* const name) {
  registerClassiWithName<C>(std::string(name));
}
template<typename Base>
template<
      typename C,
      typename OkCase<decltype(C::name)>::type,
      typename OkCase<decltype(&C::create)>::type>
void GenericFactory<Base>::helpRegister(SpecialCase) {
  registerClassiWithName<C>(C::name);
}

template<typename Base>
template<typename C>
void GenericFactory<Base>::helpRegister(BasicCase) {
  perror("Cant find name of template argument to register,"
      " or C::create is missing. Make sure ur class has the method.\n");
}

template<typename Base>
Base* GenericFactory<Base>::create(const std::string& name) {
  // Thanks to registerClass only constructable objects will be called here.
  return creationHelper(name, reflectionMap(), SpecialCase());
}

template<typename Base>
template<typename C>
void GenericFactory<Base>::registerClass() {
  // perform some checks so we dont run into a mess later on.
  static_assert(std::is_base_of<Base, C>::value,
        "C dosn`t have base Base\n");
  static_assert(!std::is_abstract<C>::value,
        "C is abstract!\n");
  static_assert(std::is_default_constructible<C>::value,
        "C is not default constructable!\n");
  helpRegister<C>(SpecialCase());
}
#endif  // GENERICFACTORY_H_
