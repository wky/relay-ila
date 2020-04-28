# Relay ILA

The ILA model of TVM/Relay.

- vector operations (add, multiply, tanh, sigmoid)
- nn dense operation (matrix-vector multiplication)
- maxpooling-2d
- lstm


# Requirements

- ILAng https://github.com/Bo-Yuan-Huang/ILAng/tree/ilator_opt
    - -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=11 -DILANG_BUILD_TEST=OFF -DILANG_BUILD_SYNTH=OFF  -DILANG_INSTALL_DEV=ON
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
    cd sim_model
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=11 ..
    make
    ./relay_sim
