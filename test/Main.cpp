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

#include <GenericFactory/GenericFactory_impl.h>

#include <string>

#include "./TestClasses.h"

int main(int, char**) {
  A* obj = genericfactory::GenericFactory<A>::create("B_ofD");
  if (obj) {
    genericfactory::GenericFactory<A>::setProperty("basic", obj, "test");
    genericfactory::GenericFactory<A>::setProperty("test", obj, "test");
    printf("Result: %s\n",
           genericfactory::GenericFactory<A>::getProperty("basic",
           obj).c_str());
  } else {
    printf("WTF\n");
  }
  delete obj;
  obj = genericfactory::GenericFactory<A>::create("A_ofD");
  if (obj) {
    genericfactory::GenericFactory<A>::setProperty("basic", obj, "test");
    genericfactory::GenericFactory<A>::setProperty("test", obj, "test");
    printf("Result: %s\n",
           genericfactory::GenericFactory<A>::getProperty("basic",
           obj).c_str());
  }
  delete obj;
  return 0;
}
