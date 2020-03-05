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

// File: relay_top_config.h

#ifndef RELAY_TOP_CONFIG_H__
#define RELAY_TOP_CONFIG_H__

namespace ilang {

// define top level input bitwidth
#define RELAY_FUNC_ADDR_IN_BITWIDTH 32
#define RELAY_FUNC_ARG_IN_BITWIDTH 8
#define RELAY_FUNC_DATA_IN_BITWIDTH 8
#define RELAY_FUNC_ID_IN_BITWIDTH 8

// define relay top function call run flag
#define RELAY_FUNC_RUN_IN "relay_func_run_in"
#define RELAY_FUNC_RUN_IN_BITWIDTH 1
#define RELAY_FUNC_RUN_ON 1
#define RELAY_FUNC_RUN_OFF 0

// define relay top level data in
#define RELAY_DATA_IN "relay_data_in"
#define RELAY_DATA_IN_BITWIDTH 8

// define the function call ID to represent the functions in Relay/TVM
#define RELAY_FUNC_ID_IN "relay_func_id"
#define RELAY_FUNC_ID_IN_BITWIDTH 8

// define the tensor memory here
#define RELAY_TENSOR_MEM "relay_tensor_mem"

}

#endif // RELAY_TOP_CONFIG_H__