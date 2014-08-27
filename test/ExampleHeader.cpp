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

#include "./ExampleHeader.h"

#include <GenericFactory/GenericFactory_impl.h>

#include <string>

#include "./TestClasses.h"

void B::registerProperties() {
  static bool m_lock(true);
  if (!m_lock)
    return;
  m_lock = false;
  genericfactory::GenericFactory<A>::registerProperty(
        "basic",
        &B::basicSet,
        &B::basicGet);
}
void A::registerProperties() {
  static bool m_lock(true);
  if (!m_lock)
    return;
  m_lock = false;
  genericfactory::GenericFactory<A>::registerProperty(
        "test",
        &D<B>::setTest,
        &D<B>::getTest);
}

namespace genericfactory {
template<>
char GenericFactory<A>::registerAllForBase() {
  GenericFactory<A>::registerClass<A>();
  GenericFactory<A>::registerClass<B>();
  GenericFactory<A>::registerClass<D<B> >();
  GenericFactory<A>::registerClass<D<A> >();
  GenericFactory<A>::registerClass<D<D<A> > >();
  return 'y';
}

// Implement string cast helpers here.
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
}  // namespace genericfactory
// String Helper Definitions.
