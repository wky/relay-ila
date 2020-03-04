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

// File: relay_internal_states.cc

// this file include the internal states of the relay model.

#include <relay/relay_top.h>
#include <ilang/util/log.h>

namespace ilang {

void DefineInternalState(Ila& m) {
  // internal states for relay maxpooling 2d function

  // flag states for loop instructions
  m.NewBvState(MAXPOOLING_START_FLAG, MAXPOOLING_START_FLAG_BITWIDTH);
  m.NewBvState(MAXPOOLING_X_LOOP_FLAG, MAXPOOLING_X_LOOP_FLAG_BITWIDTH);
  m.NewBvState(MAXPOOLING_Y_LOOP_FLAG, MAXPOOLING_Y_LOOP_FLAG_BITWIDTH);
  m.NewBvState(MAXPOOLING_FIND_MAX_FLAG, MAXPOOLING_FIND_MAX_FLAG_BITWIDTH);

  // cntr states for loop instructions
  m.NewBvState(MAXPOOLING_X_LOOP_CNTR, MAXPOOLING_X_LOOP_CNTR_BITWIDTH);
  m.NewBvState(MAXPOOLING_Y_LOOP_CNTR, MAXPOOLING_Y_LOOP_CNTR_BITWIDTH);

  m.NewBvState(MAXPOOLING_DATA_OUT_HEIGHT, MAXPOOLING_DATA_OUT_HEIGHT_BITWIDTH);
  m.NewBvState(MAXPOOLING_DATA_OUT_WIDTH, MAXPOOLING_DATA_OUT_WIDTH_BITWIDTH);
  
}

} // namespace ilang