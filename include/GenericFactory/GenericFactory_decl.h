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

#ifndef GENERICFACTORY_GENERICFACTORY_DECL_H_
#define GENERICFACTORY_GENERICFACTORY_DECL_H_

#include <cstdio>
#include <string>
#include <map>
#include <type_traits>
#include "GenericFactory/GenericFactory_fwd.h"

class literal_str_list;

namespace genericfactory {
template<typename T> class Property;
struct BasicCase { };
struct SpecialCase : BasicCase { };
template<typename> struct OkCase { typedef int type; };
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
  template<typename C, typename std::enable_if<
      !std::is_abstract<C>::value
      && std::is_constructible<C>::value, int>::type = 0>
  static void registerClass();
  /// Registers just the properties but doesnt add the class to the
  /// constructables.
  template<typename C, typename std::enable_if<
      std::is_abstract<C>::value
      || !std::is_constructible<C>::value, int>::type = 0>
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

  /// This is the map that holds all registered properties.
  static std::map<std::string, Property<Base>*>& properyMap();

  // TODO(Mi 27. Aug 14:28:59 CEST 2014, bauschp): Think of a way to move this
  // currently needed for cv++.
  template<
        typename C,
        typename OkCase<decltype(&C::registerProperties)>::type = 0>
  static void helpRegisterProperties(SpecialCase s) {
    // call register Properties.
    C::registerProperties();
    printf("%zu after calling %s::registerProperties().\n", properyMap().size(),
      nameOf(C::name).c_str());
  }

  template<typename C>
  static void helpRegisterProperties(BasicCase b) {
    fprintf(stderr, "Couldn't find static void C::registerProperties()\n");
  }

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
}  // namespace genericfactory
#endif  // GENERICFACTORY_GENERICFACTORY_DECL_H_
