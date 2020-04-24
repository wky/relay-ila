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

// File: main.cc

#include <relay/relay_top.h>

#include <fstream>
#include <ilang/util/log.h>

#include <iostream>
#include <ilang/ila/instr_lvl_abs.h>
#include <ilang/target-sc/ila_sim.h>

using namespace ilang;

int main() {
  // get the ILA model
  auto relay = GetRelayIla("relay_sim");

  ILA_INFO << "Model: " << relay;
  ILA_INFO << "#instr: " << relay.instr_num();
  ILA_INFO << "#input: " << relay.input_num();
  ILA_INFO << "#state: " << relay.state_num();

  for (auto i = 0; i < relay.child_num(); i++) {
    ILA_INFO << "Model: " << relay.child(i);
    ILA_INFO << "#instr: " << relay.child(i).instr_num();
    ILA_INFO << "#input: " << relay.child(i).input_num();
    ILA_INFO << "#state: " << relay.child(i).state_num();
  }

  auto model = relay.get();
  ILA_INFO << "before calling sim gen function";
  // simulation generation
  IlaSim simulator_generator;
  std::string sim_gen_dir = "./sim_model/";
  std::string systemc_path = "/opt/systemc";
  bool cpp_gen = false;
  // ILA_INFO << "test";
  simulator_generator.set_instr_lvl_abs(model);
  simulator_generator.set_systemc_path(systemc_path);
  // ILA_INFO << "before debug";
  //simulator_generator.sim_gen_decode_d();
  //simulator_generator.enable_cmake_support();
  simulator_generator.sim_gen(sim_gen_dir, false, true, cpp_gen);

  return 0;
}
