// Some copyright

#ifndef CORE_PROPERTY_H_
#define CORE_PROPERTY_H_

#include <cstdio>
#include <string>

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
      set(me, value);
      return;
    }
    perror("Cant call this prop on this Object!\n");
  }
  virtual std::string get(const Base* const obj) const override {
    const OwnerClass* const me = dynamic_cast<const OwnerClass* const>(obj);
    if (me)
      return get(me);
    perror("Cant call this prop on this Object!\n");
    return "ERRORINPROP";
  }

  /// Returns the value of the property as string.
  std::string get(const OwnerClass* const objPtr) const {
    return StringCastHelper<ValueType>::toString(getValue(objPtr));
  }

  /// Sets the value from string.
  void set(OwnerClass* const objPtr, const std::string& value) const {
    setValue(objPtr, StringCastHelper<ValueType>::fromString(value));
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
#endif  // CORE_PROPERTY_H_
