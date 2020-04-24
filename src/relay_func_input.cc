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

// File: relay_func_input.cc

#include <relay/relay_top.h>

#include <ilang/util/log.h>

namespace ilang {

void DefineFuncInput(Ila& m) {

  /******** input of function maxpooling **********/
  // input of matrix data
  m.NewBvInput(DATA_IN_BATCH, DATA_IN_BATCH_BITWIDTH);
  m.NewBvInput(DATA_IN_CHANNEL, DATA_IN_CHANNEL_BITWIDTH);
  m.NewBvInput(DATA_IN_Y, DATA_IN_Y_BITWIDTH);
  m.NewBvInput(DATA_IN_X, DATA_IN_X_BITWIDTH);

  // input of the pool_size
  m.NewBvInput(POOL_SIZE_Y_IN, POOL_SIZE_Y_IN_BITWIDTH);
  m.NewBvInput(POOL_SIZE_X_IN, POOL_SIZE_X_IN_BITWIDTH);
  // input of strides
  m.NewBvInput(STRIDES_Y_IN, STRIDES_Y_IN_BITWIDTH);
  m.NewBvInput(STRIDES_X_IN, STRIDES_X_IN_BITWIDTH);
  // input of padding 
  m.NewBvInput(PADDING_IN_Y, PADDING_IN_Y_BITWIDTH);
  m.NewBvInput(PADDING_IN_X, PADDING_IN_X_BITWIDTH);
  // input of layout
  m.NewBvInput(LAYOUT_IN, LAYOUT_IN_BITWIDTH);
  // input of ceiling mode
  m.NewBvInput(CEIL_MODE_IN, CEIL_MODE_IN_BITWIDTH);


  /**** Relay LSTM input ****/
  m.NewBvInput(RELAY_LSTM_IN_SIZE, RELAY_VECTOR_SIZE_BW);
  m.NewBvInput(RELAY_LSTM_OUT_SIZE, RELAY_VECTOR_SIZE_BW);

  m.NewBvInput(RELAY_LSTM_INPUT_ADDR, RELAY_LSTM_ADDR_BW);
  m.NewBvInput(RELAY_LSTM_CELL_ADDR, RELAY_LSTM_ADDR_BW);
  m.NewBvInput(RELAY_LSTM_NEXT_CELL_ADDR, RELAY_LSTM_ADDR_BW);
  m.NewBvInput(RELAY_LSTM_HIDDEN_ADDR, RELAY_LSTM_ADDR_BW);
  m.NewBvInput(RELAY_LSTM_NEXT_HIDDEN_ADDR, RELAY_LSTM_ADDR_BW);

  m.NewBvInput(RELAY_LSTM_I2H_WEIGHT_ADDR, RELAY_LSTM_ADDR_BW);
  m.NewBvInput(RELAY_LSTM_H2H_WEIGHT_ADDR, RELAY_LSTM_ADDR_BW);

  m.NewBvInput(RELAY_LSTM_I2H_BIAS_ADDR, RELAY_LSTM_ADDR_BW);
  m.NewBvInput(RELAY_LSTM_H2H_BIAS_ADDR, RELAY_LSTM_ADDR_BW);

  m.NewBvInput(RELAY_LSTM_TEMP_VECTOR0_ADDR, RELAY_LSTM_ADDR_BW);
  m.NewBvInput(RELAY_LSTM_TEMP_VECTOR1_ADDR, RELAY_LSTM_ADDR_BW);
  m.NewBvInput(RELAY_LSTM_TEMP_VECTOR2_ADDR, RELAY_LSTM_ADDR_BW);

  
}

}; // namespace ilang
