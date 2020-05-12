# Relay ILA

The ILA model of TVM/Relay.

- vector operations (add, multiply, tanh, sigmoid)
- nn dense operation (matrix-vector multiplication)
- maxpooling-2d
- lstm


# Requirements

- TVM/Relay Python interface https://tvm.apache.org/docs/install/from_source.html
- ILAng https://github.com/Bo-Yuan-Huang/ILAng/tree/ilator_opt
    - `-DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=11 -DILANG_BUILD_TEST=OFF -DILANG_BUILD_SYNTH=OFF  -DILANG_INSTALL_DEV=ON`
- SystemC https://www.accellera.org/downloads/standards/systemc
- CMake
- C++11 compiler

# Build instructions

    mkdir build
    cd build/
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=11 ..
    make
    mkdir sim_model
    ./relay
    cp ../app/sim_main.cc sim_model/app/main.cc
    cp ../uninterpreted_func/uninterpreted_func.cc sim_model/extern/
    cd sim_model
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=11 ..
    make
    # optional
    # ../../../script/lstm_test.py 64 lstm.bin
    cp ../../../flex-sim/* .
    mv flex-64x64-step1.bin lstm.bin
    ./relay_sim
    mv flex-64x64-step2.bin lstm.bin
    ./relay_sim
    

# FlexNLP data layout

FlexNLP uses 4 PE cores, the computation load is equally divided between them, which means each core executes the exact same computation but on different data. For example, on a LSTM layer with 64 inputs and 64 outputs, we expect 

- 64 element vector input `x`, 
- 64 element vector cell state from previous timestep `c_t-1`, 
- 64 element vector hidden state (output) from previous timestep `h_t-1`
- 256 x 64 matrix input weights `W_i`, each 64 x 64 sub matrix corresponds to I/G/F/O parts (Input-gate[rows 0-63]/Cell-activation[rows 64-127]/Forget-gate[rows 128-191]/Output-gate[rows 192-255]).
- 256 x 64 matrix hidden weights `W_h`, same division as `W_i`
- 256 element vector input bias `b_i`, Four 64 sub vectors for I/G/F/O
- 256 element vector hidden bias `b_h`, same division as `b_i`

Input `x` is divided into 4 parts, each has 16 elements, stored in GB memory space at `0x33500000, 0x33500100, 0x33500200, 0x33500300`

Input `x` for the second timestep is stored at `0x33500010, 0x33500110, 0x33500210, 0x33500310`

Each PE will store their own parts of cell state `c_t-1`, each storing 16 elements (64/4), PE0 stores [0...15], PE1 stores [16...31] etc.

Hidden states are also divided into 4 equal parts, written to GB memory space (same address as the inputs)

For the weight matrix, each PE will have a quarter of I/G/F/O. 

- PE0 address space contains `W_i` rows [0-15][64-79][128-143][192-207]
- PE1 address space contains `W_i` rows [16-31][80-95][144-159][208-223]
- PE2 address space contains `W_i` rows [32-47][96-111][160-175][224-239]
- PE3 address space contains `W_i` rows [48-63][112-127][176-191][240-255]

Since PEs can only do multiplication between 16x16 matrix and 16 vector, and the sub matrices in each PE is of size 16x64 (for I/G/F/O), they are furthur divided in to four 16x16 blocks then stored sequencially. Therefore, to convert from FlexNLP format, the four 16x16 blocks should be concatenated horizontally for form one 16x64 matrix.

Then, same storage layout for `W_h` for each PE which only stores a quarter of I/G/F/O portions.

The bias vectors are equally divided into 4 segments and distributes to the 4 PEs, each storing them sequentially, `b_h` then `b_i`.

See `convert.cpp` for address translation from Flex to relay layout.



# Input/Output sizes

for input size of I and output size of O

`input` shoud be vectors of size `I`

`cell`, `next_cell`, `hidden`, `next_hidden` shoud be vectors of size `O`

`12h_weight` should be matrix of size `O*4 x I`

`i2h_bias` should be vector of size `O*4`

`h2h_weight` should be matrix of size `O*4 x O`

`h2h_bias` should be vector of size `O*4`

`temp_vector0` `temp_vector1` `temp_vector2` should be vectors of size `O*4`

# instruction sequence

- i2h
    - `O*4` groups of `relay_nn_dense_loop_fma_instr`, `I` per group (`O*I*4` total)
- h2h
    - `O*4` groups of `relay_nn_dense_loop_fma_instr`, `O` per group (`O*O*4` total)
- add dense results
    - `O*4` number of `relay_vector_add_child_instr`
- sigmoid 
    - `O*3` number of `relay_vector_sigmoid_child_instr`
- cell tanh
    - `O` number of `relay_vector_tanh_child_instr`
- forget gate
    - `O` number of `relay_vector_multiply_child_instr`
- input gate
    - `O` number of `relay_vector_multiply_child_instr`
- next cell
    - `O` number of `relay_vector_add_child_instr`
- next cell tanh
    - `O` number of `relay_vector_tanh_child_instr`
- output gate
    - `O` number of `relay_vector_multiply_child_instr`


