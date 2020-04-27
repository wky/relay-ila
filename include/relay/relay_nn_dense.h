#ifndef RELAY_NN_DENSE_H__
#define RELAY_NN_DENSE_H__

namespace ilang 
{

#define RELAY_NN_SIZE_BW 32
#define RELAY_NN_ADDR_BW 32

#define RELAY_NN_CHILD "relay_nn_child_module"

#define RELAY_NN_DENSE_ENABLE "relay_nn_dense_enable"
#define RELAY_NN_DENSE_INSTR "relay_nn_dense_instr"
#define RELAY_NN_DENSE_LOOP_INIT_INSTR "relay_nn_dense_loop_init_instr"
#define RELAY_NN_DENSE_LOOP_WRITE_INSTR "relay_nn_dense_loop_write_instr"
#define RELAY_NN_DENSE_LOOP_FMA_INSTR "relay_nn_dense_loop_fma_instr"
	
#define RELAY_NN_DENSE_STATE "relay_nn_dense_state"
#define RELAY_NN_DENSE_STATE_BW 8

#define RELAY_NN_DENSE_IDLE_STATE 0
#define RELAY_NN_DENSE_LOOP_INIT_STATE 1
#define RELAY_NN_DENSE_LOOP_WRITE_STATE 2
#define RELAY_NN_DENSE_LOOP_FMA_STATE 3

#define RELAY_NN_DENSE_LOOP_CHILD "relay_nn_dense_loop_child_module"
#define RELAY_NN_DENSE_FMA_CHILD "relay_nn_dense_fma_child_module"

#define RELAY_NN_INPUT_SIZE "relay_nn_input_size"
#define RELAY_NN_OUTPUT_SIZE "relay_nn_output_size"
#define RELAY_NN_INPUT_WRAP_AROUND "relay_nn_input_wrap_around"

#define RELAY_NN_WEIGHT_ADDR "relay_nn_weight_addr"
#define RELAY_NN_BIAS_ADDR "relay_nn_bias_addr"
#define RELAY_NN_INPUT_ADDR "relay_nn_input_addr"
#define RELAY_NN_OUTPUT_ADDR "relay_nn_output_addr"


#define RELAY_NN_DENSE_LOOP_CNTR "relay_nn_dense_loop_cntr"
#define RELAY_NN_DENSE_LOOP_START "relay_nn_dense_loop_start"

// internal child states
#define RELAY_NN_DENSE_INPUT_INDEX "relay_nn_dense_input_index"
#define RELAY_NN_DENSE_LOOP_FMA_CNTR "relay_nn_dense_loop_fma_cntr"
#define RELAY_NN_DENSE_ACC "relay_nn_dense_acc"
}

#endif
