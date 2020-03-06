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

// File: relay_maxpooling_2d.cc
// tvm.relay.nn.max_pool2d(data, pool_size=(1, 1), strides=(1, 1), padding=(0, 0), layout='NCHW', ceil_mode=False)

// This file contains the model of maxpooling 2d instruction of Relay IR

#include <relay/relay_top.h>

#include <ilang/util/log.h>

namespace ilang {

void AddChild_Loop_Op(Ila& m);
void AddChild_Find_Max(Ila& m);

auto uf_out = SortRef::BV(RELAY_FUNC_DATA_IN_BITWIDTH);
auto uf_in1 = SortRef::BV(RELAY_FUNC_DATA_IN_BITWIDTH);
auto uf_in2 = SortRef::BV(RELAY_FUNC_DATA_IN_BITWIDTH);
FuncRef signed_gt("signed_gt", uf_out, uf_in1, uf_in2);

void DefineMaxpooling2D(Ila& m) {
  
  {
    auto instr = m.NewInstr(F_MAXPOOING_2D);

    auto func_id_match = (m.input(RELAY_FUNC_ID_IN) == F_MAXPOOLING_2D_ID);
    auto func_run = (m.input(RELAY_FUNC_RUN_IN) == RELAY_FUNC_RUN_ON);

    instr.SetDecode(func_id_match & func_run);

    // function arguments
    auto height_in = m.input(DATA_IN_Y); // 32
    auto width_in = m.input(DATA_IN_X);

    auto pool_y = m.input(POOL_SIZE_Y_IN);
    auto pool_x = m.input(POOL_SIZE_X_IN);

    auto stride_y = m.input(STRIDES_Y_IN); // 8
    auto stride_x = m.input(STRIDES_X_IN);

    auto stride_y_32 = Concat(BvConst(0, 24), stride_y);
    auto stride_x_32 = Concat(BvConst(0, 24), stride_x);
    // calculate the output tensor size
    auto height_out_tmp = height_in / stride_y_32;
    auto width_out_tmp = width_in / stride_x_32;

    // states used for child
    auto flag_start = m.state(MAXPOOLING_START_FLAG); // ON/OFF

    // maxpooling state machine
    auto state = m.state(MAXPOOLING_STATE);

    auto cntr_X = m.state(MAXPOOLING_X_LOOP_CNTR);
    auto cntr_Y = m.state(MAXPOOLING_Y_LOOP_CNTR);

    auto height_out = m.state(MAXPOOLING_DATA_OUT_HEIGHT);
    auto width_out = m.state(MAXPOOLING_DATA_OUT_WIDTH);

    // states update for child 
    instr.SetUpdate(flag_start, BvConst(FLAG_ON, MAXPOOLING_START_FLAG_BITWIDTH));

    instr.SetUpdate(state, BvConst(MAXPOOLING_STATE_FIND_MAX, MAXPOOLING_STATE_BITWIDTH));

    instr.SetUpdate(cntr_X, BvConst(0, MAXPOOLING_X_LOOP_CNTR_BITWIDTH));
    instr.SetUpdate(cntr_Y, BvConst(0, MAXPOOLING_Y_LOOP_CNTR_BITWIDTH));

    instr.SetUpdate(height_out, height_out_tmp);
    instr.SetUpdate(width_out, width_out_tmp);

    // add child to do the loop
    AddChild_Loop_Op(m);
  }
}

  void AddChild_Loop_Op(Ila& m) {
    auto child = m.NewChild("maxpooling_loop_op");

    auto flag_start = m.state(MAXPOOLING_START_FLAG); // ON/OFF

    // maxpooling state machine
    auto state = m.state(MAXPOOLING_STATE);

    child.SetValid(flag_start == FLAG_ON);
    child.SetFetch(BvConst(1,1));

    auto cntr_X = m.state(MAXPOOLING_X_LOOP_CNTR);
    auto cntr_Y = m.state(MAXPOOLING_Y_LOOP_CNTR);

    auto height_out = m.state(MAXPOOLING_DATA_OUT_HEIGHT);
    auto width_out = m.state(MAXPOOLING_DATA_OUT_WIDTH);

    auto pool_y = m.input(POOL_SIZE_Y_IN);
    auto pool_x = m.input(POOL_SIZE_X_IN);

    auto stride_y = m.input(STRIDES_Y_IN); // 8
    auto stride_x = m.input(STRIDES_X_IN);

    // tensor memory state
    auto tensor = m.state(RELAY_TENSOR_MEM);

    // child states for find max
    auto cntr_find_max = child.NewBvState(MAXPOOLING_FIND_MAX_CNTR, MAXPOOLING_FIND_MAX_CNTR_BITWIDTH);
    auto result_max = child.NewBvState(MAXPOOLING_FIND_MAX_RESULT, MAXPOOLING_FIND_MAX_RESULT_BITWIDTH);

    // child instruction 1 -- X loop parameter update
    {
      auto instr = child.NewInstr("child_loop_X_update");

      auto cond_flag = (flag_start == FLAG_ON);
      auto cond_state = (state == MAXPOOLING_STATE_INC_X);

      instr.SetDecode(cond_flag & cond_state);

      auto end_of_X = (cntr_X == width_out);

      auto cntr_X_new = Ite(end_of_X,
                              BvConst(0, MAXPOOLING_X_LOOP_CNTR_BITWIDTH),
                              cntr_X + 1);

      auto next_state = Ite(end_of_X,
                              BvConst(MAXPOOLING_STATE_INC_Y, MAXPOOLING_STATE_BITWIDTH),
                              BvConst(MAXPOOLING_STATE_FIND_MAX, MAXPOOLING_STATE_BITWIDTH));

      instr.SetUpdate(cntr_X, cntr_X_new);
      instr.SetUpdate(state, next_state);

    }

    // child instruction 2 -- Y loop parameters update
    {
      auto instr = child.NewInstr("child_loop_Y_update");

      auto cond_flag = (flag_start == FLAG_ON);
      auto cond_state = (state == MAXPOOLING_STATE_INC_Y);

      instr.SetDecode(cond_flag & cond_state);

      auto end_of_Y = (cntr_Y == (height_out - 1));
      auto cntr_Y_new = cntr_Y + 1;
      auto next_state = Ite(end_of_Y,
                              BvConst(MAXPOOLING_STATE_VAR_UPDATE, MAXPOOLING_STATE_BITWIDTH),
                              BvConst(MAXPOOLING_STATE_FIND_MAX, MAXPOOLING_STATE_BITWIDTH));


      instr.SetUpdate(cntr_Y, cntr_Y_new);
      instr.SetUpdate(state, next_state);
    }

    // child instruction 3 -- find max in the given coordinates
    {
      auto instr = child.NewInstr("child_call_find_max");

      auto cond_flag = (flag_start == FLAG_ON);
      auto cond_state = (state == MAXPOOLING_STATE_FIND_MAX);

      instr.SetDecode(cond_flag & cond_state);

      auto next_state = BvConst(MAXPOOLING_STATE_FIND_MAX_CHILD,
                                  MAXPOOLING_STATE_BITWIDTH);

      instr.SetUpdate(cntr_find_max, BvConst(0, MAXPOOLING_FIND_MAX_CNTR_BITWIDTH));
      instr.SetUpdate(state, next_state);

      AddChild_Find_Max(m);
    }
    // child instruction 4 -- write the max value back into the memory
    {
      auto instr = child.NewInstr("child_write_max_value");
      auto cond_flag = (flag_start == FLAG_ON);
      auto cond_state = (state == MAXPOOLING_STATE_WRITE);
      
      instr.SetDecode(cond_flag & cond_state);

      auto addr = cntr_X + cntr_Y * width_out;
      auto next_state = BvConst(MAXPOOLING_STATE_VAR_UPDATE, MAXPOOLING_STATE_BITWIDTH);

      instr.SetUpdate(tensor, Store(tensor, addr, result_max));
      instr.SetUpdate(state, next_state);
    }

    // child instruction 5 -- set flags for finish.
    {
      auto instr = child.NewInstr("child_update_var");
      auto cond_flag = (flag_start == FLAG_ON);
      auto cond_state = (state == MAXPOOLING_STATE_VAR_UPDATE);

      instr.SetDecode(cond_flag & cond_state);

      auto done = (cntr_X == (width_out - 1) & (cntr_Y == (height_out - 1)));
      auto next_state = Ite(done, BvConst(MAXPOOLING_STATE_DONE, MAXPOOLING_STATE_BITWIDTH),
                                    BvConst(MAXPOOLING_STATE_INC_X, MAXPOOLING_STATE_BITWIDTH));
      
      instr.SetUpdate(state, next_state);
    }
    
  }

  void AddChild_Find_Max(Ila& m) {
    auto child_loop = m.child("maxpooling_loop_op");
    auto child_find_max = child_loop.NewChild("maxpooling_find_max_loop");

    auto state = m.state(MAXPOOLING_STATE);

    child_find_max.SetValid(state == MAXPOOLING_STATE_FIND_MAX_CHILD);
    child_find_max.SetFetch(BvConst(1,1));

    auto height_in = m.input(DATA_IN_Y); // 32
    auto width_in = m.input(DATA_IN_X);

    auto pool_y = m.input(POOL_SIZE_Y_IN);
    auto pool_x = m.input(POOL_SIZE_X_IN); // 8

    auto stride_y = m.input(STRIDES_Y_IN); // 8
    auto stride_x = m.input(STRIDES_X_IN);

    auto out_y = m.state(MAXPOOLING_Y_LOOP_CNTR);
    auto out_x = m.state(MAXPOOLING_X_LOOP_CNTR); // 32

    auto pool_y_16 = Concat(BvConst(0, POOL_SIZE_Y_IN_BITWIDTH), pool_y);
    auto pool_x_16 = Concat(BvConst(0, POOL_SIZE_X_IN_BITWIDTH), pool_x);

    auto cntr_find_max = child_loop.state(MAXPOOLING_FIND_MAX_CNTR);
    auto result = child_loop.state(MAXPOOLING_FIND_MAX_RESULT);

    // tensor memory state
    auto tensor = m.state(RELAY_TENSOR_MEM);


    // instruction finding the max value in the pooling window
    {
      auto instr = child_find_max.NewInstr("maxpooling_find_max_op");
      auto window_size = pool_y_16 * pool_x_16;

      auto cntr_cond = (cntr_find_max < window_size);
      auto state_cond = (state == MAXPOOLING_STATE_FIND_MAX_CHILD);

      instr.SetDecode(cntr_cond & state_cond);

      // base coordinates of the pooling window
      auto win_x_base = out_x * Concat(BvConst(0, 24), stride_x);
      auto win_y_base = out_y * Concat(BvConst(0, 24), stride_y);

      // coordinates within the pooling window
      auto win_x_offset = URem(cntr_find_max, pool_x_16);
      auto win_y_offset = cntr_find_max / pool_x_16;

      auto win_x_offset_32 = Concat(BvConst(0, 16), win_x_offset);
      auto win_y_offset_32 = Concat(BvConst(0, 16), win_y_offset);

      // coordinates in the 2D tensor
      auto tensor_x = win_x_base + win_x_offset_32;
      auto tensor_y = win_y_base + win_y_offset_32;

      // calculate the memory address according to the tensor coordinates
      auto addr = tensor_y * width_in + tensor_x;
      
      // fetch the data in the memory
      auto data = Load(tensor, addr);
      
      // auto result_tmp = Ite(cntr_find_max == 0, data,
      //                       Ite(data > result, data, result));

      // use uninterpreted function
      auto result_tmp = Ite(cntr_find_max == 0, data,
                              signed_gt(result, data));
      
      // state updates
      auto find_finish = (cntr_find_max == (window_size - 1));
      auto next_state = Ite(find_finish, 
                              BvConst(MAXPOOLING_STATE_WRITE, MAXPOOLING_STATE_BITWIDTH),
                              BvConst(MAXPOOLING_STATE_FIND_MAX_CHILD, MAXPOOLING_STATE_BITWIDTH));

      instr.SetUpdate(cntr_find_max, cntr_find_max + 1);
      instr.SetUpdate(result, result_tmp);
      instr.SetUpdate(state, next_state);
    }

  }


}; // namespace ilang
