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

#ifndef EXAMPLEHEADER_H_
#define EXAMPLEHEADER_H_

#include <GenericFactory/GenericFactory_decl.h>
#include <GenericFactory/Property.h>

#include <string>


// Forward declate all needed classes here.
struct A;
namespace genericfactory {

// Define Bases for GenericFactory.
template<>
char GenericFactory<A>::registerAllForBase();

// Define StringCastHelper specializations here.
template<>
std::string StringCastHelper<float>::toString(
      const float&);
template<>
float StringCastHelper<float>::fromString(
      const std::string&);
template<>
std::string StringCastHelper<std::string>::toString(
      const std::string& value);

template<>
std::string StringCastHelper<std::string>::fromString(
      const std::string& value);
}  // namespace genericfactory
#endif  // EXAMPLEHEADER_H_
