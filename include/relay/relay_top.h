// =============================================================================
// MIT License
//
// Copyright (c) 2020 Princeton University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// =============================================================================

// File: relay_top.h

#ifndef RELAY_TOP_H__
#define RELAY_TOP_H__

#include <ilang/ilang++.h>
#include <relay/relay_func_config.h>
#include <relay/relay_top_config.h>
#include <string>

namespace ilang {

Ila GetRelayIla(const std::string& model_name = "relay");

// define top input
void DefineTopInput(Ila& m);

// define function input
void DefineFuncInput(Ila& m);

// define architectural states
void DefineArchState(Ila& m);

// define internal states
void DefineInternalState(Ila& m);

// define Relay instructions
void DefineTensorStore(Ila& m);
void DefineMaxpooling2D(Ila& m);

// define Relay operations

void DefineVectorAdd(Ila& m);
void DefineVectorMultiply(Ila& m);
void DefineVectorSigmoid(Ila& m);
void DefineVectorTanh(Ila& m);

void DefineNNDense(Ila& m);

// define LSTM instructions
void DefineLSTM(Ila& m);

};

#endif // RELAY_TOP_H__
