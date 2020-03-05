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

// File: relay_tensor_store.cc

#include <relay/relay_top.h>
#include <ilang/util/log.h>

namespace ilang {

void DefineTensorStore(Ila& m) {
  auto instr = m.NewInstr(F_TENSOR_STORE);

  auto func_run = (m.input(RELAY_FUNC_RUN_IN) == RELAY_FUNC_RUN_ON);
  auto func_id_match = (m.input(RELAY_FUNC_ID_IN) == F_TENSOR_STORE_ID);

  instr.SetDecode(func_run & func_id_match);

  auto tensor = m.state(RELAY_TENSOR_MEM);
  auto addr = m.input(DATA_IN_X);
  auto data = m.input(RELAY_DATA_IN);

  instr.SetUpdate(tensor, Store(tensor, addr, data));

}

}