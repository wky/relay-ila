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

// File: relay_func_call.h

#ifndef RELAY_FUNC_CALL_H__
#define RELAY_FUNC_CALL_H__

#include <relay/relay_top_config.h>

namespace ilang {

// define function ID here
#define F_MAXPOOING_2D "func_maxpooling_2d"
#define F_MAXPOOLING_2D_ID 1
#define F_MAXPOOLING_2D_ID_BITWIDTH RELAY_FUNC_ID_IN_BITWIDTH

#define F_TENSOR_STORE "func_tensor_store"
#define F_TENSOR_STORE_ID 2
#define F_TENSOR_STORE_ID_BITWIDTH RELAY_FUNC_ID_IN_BITWIDTH


#define F_LSTM "func_lstm"
#define F_LSTM_ID 3

// #define F_VECTOR_ADD "func_vector_add"
// #define F_VECTOR_ADD_ID 4

// #define F_VECTOR_MULTIPLY "func_vector_multiply"
// #define F_VECTOR_MULTIPLY_ID 5


// #define F_VECTOR_SIGMOID "func_vector_sigmoid"
// #define F_VECTOR_SIGMOID_ID 6

// #define F_VECTOR_TANH "func_vector_tanh"
// #define F_VECTOR_TANH_ID 7

// #define F_NN_DENSE "func_nn_dense"
// #define F_NN_DENSE_ID 8


};

#endif // RELAY_FUNCTION_CALL_H__