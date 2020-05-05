#include <relay/relay_top.h>

#include <ilang/util/log.h>

namespace ilang
{

void DefineNNDense(Ila& m)
{
  auto nn_child = m.child(RELAY_NN_CHILD);

  auto instr = nn_child.NewInstr(RELAY_NN_DENSE_INSTR);

  auto dense_enable = m.state(RELAY_NN_DENSE_ENABLE);
  auto input_size = m.state(RELAY_NN_INPUT_SIZE);
  auto output_size = m.state(RELAY_NN_OUTPUT_SIZE);
  auto input_wrap_around = m.state(RELAY_NN_INPUT_WRAP_AROUND);

  auto weight_addr = m.state(RELAY_NN_WEIGHT_ADDR);
  auto bias_addr = m.state(RELAY_NN_BIAS_ADDR);
  auto input_addr = m.state(RELAY_NN_INPUT_ADDR);
  auto output_addr = m.state(RELAY_NN_OUTPUT_ADDR);

  auto state = m.state(RELAY_NN_DENSE_STATE);

  auto return_state = m.state(RELAY_LSTM_RETURN_STATE);

  auto memory = m.state(RELAY_MEMORY);

  instr.SetDecode((dense_enable == BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW))
    & (input_size != BvConst(0, RELAY_NN_SIZE_BW))
    & (output_size != BvConst(0, RELAY_NN_SIZE_BW))
    & (state == BvConst(RELAY_NN_DENSE_IDLE_STATE, RELAY_NN_DENSE_STATE_BW)));

  auto loop_cntr = m.state(RELAY_NN_DENSE_LOOP_CNTR);
  auto loop_start = m.state(RELAY_NN_DENSE_LOOP_START);

  instr.SetUpdate(loop_cntr, BvConst(0, RELAY_NN_SIZE_BW));
  instr.SetUpdate(loop_start, BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW));
  instr.SetUpdate(state, BvConst(RELAY_NN_DENSE_LOOP_INIT_STATE, RELAY_NN_DENSE_STATE_BW));

  {
    auto loop_child = nn_child.NewChild(RELAY_NN_DENSE_LOOP_CHILD);
    loop_child.SetValid((dense_enable == BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW)) & (loop_start == BvConst(RELAY_FLAG_ON, RELAY_FLAG_BW)));

    auto fma_cntr = loop_child.NewBvState(RELAY_NN_DENSE_LOOP_FMA_CNTR, RELAY_NN_SIZE_BW);
    auto input_index = loop_child.NewBvState(RELAY_NN_DENSE_INPUT_INDEX, RELAY_NN_SIZE_BW);
    auto acc = loop_child.NewBvState(RELAY_NN_DENSE_ACC, RELAY_NN_SIZE_BW);

    {
      auto init_instr = loop_child.NewInstr(RELAY_NN_DENSE_LOOP_INIT_INSTR);
      init_instr.SetDecode(state == BvConst(RELAY_NN_DENSE_LOOP_INIT_STATE, RELAY_NN_DENSE_STATE_BW));
      
      init_instr.SetUpdate(fma_cntr, BvConst(0, RELAY_NN_SIZE_BW));
      init_instr.SetUpdate(input_index, BvConst(0, RELAY_NN_SIZE_BW));
      init_instr.SetUpdate(state, BvConst(RELAY_NN_DENSE_LOOP_FMA_STATE, RELAY_NN_DENSE_STATE_BW));
      init_instr.SetUpdate(acc, BvConst(RELAY_VECTOR_DATA_ZERO, RELAY_VECTOR_DATA_BW));

      {
        auto fma_child = loop_child.NewChild(RELAY_NN_DENSE_FMA_CHILD);
        fma_child.SetValid(state == RELAY_NN_DENSE_LOOP_FMA_STATE);
        {
          auto fma_instr = fma_child.NewInstr(RELAY_NN_DENSE_LOOP_FMA_INSTR);

          fma_instr.SetDecode(state == RELAY_NN_DENSE_LOOP_FMA_STATE);

          auto load_weight_addr = weight_addr + (loop_cntr * input_size + fma_cntr) * RELAY_VECTOR_DATA_BYTES;
          
          auto input_index_plus1 = input_index + BvConst(1, RELAY_NN_SIZE_BW);
          auto next_input_index = Ite((input_wrap_around != BvConst(0, RELAY_NN_SIZE_BW)) & (input_index_plus1 != input_wrap_around),
            BvConst(0, RELAY_NN_SIZE_BW), input_index_plus1);
          
          auto load_input_addr = input_addr +  input_index * RELAY_VECTOR_DATA_BYTES;
          
          auto next_acc = bv_add(acc, bv_multiply(RELAY_LOAD_WORD(memory, load_weight_addr), RELAY_LOAD_WORD(memory, load_input_addr)));

          auto next_fma_cntr = fma_cntr + BvConst(1, RELAY_NN_SIZE_BW);
          auto fma_continue = (next_fma_cntr != input_size);

          auto next_state = Ite(fma_continue, state, BvConst(RELAY_NN_DENSE_LOOP_WRITE_STATE, RELAY_NN_DENSE_STATE_BW));

          fma_instr.SetUpdate(state, next_state);
          fma_instr.SetUpdate(acc, next_acc);
          fma_instr.SetUpdate(input_index, next_input_index);
          fma_instr.SetUpdate(fma_cntr, next_fma_cntr);

        }
      }
    }
    
    {
      auto write_instr = loop_child.NewInstr(RELAY_NN_DENSE_LOOP_WRITE_INSTR);
      write_instr.SetDecode(state == BvConst(RELAY_NN_DENSE_LOOP_WRITE_STATE, RELAY_NN_DENSE_STATE_BW));

      auto next_loop_cntr = loop_cntr + BvConst(1, RELAY_NN_SIZE_BW);
      auto loop_continue = (next_loop_cntr != output_size);
      auto addr_offset = loop_cntr * RELAY_VECTOR_DATA_BYTES;
      auto result = bv_add(acc, RELAY_LOAD_WORD(memory, bias_addr + addr_offset));

      auto next_state = Ite(loop_continue, 
        BvConst(RELAY_NN_DENSE_LOOP_INIT_STATE, RELAY_NN_DENSE_STATE_BW),
        BvConst(RELAY_NN_DENSE_IDLE_STATE, RELAY_NN_DENSE_STATE_BW));

      auto next_dense_enable = RELAY_ITE_FLAG(loop_continue);
      auto next_loop_start = RELAY_ITE_FLAG(loop_continue);
      
      auto lstm_state = m.state(RELAY_LSTM_STATE);
      auto next_lstm_state = Ite(loop_continue, lstm_state, m.state(RELAY_LSTM_RETURN_STATE));
      
      write_instr.SetUpdate(state, next_state);
      write_instr.SetUpdate(dense_enable, next_dense_enable);
      write_instr.SetUpdate(loop_start, next_loop_start);
      write_instr.SetUpdate(lstm_state, next_lstm_state);
      write_instr.SetUpdate(loop_cntr, next_loop_cntr);
      write_instr.SetUpdate(memory, RELAY_STORE_WORD(memory, output_addr + addr_offset, result));
    }
  }

}

};
