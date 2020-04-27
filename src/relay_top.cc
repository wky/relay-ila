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

// File: relay_top.cc

#include <relay/relay_top.h>

#include <ilang/util/log.h>

namespace ilang {

Ila GetRelayIla(const std::string& model_name) {
  auto m = Ila(model_name);

  // TODO
  // define top input
  DefineTopInput(m);

  // define function input
  DefineFuncInput(m);

  // define architectural states
  DefineArchState(m);

  // define internal states
  DefineInternalState(m);

  auto is_func_call = (m.input(RELAY_FUNC_RUN_IN) == RELAY_FUNC_RUN_ON);
  auto is_valid_func = (m.input(RELAY_FUNC_ID_IN) > 0);
  m.SetValid(is_func_call & is_valid_func);

  // define Relay instructions

  auto vector_child = m.NewChild(RELAY_VECTOR_OP_CHILD);
  vector_child.SetValid(is_func_call & is_valid_func);
  DefineVectorAdd(m);
  DefineVectorMultiply(m);
  DefineVectorSigmoid(m);
  DefineVectorTanh(m);

  auto nn_child = m.NewChild(RELAY_NN_CHILD);
  nn_child.SetValid(is_func_call & is_valid_func);
  DefineNNDense(m);

  DefineLSTM(m);
  
  
  DefineTensorStore(m);
  DefineMaxpooling2D(m);


  
  return m;
}

}; // namespace ilang
