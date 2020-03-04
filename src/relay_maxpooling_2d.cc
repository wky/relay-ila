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
    auto flag_start = m.state(MAXPOOLING_START_FLAG);
    auto flag_X = m.state(MAXPOOLING_X_LOOP_FLAG);
    auto flag_Y = m.state(MAXPOOLING_Y_LOOP_FLAG);
    auto flag_find_max = m.state(MAXPOOLING_FIND_MAX_FLAG);

    auto cntr_X = m.state(MAXPOOLING_X_LOOP_CNTR);
    auto cntr_Y = m.state(MAXPOOLING_Y_LOOP_CNTR);

    auto height_out = m.state(MAXPOOLING_DATA_OUT_HEIGHT);
    auto width_out = m.state(MAXPOOLING_DATA_OUT_WIDTH);

    // states update for child 
    instr.SetUpdate(flag_start, BvConst(FLAG_ON, MAXPOOLING_START_FLAG_BITWIDTH));
    instr.SetUpdate(flag_X, BvConst(FLAG_DONE, MAXPOOLING_X_LOOP_FLAG_BITWIDTH));
    instr.SetUpdate(flag_Y, BvConst(FLAG_DONE, MAXPOOLING_Y_LOOP_FLAG_BITWIDTH));
    instr.SetUpdate(flag_find_max, BvConst(FLAG_UNDONE, MAXPOOLING_FIND_MAX_FLAG_BITWIDTH));

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

    auto flag_start = m.state(MAXPOOLING_START_FLAG);
    auto flag_X = m.state(MAXPOOLING_X_LOOP_FLAG);
    auto flag_Y = m.state(MAXPOOLING_Y_LOOP_FLAG);
    auto flag_find_max = m.state(MAXPOOLING_FIND_MAX_FLAG);

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
    child.NewBvState(MAXPOOLING_FIND_MAX_CNTR, MAXPOOLING_FIND_MAX_CNTR_BITWIDTH);

    // child instruction 1 -- X loop parameter update
    {
      auto instr = m.NewInstr("child_loop_X_update");

      auto cond_cntr = (cntr_X < width_out);
      auto cond_flag = (flag_start == FLAG_ON) & 
                        (flag_X == FLAG_UNDONE) &
                          (flag_Y == FLAG_DONE) &
                            (flag_find_max == FLAG_DONE);

      instr.SetDecode(cond_cntr & cond_flag);

      auto end_of_X = (cntr_X == (width_out - 1));

      auto cntr_X_new = Ite(end_of_X,
                                BvConst(0, MAXPOOLING_X_LOOP_CNTR_BITWIDTH),
                                cntr_X + 1);

      instr.SetUpdate(cntr_X, cntr_X_new);

      instr.SetUpdate(flag_X, Ite(end_of_X,
                                    BvConst(FLAG_DONE, MAXPOOLING_X_LOOP_FLAG_BITWIDTH),
                                    BvConst(FLAG_UNDONE, MAXPOOLING_Y_LOOP_FLAG_BITWIDTH)));

      instr.SetUpdate(flag_Y, Ite(end_of_X,
                                        BvConst(FLAG_UNDONE, MAXPOOLING_Y_LOOP_FLAG_BITWIDTH),
                                        BvConst(FLAG_DONE, MAXPOOLING_Y_LOOP_FLAG_BITWIDTH)));

      instr.SetUpdate(flag_find_max, BvConst(FLAG_UNDONE, MAXPOOLING_FIND_MAX_FLAG_BITWIDTH));  
    }

    // child instruction 2 -- Y loop parameters update
    {
      auto instr = m.NewInstr("child_loop_Y_update");

      auto cond_cntr = (cntr_Y < height_out);
      auto cond_flag = (flag_start == FLAG_ON) &
                        (flag_X == FLAG_DONE) &
                          (flag_Y == FLAG_UNDONE) &
                            (flag_find_max == FLAG_DONE);

      instr.SetDecode(cond_cntr & cond_flag);

      auto end_of_Y = (cntr_Y == (height_out - 1));

      auto cntr_Y_new = cntr_Y + 1;

      instr.SetUpdate(cntr_Y, cntr_Y_new);
      instr.SetUpdate(flag_Y, BvConst(FLAG_DONE, MAXPOOLING_Y_LOOP_FLAG_BITWIDTH));
      instr.SetUpdate(flag_X, BvConst(FLAG_UNDONE, MAXPOOLING_X_LOOP_FLAG_BITWIDTH)); 
    }

    // child instruction 3 -- find max in the given coordinates
    {
      auto instr = m.NewInstr("child_call_find_max");

      instr.SetDecode(flag_find_max == FLAG_ON);

    }
    
  }


}; // namespace ilang
