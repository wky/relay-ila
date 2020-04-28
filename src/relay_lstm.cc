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

// File: relay_lstm.cc

#include <relay/relay_top.h>

#include <ilang/util/log.h>

namespace ilang {


void DefineLSTM(Ila& m) 
{
  auto instr = m.NewInstr(F_LSTM);

  auto func_id_match = (m.input(RELAY_FUNC_ID_IN) == F_LSTM_ID);
  auto func_run = (m.input(RELAY_FUNC_RUN_IN) == RELAY_FUNC_RUN_ON);

  instr.SetDecode(func_id_match & func_run);

  // function arguments
  auto layer_in_size = m.input(RELAY_LSTM_IN_SIZE);
  auto layer_out_size = m.input(RELAY_LSTM_OUT_SIZE);

  auto input_addr = m.input(RELAY_LSTM_INPUT_ADDR);
  auto cell_addr = m.input(RELAY_LSTM_CELL_ADDR); // cell input from prev timestep
  auto next_cell_addr = m.input(RELAY_LSTM_NEXT_CELL_ADDR); // cell output to next timestep

  auto hidden_addr = m.input(RELAY_LSTM_HIDDEN_ADDR); // hidden state from prev timestep
  auto next_hidden_addr = m.input(RELAY_LSTM_NEXT_HIDDEN_ADDR); // hidden state to next timestep

  auto i2h_weight_addr =  m.input(RELAY_LSTM_I2H_WEIGHT_ADDR); // input to hidden weights
  auto h2h_weight_addr =  m.input(RELAY_LSTM_H2H_WEIGHT_ADDR); // hidden to hidden weights

  auto i2h_bias_addr =  m.input(RELAY_LSTM_I2H_BIAS_ADDR); // input to hidden bias
  auto h2h_bias_addr =  m.input(RELAY_LSTM_H2H_BIAS_ADDR);// hidden to hidden bias
  
  auto temp_vector0_addr = m.input(RELAY_LSTM_TEMP_VECTOR0_ADDR);
  auto temp_vector1_addr = m.input(RELAY_LSTM_TEMP_VECTOR1_ADDR);
  auto temp_vector2_addr = m.input(RELAY_LSTM_TEMP_VECTOR2_ADDR);

  // auto in_gate_addr = m.input(LSTM_LAYER_IN_GATE_ADDR);
  // auto forget_gate_addr = m.input(LSTM_LAYER_FORGET_GATE_ADDR);
  // auto out_gate_addr = m.input(LSTM_LAYER_OUT_GATE_ADDR);
  
  auto flag_start = m.state(RELAY_LSTM_START); // ON/OFF

  // lstm state machine
  auto state = m.state(RELAY_LSTM_STATE);
  
  // states update for child 
  instr.SetUpdate(flag_start, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));

  instr.SetUpdate(state, BvConst(RELAY_LSTM_DENSE_I2H_STATE, RELAY_LSTM_STATE_BW));
  
  {
    auto child = m.NewChild(RELAY_LSTM_MATRIX_VECTOR);
    auto child_started = (flag_start == RELAY_LSTM_FLAG_ON);
    child.SetValid(child_started);

    auto dense_enable = m.state(RELAY_NN_DENSE_ENABLE);

    auto dense_input_size = m.state(RELAY_NN_INPUT_SIZE);
    auto dense_output_size = m.state(RELAY_NN_OUTPUT_SIZE);
    auto dense_input_wrap_around = m.state(RELAY_NN_INPUT_WRAP_AROUND);

    auto dense_weight_addr = m.state(RELAY_NN_WEIGHT_ADDR);
    auto dense_bias_addr = m.state(RELAY_NN_BIAS_ADDR);
    auto dense_input_addr = m.state(RELAY_NN_INPUT_ADDR);
    auto dense_output_addr = m.state(RELAY_NN_OUTPUT_ADDR);

    auto dense_state = m.state(RELAY_NN_DENSE_STATE);
    auto return_state = m.state(RELAY_LSTM_RETURN_STATE);
    
    {
      // setup matrix-vector multiplication for I2H
      auto i2h_instr = child.NewInstr(RELAY_LSTM_DENSE_I2H_INSTR);
      i2h_instr.SetDecode(child_started 
        & (state == BvConst(RELAY_LSTM_DENSE_I2H_STATE, RELAY_LSTM_STATE_BW)));

      i2h_instr.SetUpdate(state, BvConst(RELAY_WAIT_STATE, RELAY_LSTM_STATE_BW));
      
      i2h_instr.SetUpdate(dense_enable, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
      i2h_instr.SetUpdate(dense_state, BvConst(RELAY_NN_DENSE_IDLE_STATE, RELAY_NN_DENSE_STATE_BW));
      i2h_instr.SetUpdate(dense_input_size, layer_in_size);
      i2h_instr.SetUpdate(dense_input_wrap_around, BvConst(0, RELAY_VECTOR_SIZE_BW));
      i2h_instr.SetUpdate(dense_output_size, layer_out_size * 4);

      i2h_instr.SetUpdate(dense_weight_addr, i2h_weight_addr);
      i2h_instr.SetUpdate(dense_bias_addr, i2h_bias_addr);
      i2h_instr.SetUpdate(dense_input_addr, input_addr);
      i2h_instr.SetUpdate(dense_output_addr, temp_vector0_addr);
      i2h_instr.SetUpdate(return_state, BvConst(RELAY_LSTM_DENSE_H2H_STATE, RELAY_LSTM_STATE_BW));
    }
    {
      // setup matrix-vector multiplication for H2H 
      auto h2h_instr = child.NewInstr(RELAY_LSTM_DENSE_H2H_INSTR);
      h2h_instr.SetDecode(child_started 
        & (state == BvConst(RELAY_LSTM_DENSE_H2H_STATE, RELAY_LSTM_STATE_BW)));

      h2h_instr.SetUpdate(state, BvConst(RELAY_WAIT_STATE, RELAY_LSTM_STATE_BW));
      
      h2h_instr.SetUpdate(dense_enable, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
      h2h_instr.SetUpdate(dense_state, BvConst(RELAY_NN_DENSE_IDLE_STATE, RELAY_NN_DENSE_STATE_BW));

      h2h_instr.SetUpdate(dense_input_size, layer_out_size);
      h2h_instr.SetUpdate(dense_input_wrap_around, BvConst(0, RELAY_VECTOR_SIZE_BW));
      h2h_instr.SetUpdate(dense_output_size, layer_out_size * 4);

      h2h_instr.SetUpdate(dense_weight_addr, h2h_weight_addr);
      h2h_instr.SetUpdate(dense_bias_addr, h2h_bias_addr);
      h2h_instr.SetUpdate(dense_input_addr, hidden_addr);
      h2h_instr.SetUpdate(dense_output_addr, temp_vector1_addr);
      h2h_instr.SetUpdate(return_state, BvConst(RELAY_LSTM_ADD_DENSE_STATE, RELAY_LSTM_STATE_BW));

    }

    auto vadd_enable = m.state(RELAY_VECTOR_ADD_ENABLE);
    auto vadd_size = m.state(RELAY_VECTOR_OP_SIZE);
    auto vadd_op0_addr = m.state(RELAY_VECTOR_ADD_OP0_ADDR);
    auto vadd_op1_addr = m.state(RELAY_VECTOR_ADD_OP1_ADDR);
    auto vadd_output_addr = m.state(RELAY_VECTOR_ADD_OUTPUT_ADDR);
    auto vadd_start = m.state(RELAY_VECTOR_ADD_START);

    {
      // setup vector-add of temp0 and temp1
      auto add_dense_instr = child.NewInstr(RELAY_LSTM_ADD_DENSE_INSTR);
      add_dense_instr.SetDecode(child_started 
        & (state == BvConst(RELAY_LSTM_ADD_DENSE_STATE, RELAY_LSTM_STATE_BW)));

      add_dense_instr.SetUpdate(state, BvConst(RELAY_WAIT_STATE, RELAY_LSTM_STATE_BW));
      add_dense_instr.SetUpdate(vadd_enable, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
      add_dense_instr.SetUpdate(vadd_start, BvConst(RELAY_FLAG_OFF, RELAY_FLAG_BW));
      add_dense_instr.SetUpdate(vadd_size, layer_out_size * 4);
      add_dense_instr.SetUpdate(vadd_op0_addr, temp_vector0_addr);
      add_dense_instr.SetUpdate(vadd_op1_addr, temp_vector1_addr);
      add_dense_instr.SetUpdate(vadd_output_addr, temp_vector2_addr);
      add_dense_instr.SetUpdate(return_state, BvConst(RELAY_LSTM_SIGMOID_STATE, RELAY_LSTM_STATE_BW));
    }

    auto vsig_enable = m.state(RELAY_VECTOR_SIGMOID_ENABLE);
    auto vsig_size = m.state(RELAY_VECTOR_OP_SIZE);
    auto vsig_op0_addr = m.state(RELAY_VECTOR_SIGMOID_OP0_ADDR);
    auto vsig_output_addr = m.state(RELAY_VECTOR_SIGMOID_OUTPUT_ADDR);
    auto vsig_start = m.state(RELAY_VECTOR_SIGMOID_START);
    {
      // setup vector-sigmoid on temp2 (first 3 slices, forget/input/output gates)
      // output => temp0
      auto sigmoid_instr = child.NewInstr(RELAY_LSTM_SIGMOID_INSTR);
      sigmoid_instr.SetDecode(child_started
        & (state == BvConst(RELAY_LSTM_SIGMOID_STATE, RELAY_LSTM_STATE_BW)));

      sigmoid_instr.SetUpdate(state, BvConst(RELAY_WAIT_STATE, RELAY_LSTM_STATE_BW));
      sigmoid_instr.SetUpdate(vsig_enable, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
      sigmoid_instr.SetUpdate(vsig_start, BvConst(RELAY_FLAG_OFF, RELAY_FLAG_BW));
      sigmoid_instr.SetUpdate(vsig_size, layer_out_size * 3);
      sigmoid_instr.SetUpdate(vsig_op0_addr, temp_vector2_addr);
      sigmoid_instr.SetUpdate(vsig_output_addr, temp_vector0_addr);
      sigmoid_instr.SetUpdate(return_state, BvConst(RELAY_LSTM_CELL_TANH_STATE, RELAY_LSTM_STATE_BW));
    }

    auto vtanh_enable = m.state(RELAY_VECTOR_TANH_ENABLE);
    auto vtanh_size = m.state(RELAY_VECTOR_OP_SIZE);
    auto vtanh_op0_addr = m.state(RELAY_VECTOR_TANH_OP0_ADDR);
    auto vtanh_output_addr = m.state(RELAY_VECTOR_TANH_OUTPUT_ADDR);
    auto vtanh_start = m.state(RELAY_VECTOR_TANH_START);
    {
      // setup vector-tanh on temp2 (4th slice, "cell input activation")
      // output => temp0 (4th slice)
      auto cell_tanh_instr = child.NewInstr(RELAY_LSTM_CELL_TANH_INSTR);
      cell_tanh_instr.SetDecode(child_started
        & (state == BvConst(RELAY_LSTM_CELL_TANH_STATE, RELAY_LSTM_STATE_BW)));

      auto addr_offset = layer_out_size * (RELAY_VECTOR_DATA_BYTES * 3);
      cell_tanh_instr.SetUpdate(state, BvConst(RELAY_WAIT_STATE, RELAY_LSTM_STATE_BW));
      cell_tanh_instr.SetUpdate(vtanh_enable, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
      cell_tanh_instr.SetUpdate(vtanh_start, BvConst(RELAY_FLAG_OFF, RELAY_FLAG_BW));
      cell_tanh_instr.SetUpdate(vtanh_size, layer_out_size);
      cell_tanh_instr.SetUpdate(vtanh_op0_addr, temp_vector2_addr + addr_offset);
      cell_tanh_instr.SetUpdate(vtanh_output_addr, temp_vector0_addr + addr_offset);
      cell_tanh_instr.SetUpdate(return_state, BvConst(RELAY_LSTM_FORGET_GATE_STATE, RELAY_LSTM_STATE_BW)); 
    }

    auto vmul_enable = m.state(RELAY_VECTOR_MULTIPLY_ENABLE);
    auto vmul_size = m.state(RELAY_VECTOR_OP_SIZE);
    auto vmul_op0_addr = m.state(RELAY_VECTOR_MULTIPLY_OP0_ADDR);
    auto vmul_op1_addr = m.state(RELAY_VECTOR_MULTIPLY_OP1_ADDR);
    auto vmul_output_addr = m.state(RELAY_VECTOR_MULTIPLY_OUTPUT_ADDR);
    auto vmul_start = m.state(RELAY_VECTOR_MULTIPLY_START);
    {
      // setup vector-multiply between forget gate (1st slice in temp 0) and cell state
      // save into temp 1 (1st slice)

      auto forget_gate_instr = child.NewInstr(RELAY_LSTM_FORGET_GATE_INSTR);
      forget_gate_instr.SetDecode(child_started 
        & (state == BvConst(RELAY_LSTM_FORGET_GATE_STATE, RELAY_LSTM_STATE_BW)));

      forget_gate_instr.SetUpdate(state, BvConst(RELAY_WAIT_STATE, RELAY_LSTM_STATE_BW));
      
      forget_gate_instr.SetUpdate(vmul_enable, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
      forget_gate_instr.SetUpdate(vmul_start, BvConst(RELAY_FLAG_OFF, RELAY_FLAG_BW));
      forget_gate_instr.SetUpdate(vmul_size, layer_out_size);
      forget_gate_instr.SetUpdate(vmul_op0_addr, temp_vector0_addr);
      forget_gate_instr.SetUpdate(vmul_op1_addr, cell_addr);
      forget_gate_instr.SetUpdate(vmul_output_addr, temp_vector1_addr);
      forget_gate_instr.SetUpdate(return_state, BvConst(RELAY_LSTM_INPUT_GATE_STATE, RELAY_LSTM_STATE_BW));
    }

    {
      // setup vector-multiply between input gate (2st slice in temp 0) and "cell input activation" (4th slice in temp 0)
      // save into temp 1 (2nd slice)

      auto input_gate_instr = child.NewInstr(RELAY_LSTM_INPUT_GATE_INSTR);
      input_gate_instr.SetDecode(child_started 
        & (state == BvConst(RELAY_LSTM_INPUT_GATE_STATE, RELAY_LSTM_STATE_BW)));

      auto addr_offset = layer_out_size * RELAY_VECTOR_DATA_BYTES;

      input_gate_instr.SetUpdate(state, BvConst(RELAY_WAIT_STATE, RELAY_LSTM_STATE_BW));
      
      input_gate_instr.SetUpdate(vmul_enable, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
      input_gate_instr.SetUpdate(vmul_start, BvConst(RELAY_FLAG_OFF, RELAY_FLAG_BW));
      input_gate_instr.SetUpdate(vmul_size, layer_out_size);
      input_gate_instr.SetUpdate(vmul_op0_addr, temp_vector0_addr + addr_offset);
      input_gate_instr.SetUpdate(vmul_op1_addr, temp_vector0_addr + addr_offset * 3);
      input_gate_instr.SetUpdate(vmul_output_addr, temp_vector1_addr + addr_offset);
      input_gate_instr.SetUpdate(return_state, BvConst(RELAY_LSTM_NEXT_CELL_STATE, RELAY_LSTM_STATE_BW));

    }

    {
      // setup vector-add between forget gate (temp 1 1st slice) and input gate (temp 1 2nd slice)
      // save into next_cell_addr
      auto next_cell_instr = child.NewInstr(RELAY_LSTM_NEXT_CELL_INSTR);
      next_cell_instr.SetDecode(child_started 
        & (state == BvConst(RELAY_LSTM_NEXT_CELL_STATE, RELAY_LSTM_STATE_BW)));

      next_cell_instr.SetUpdate(state, BvConst(RELAY_WAIT_STATE, RELAY_LSTM_STATE_BW));
      next_cell_instr.SetUpdate(vadd_enable, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
      next_cell_instr.SetUpdate(vadd_start, BvConst(RELAY_FLAG_OFF, RELAY_FLAG_BW));
      next_cell_instr.SetUpdate(vadd_size, layer_out_size);
      next_cell_instr.SetUpdate(vadd_op0_addr, temp_vector1_addr);
      next_cell_instr.SetUpdate(vadd_op1_addr, temp_vector1_addr + layer_out_size * RELAY_VECTOR_DATA_BYTES);
      next_cell_instr.SetUpdate(vadd_output_addr, next_cell_addr);
      next_cell_instr.SetUpdate(return_state, BvConst(RELAY_LSTM_NEXT_CELL_TANH_STATE, RELAY_LSTM_STATE_BW));
    }

    {
      // setup vector-tanh over next cell state
      // save in temp1 (3rd slice)
      auto next_cell_tanh_instr = child.NewInstr(RELAY_LSTM_NEXT_CELL_TANH_INSTR);
      next_cell_tanh_instr.SetDecode(child_started
        & (state == BvConst(RELAY_LSTM_NEXT_CELL_TANH_STATE, RELAY_LSTM_STATE_BW)));

      next_cell_tanh_instr.SetUpdate(state, BvConst(RELAY_WAIT_STATE, RELAY_LSTM_STATE_BW));
      next_cell_tanh_instr.SetUpdate(vtanh_enable, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
      next_cell_tanh_instr.SetUpdate(vtanh_start, BvConst(RELAY_FLAG_OFF, RELAY_FLAG_BW));
      next_cell_tanh_instr.SetUpdate(vtanh_size, layer_out_size);
      next_cell_tanh_instr.SetUpdate(vtanh_op0_addr, next_cell_addr);
      next_cell_tanh_instr.SetUpdate(vtanh_output_addr, temp_vector1_addr + layer_out_size * (RELAY_VECTOR_DATA_BYTES * 2));
      next_cell_tanh_instr.SetUpdate(return_state, BvConst(RELAY_LSTM_OUTPUT_GATE_STATE, RELAY_LSTM_STATE_BW)); 
    }

    {
      // setup vector-mul between output gate (3rd slice of temp0) and tanh of next cell state (3rd slice of temp1)
      // output => next_hidden_addr
      auto output_gate_instr = child.NewInstr(RELAY_LSTM_OUTPUT_GATE_INSTR);
      output_gate_instr.SetDecode(child_started 
        & (state == BvConst(RELAY_LSTM_OUTPUT_GATE_STATE, RELAY_LSTM_STATE_BW)));

      auto addr_offset = layer_out_size * (RELAY_VECTOR_DATA_BYTES * 2);
      output_gate_instr.SetUpdate(state, BvConst(RELAY_WAIT_STATE, RELAY_LSTM_STATE_BW));
      
      output_gate_instr.SetUpdate(vmul_enable, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
      output_gate_instr.SetUpdate(vmul_start, BvConst(RELAY_FLAG_OFF, RELAY_FLAG_BW));
      output_gate_instr.SetUpdate(vmul_size, layer_out_size);
      output_gate_instr.SetUpdate(vmul_op0_addr, temp_vector0_addr + addr_offset);
      output_gate_instr.SetUpdate(vmul_op1_addr, temp_vector1_addr + addr_offset);
      output_gate_instr.SetUpdate(vmul_output_addr, next_hidden_addr);
      output_gate_instr.SetUpdate(return_state, BvConst(RELAY_LSTM_END_STATE, RELAY_LSTM_STATE_BW));

    }
  }
}


  
}; // namespace ilang
