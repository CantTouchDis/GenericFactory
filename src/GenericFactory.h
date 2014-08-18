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

#include <typeinfo>
#include <cstdio>
#include <string>
#include <type_traits>
#include <map>

#ifndef DISABLELITERALSTRING
#include "./LiteralStringList.h"
#endif  // DISABLELITERALSTRING

#include "./Property.h"

namespace genfactory {
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
  /// Static field to import everything at compiletime.
  static const char helpInit;

  /// Registeres all classes for base (has to be user defined).
  static char registerAllForBase();
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
  static typename std::enable_if<
      !std::is_abstract<C>::value
      && std::is_default_constructible<C>::value, void>::type
        registerClass();
  /// Registers just the properties but doesnt add the class to the
  /// constructables.
  template<typename C>
  static typename std::enable_if<
      std::is_abstract<C>::value
      || !std::is_default_constructible<C>::value, void>::type
    registerClass();
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

  /// This is the map that holds all registered properties.
  static std::map<std::string, Property<Base>*>& properyMap();

  template<
        typename C,
        typename OkCase<decltype(&C::registerProperties)>::type = 0>
  static void helpRegisterProperties(SpecialCase s);

  template<typename C>
  static void helpRegisterProperties(BasicCase b);
  template<
        typename C,
        typename OkCase<decltype(C::name)>::type = 0,
        typename OkCase<decltype(&C::create)>::type = 0>
  static void helpRegisterClass(SpecialCase s);
  // Dont register if u cant find the name.
  template<typename C>
  static void helpRegisterClass(BasicCase b);

  // Helper to get the name in a uniform way.
  static std::string nameOf(const char* const name);
  static std::string nameOf(const std::string& name);
#ifndef DISABLELITERALSTRING
  static std::string nameOf(const literal_str_list& name);
#endif  // DISABLELITERALSTRING
};

// #########################DEFINITIONS#########################################
// Definitions to auto initialize the reflection maps.
template<typename Base>
const char GenericFactory<Base>::helpInit =
      GenericFactory<Base>::registerAllForBase();
// Used to produce compiler errors when not defined for used base.
template<typename Base>
char GenericFactory<Base>::registerAllForBase() {
  static_assert(sizeof(Base) != sizeof(Base), "Specialize a"
      "this method to register all of your used subtypes ob Base!");
  return 'n';
}

// Definitions to access the static maps.
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
// Definitions to register the classes and properties.
template<typename Base>
template<
      typename C,
      typename OkCase<decltype(&C::registerProperties)>::type>
void GenericFactory<Base>::helpRegisterProperties(SpecialCase) {
  // call register Properties.
  C::registerProperties();
  printf("%zu after calling %s::registerProperties().\n",
      properyMap().size(),
      nameOf(C::name).c_str());
}

template<typename Base>
template<typename C>
void GenericFactory<Base>::helpRegisterProperties(BasicCase) {
  fprintf(stderr, "Couldn't find static void C::registerProperties()\n");
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
template<typename C>
typename std::enable_if<
      std::is_abstract<C>::value
      || !std::is_default_constructible<C>::value, void>::type
        GenericFactory<Base>::registerClass() {
  GenericFactory<Base>::helpRegisterProperties<C>(SpecialCase());
}
template<typename Base>
template<typename C>
typename std::enable_if<
      !std::is_abstract<C>::value
      && std::is_default_constructible<C>::value, void>::type
        GenericFactory<Base>::registerClass() {
  // perform some checks so we dont run into a mess later on.
  static_assert(std::is_base_of<Base, C>::value,
        "C dosn`t have base Base\n");
  helpRegisterClass<C>(SpecialCase());
  GenericFactory<Base>::helpRegisterProperties<C>(SpecialCase());
}

template<typename Base>
template<
      typename C,
      typename OkCase<decltype(C::name)>::type,
      typename OkCase<decltype(&C::create)>::type>
void GenericFactory<Base>::helpRegisterClass(SpecialCase) {
  std::string name = nameOf(C::name);
  // This is used to register them automaticly.
  if (sizeof(helpInit) != sizeof(helpInit) && helpInit)
    return;
  printf("REGISTERING %s.\n", name.c_str());
  if (reflectionMap().find(name) != reflectionMap().end()) {
    perror("There already exists a class with this name\n");
    delete reflectionMap()[name];
  }
  // only default constructable C will land here.
  // just construct one.
  reflectionMap()[name] = new C();
}

template<typename Base>
template<typename C>
void GenericFactory<Base>::helpRegisterClass(BasicCase) {
  fprintf(stderr, "(%s) is missing the static field with the reflection "
      "name or Base* create() const member. Make sure you add them!!.\n",
      typeid(C).name());
}

// Definitions to access a property. (get/set).
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
  fprintf(stderr, "There is no property named %s\n", propName.c_str());
}

template<typename Base>
std::string GenericFactory<Base>::getProperty(
        const std::string& propName,
        Base* const objPtr) {
  Property<Base>* prop = properyMap()[propName];
  if (prop)
    return prop->get(objPtr);
  fprintf(stderr, "There is no property named %s\n", propName.c_str());
  return "ERROR";
}

// Definition to create a object with given name.
template<typename Base>
Base* GenericFactory<Base>::create(const std::string& name) {
  // Thanks to registerClass only constructable objects will be called here.
  return creationHelper(name, reflectionMap(), SpecialCase());
}

template<typename Base>
std::string GenericFactory<Base>::nameOf(const char* const name) {
  return std::string(name);
}

template<typename Base>
std::string GenericFactory<Base>::nameOf(const std::string& name) {
  return name;
}
#ifndef DISABLELITERALSTRING
template<typename Base>
std::string GenericFactory<Base>::nameOf(const literal_str_list& name) {
  return convert_to_string(name);
}
#endif  // DISABLELITERALSTRING
}  // namespace genfactory
#endif  // GENERICFACTORY_H_
