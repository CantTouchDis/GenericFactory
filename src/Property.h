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

#ifndef PROPERTY_H_
#define PROPERTY_H_

#include <cstdio>
#include <string>

namespace genfactory {
/// A property consists of getter and setter.
template<typename Base>
class Property {
 public:
  virtual void set(Base* const obj, const std::string& value) const = 0;
  virtual std::string get(const Base* const obj) const = 0;
  virtual ~Property() { }
};

/// Helper struct to partially specialize.
template<typename ValueType>
struct StringCastHelper {
 public:
  /// Returns the value of the property as string.
  static std::string toString(const ValueType& value);

  /// Returns the value from string.
  static ValueType fromString(const std::string& value);

 private:
  /// Hide the Constructor.
  StringCastHelper();
};

/// Proeprty specialized for one ValueType.
template<typename Base, typename OwnerClass, typename ValueType>
class TypeProperty : public Property<Base> {
 public:
  /// Constructor setting the getter and setter ptr.
  TypeProperty(void (OwnerClass::*pSetter)(ValueType value),
               ValueType (OwnerClass::*pGetter)() const)
    : setterPtr(pSetter),
      getterPtr(pGetter) { }

  virtual void set(Base* const obj, const std::string& value) const override {
    OwnerClass* const me = dynamic_cast<OwnerClass* const>(obj);
    if (me) {
      setValue(me, StringCastHelper<ValueType>::fromString(value));
      return;
    }
    perror("Cant call this prop on this Object!\n");
  }
  virtual std::string get(const Base* const obj) const override {
    const OwnerClass* const me = dynamic_cast<const OwnerClass* const>(obj);
    if (me)
      return StringCastHelper<ValueType>::toString(getValue(me));
    perror("Cant call this prop on this Object!\n");
    return "ERRORINPROP";
  }

 private:
  /// Getter for the property.
  ValueType getValue(const OwnerClass* const objPtr) const {
    return (objPtr->*getterPtr)();
  }

  /// Setter for the property.
  void setValue(OwnerClass* const objPtr, ValueType value) const {
    (objPtr->*setterPtr)(value);
  }
  /// Saves pointer to the setter (offset to class pointer).
  void (OwnerClass::*setterPtr)(ValueType value);
  /// Saves pointer to the getter (offset to class pointer).
  ValueType (OwnerClass::*getterPtr)() const;
};

// #########################DEFINITIONS#########################################

template<typename ValueType>
std::string StringCastHelper<ValueType>::toString(const ValueType&) {
  // this sizeof trick is a lottle bit dirty. But false will allways cause this
  // exception.
  static_assert(sizeof(ValueType) != sizeof(ValueType), "Specialize a"
      "StringCastHelper for your ValueType!");
}

template<typename ValueType>
ValueType StringCastHelper<ValueType>::fromString(const std::string&) {
  // this sizeof trick is a lottle bit dirty. But false will allways cause this
  // exception.
  static_assert(sizeof(ValueType) != sizeof(ValueType), "Specialize a"
      "StringCastHelper for your ValueType!");
}
}  // namespace genfactory
#endif  // PROPERTY_H_
