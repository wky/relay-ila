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

// File: relay_maxpooling.h

#ifndef RELAY_MAXPOOLING_H__
#define RELAY_MAXPOOLING_H__

namespace ilang {

// set maxpooling function arguments as ILA input for now
// define function input names
#define DATA_ADDR_IN "data_addr_in"
#define DATA_ADDR_IN_BITWIDTH   RELAY_FUNC_ADDR_IN_BITWIDTH

// define input pool_size(y, x)
#define POOL_SIZE_Y_IN "pool_size_y"
#define POOL_SIZE_Y_IN_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

#define POOL_SIZE_X_IN "pool_size_x"
#define POOL_SIZE_X_IN_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

// define input strides(y, x)
#define STRIDES_Y_IN "strides_y_in"
#define STRIDES_Y_IN_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

#define STRIDES_X_IN "strides_x_in"
#define STRIDES_Y_IN_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

// define input padding(y, x)
#define PADDING_IN_Y "padding_in_y"
#define PADDING_IN_Y_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

#define PADDING_IN_X "padding_in_x"
#define PADDING_IN_X_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

// define input layout 
#define 

// define input pool
}


#endif // RELAY_MAXPOOLING_H__