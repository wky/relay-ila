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

// define input data info (default input layout NCHW)
#define DATA_IN_BATCH "data_in_batch"
#define DATA_IN_BATCH_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

#define DATA_IN_CHANNEL "data_in_channel"
#define DATA_IN_CHANNEL_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

#define DATA_IN_Y "data_in_y"
#define DATA_IN_Y_BITWIDTH RELAY_FUNC_ADDR_IN_BITWIDTH

#define DATA_IN_X "data_in_x"
#define DATA_IN_X_BITWIDTH RELAY_FUNC_ADDR_IN_BITWIDTH


// define input pool_size(y, x)
#define POOL_SIZE_Y_IN "pool_size_y"
#define POOL_SIZE_Y_IN_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

#define POOL_SIZE_X_IN "pool_size_x"
#define POOL_SIZE_X_IN_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

// define input strides(y, x)
#define STRIDES_Y_IN "strides_y_in"
#define STRIDES_Y_IN_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

#define STRIDES_X_IN "strides_x_in"
#define STRIDES_X_IN_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

// define input padding(y, x)
#define PADDING_IN_Y "padding_in_y"
#define PADDING_IN_Y_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

#define PADDING_IN_X "padding_in_x"
#define PADDING_IN_X_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

// define input layout 
#define LAYOUT_IN "layout_in"
#define LAYOUT_IN_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

// define input ceil_mode
#define CEIL_MODE_IN "ceil_mode_in"
#define CEIL_MODE_IN_BITWIDTH RELAY_FUNC_ARG_IN_BITWIDTH

/************ internal states for maxpooling ***************/
#define FLAG_BITWIDTH 1
#define FLAG_DONE 1
#define FLAG_UNDONE 0
#define FLAG_ON 1
#define FLAG_OFF 0

#define MAXPOOLING_START_FLAG "maxpooling_start_flag"
#define MAXPOOLING_START_FLAG_BITWIDTH FLAG_BITWIDTH

// #define MAXPOOLING_X_END_LOOP_FLAG "maxpooling_X_end_loop_flag"
// #define MAXPOOLING_X_END_LOOP_FLAG_BITWIDTH FLAG_BITWIDTH

// #define MAXPOOLING_Y_END_LOOP_FLAG "maxpooling_Y_end_loop_flag"
// #define MAXPOOLING_Y_END_LOOP_FLAG_BITWIDTH FLAG_BITWIDTH

// #define MAXPOOLING_FIND_MAX_FLAG "maxpooling_find_max_flag"
// #define MAXPOOLING_FIND_MAX_FLAG_BITWIDTH FLAG_BITWIDTH

// #define MAXPOOLING_MAX_FOUND_FLAG "maxpooling_max_found_flag"
// #define MAXPOOLING_MAX_FOUND_FLAG_BITWIDTH FLAG_BITWIDTH

// #define MAXPOOLING_VAR_UPDATE_FLAG "maxpooling_var_update_flag"
// #define MAXPOOLING_VAR_UPDATE_FLAG_BITWIDTH FLAG_BITWIDTH

#define MAXPOOLING_STATE "maxpooling_state"
#define MAXPOOLING_STATE_BITWIDTH 3

#define MAXPOOLING_STATE_INC_X 0
#define MAXPOOLING_STATE_INC_Y 1
#define MAXPOOLING_STATE_FIND_MAX 2
#define MAXPOOLING_STATE_FIND_MAX_CHILD 3
#define MAXPOOLING_STATE_WRITE 4
#define MAXPOOLING_STATE_VAR_UPDATE 5
#define MAXPOOLING_STATE_DONE 6


// counter
#define MAXPOOLING_X_LOOP_CNTR "maxpooling_X_loop_cntr"
#define MAXPOOLING_X_LOOP_CNTR_BITWIDTH  RELAY_FUNC_ADDR_IN_BITWIDTH

#define MAXPOOLING_Y_LOOP_CNTR "maxpooling_Y_loop_cntr"
#define MAXPOOLING_Y_LOOP_CNTR_BITWIDTH  RELAY_FUNC_ADDR_IN_BITWIDTH

#define MAXPOOLING_DATA_OUT_HEIGHT "maxpooling_data_out_height"
#define MAXPOOLING_DATA_OUT_HEIGHT_BITWIDTH RELAY_FUNC_ADDR_IN_BITWIDTH

#define MAXPOOLING_DATA_OUT_WIDTH "maxpooling_data_out_width"
#define MAXPOOLING_DATA_OUT_WIDTH_BITWIDTH RELAY_FUNC_ADDR_IN_BITWIDTH

// child states for find max
// find_max_cntr bitwidth should be twice larger than arg width
#define MAXPOOLING_FIND_MAX_CNTR "maxpooling_find_max_cntr"
#define MAXPOOLING_FIND_MAX_CNTR_BITWIDTH 2*RELAY_FUNC_ARG_IN_BITWIDTH

#define MAXPOOLING_FIND_MAX_RESULT "maxpooling_find_max_result"
#define MAXPOOLING_FIND_MAX_RESULT_BITWIDTH RELAY_FUNC_DATA_IN_BITWIDTH

#define MAXPOOLING_FIND_MAX_CHILD_FLAG "maxpooling_find_max_child_flag"
#define MAXPOOLING_FIND_MAX_CHILD_FLAG_BITWIDTH FLAG_BITWIDTH


}


#endif // RELAY_MAXPOOLING_H__