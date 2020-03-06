#include <systemc>
#include <iostream>
#include <string>
#include <fstream>

#include "relay_sim.h"

// source module of the testbench
// creating signals for flex_sim model
SC_MODULE(Source) {
  sc_in<bool> clk{"clk"};
  //sc_in<bool> rst;

  sc_out< sc_biguint<1> > relay_sim_relay_func_run_in_in;
  sc_out< sc_biguint<8> > relay_sim_relay_func_id_in;
  sc_out< sc_biguint<8> > relay_sim_relay_data_in_in;
  sc_out< sc_biguint<8> > relay_sim_data_in_batch_in;
  sc_out< sc_biguint<8> > relay_sim_data_in_channel_in;
  sc_out< sc_biguint<32> > relay_sim_data_in_y_in;
  sc_out< sc_biguint<32> > relay_sim_data_in_x_in;
  sc_out< sc_biguint<8> > relay_sim_pool_size_y_in;
  sc_out< sc_biguint<8> > relay_sim_pool_size_x_in;
  sc_out< sc_biguint<8> > relay_sim_strides_y_in_in;
  sc_out< sc_biguint<8> > relay_sim_strides_x_in_in;
  sc_out< sc_biguint<8> > relay_sim_padding_in_y_in;
  sc_out< sc_biguint<8> > relay_sim_padding_in_x_in;
  sc_out< sc_biguint<8> > relay_sim_layout_in_in;
  sc_out< sc_biguint<8> > relay_sim_ceil_mode_in_in;

//  void source_input();

  SC_CTOR(Source) {
    SC_THREAD(source_input);
    sensitive << clk.pos();
  }

  void source_input() {
    // reset
    relay_sim_relay_func_run_in_in = 0;
    relay_sim_relay_func_id_in = 0;
    relay_sim_relay_data_in_in = 0;
    relay_sim_data_in_batch_in = 0;
    relay_sim_data_in_channel_in = 0;
    relay_sim_data_in_y_in = 0;
    relay_sim_data_in_x_in = 0;
    relay_sim_pool_size_y_in = 0;
    relay_sim_pool_size_x_in = 0;
    relay_sim_strides_y_in_in = 0;
    relay_sim_strides_x_in_in = 0;
    relay_sim_padding_in_y_in = 0;
    relay_sim_padding_in_x_in = 0;
    relay_sim_layout_in_in = 0;
    relay_sim_ceil_mode_in_in = 0;

    wait(10, SC_NS);

    std::ifstream fin;
    std::string temp;
    std::string hex_hdr = "0x";

    std::string addr_x, addr_y;
    std::string addr_x_format, addr_y_format;
    std::string data_format;

    // relay input temp
    std::string func_run, func_id;
    std::string data_in;

    std::string data_in_y, data_in_x;
    std::string pool_y, pool_x;
    std::string stride_y, stride_x;
    
    const char *addr_x_c, *addr_y_c;
    const char *data_byte_c;

    sc_biguint<32> relay_sim_data_in_x_base;

    wait(10, SC_NS);

    fin.open("/u/yl29/3LA/test_input_relay.csv", ios::in);

    while(std::getline(fin, temp, ',')) {
      std::getline(fin, func_run, ',');
      std::getline(fin, func_id, ',');

      if (func_id.compare("1") == 0) { // function maxpooling
        // std::getline(fin, data_in, ',');
        std::getline(fin, data_in_y, ',');
        std::getline(fin, data_in_x, ',');
        std::getline(fin, pool_y, ',');
        std::getline(fin, pool_x, ',');
        std::getline(fin, stride_y, ',');
        std::getline(fin, stride_x, '\n');


      } else { // function tensor store
        std::getline(fin, data_in_x, ',');
        std::getline(fin, data_in_y, '\n');
      }

      relay_sim_relay_func_run_in_in = func_run.c_str();
      relay_sim_relay_func_id_in = func_id.c_str();

      // tensor store
      if (func_id.compare("2") == 0) {
        //TODO: adapt the correct format of addr
        // extract the address
        addr_x = data_in_x.substr(data_in_x.length() - 5, 5);
        addr_x_format = "0x00" + addr_x;
        addr_x_c = addr_x_format.c_str();
        relay_sim_data_in_x_base = addr_x_c;

        // extract the data
        data_format.clear();
        if (data_in_y.length() <= 34) {
          data_format.append(34 - data_in_y.length(), '0');
          data_format.append(data_in_y.substr(2));
        } else {
          data_format.append(data_in_y.substr(data_in_y.length()-32));
        }
        // TODO: modify here
        std::string data_byte;
        for (int i = 0; i<16; i++) {
          data_byte = data_format.substr(30-2*i, 2);
          data_byte_c = ("0x" + data_byte).c_str();

          relay_sim_relay_data_in_in = data_byte_c;
          relay_sim_data_in_x_in = relay_sim_data_in_x_base + i;

          std::cout << "@" << sc_time_stamp() << '\t';
          std::cout << "addr: " << hex << relay_sim_data_in_x_in << '\t';
          std::cout << "data: " << hex << relay_sim_relay_data_in_in << std::endl;

          wait(1, SC_NS);
        }
      }

      // maxpooling
      if (func_id.compare("1") == 0) {
    
        // extract the address
        addr_x = data_in_x.substr(data_in_x.length() - 5, 5);
        addr_x_format = "0x00" + addr_x;
        addr_x_c = addr_x_format.c_str();

        addr_y = data_in_y.substr(data_in_y.length() - 5, 5);
        addr_y_format = "0x00" + addr_y;
        addr_y_c = addr_y_format.c_str();

        // pass the data to the port
        relay_sim_relay_data_in_in = 0;
        relay_sim_data_in_y_in = addr_y_c;
        relay_sim_data_in_x_in = addr_x_c;
        relay_sim_pool_size_y_in = pool_y.c_str();
        relay_sim_pool_size_x_in = pool_x.c_str();
        relay_sim_strides_y_in_in = stride_y.c_str();
        relay_sim_strides_x_in_in = stride_x.c_str();

        std::cout << "@" << sc_time_stamp() << '\t' << "relay maxpooling instr passed!" << std::endl;
      }


      wait(50, SC_NS);
    }

    // cout << "source created for testbench" << endl;
  }
};

SC_MODULE(testbench) {
  SC_HAS_PROCESS(testbench);
  relay_sim relay;
  Source src;

  sc_clock clk;

  sc_signal< sc_biguint<1> > relay_sim_relay_func_run_in_signal;
  sc_signal< sc_biguint<8> > relay_sim_relay_func_id_signal;
  sc_signal< sc_biguint<8> > relay_sim_relay_data_in_signal;
  sc_signal< sc_biguint<8> > relay_sim_data_in_batch_signal;
  sc_signal< sc_biguint<8> > relay_sim_data_in_channel_signal;
  sc_signal< sc_biguint<32> > relay_sim_data_in_y_signal;
  sc_signal< sc_biguint<32> > relay_sim_data_in_x_signal;
  sc_signal< sc_biguint<8> > relay_sim_pool_size_y_signal;
  sc_signal< sc_biguint<8> > relay_sim_pool_size_x_signal;
  sc_signal< sc_biguint<8> > relay_sim_strides_y_in_signal;
  sc_signal< sc_biguint<8> > relay_sim_strides_x_in_signal;
  sc_signal< sc_biguint<8> > relay_sim_padding_in_y_signal;
  sc_signal< sc_biguint<8> > relay_sim_padding_in_x_signal;
  sc_signal< sc_biguint<8> > relay_sim_layout_in_signal;
  sc_signal< sc_biguint<8> > relay_sim_ceil_mode_in_signal;

  testbench(sc_module_name name)
  : sc_module(name),
    clk("clk", 1, SC_NS),
    src("source"),
    relay("flexnlp")
    {
    // binding the signals from the source
    src.clk(clk);

    src.relay_sim_relay_func_run_in_in(relay_sim_relay_func_run_in_signal);
    src.relay_sim_relay_func_id_in(relay_sim_relay_func_id_signal);
    src.relay_sim_relay_data_in_in(relay_sim_relay_data_in_signal);
    src.relay_sim_data_in_batch_in(relay_sim_data_in_batch_signal);
    src.relay_sim_data_in_channel_in(relay_sim_data_in_channel_signal);
    src.relay_sim_data_in_y_in(relay_sim_data_in_y_signal);
    src.relay_sim_data_in_x_in(relay_sim_data_in_x_signal);
    src.relay_sim_pool_size_y_in(relay_sim_pool_size_y_signal);
    src.relay_sim_pool_size_x_in(relay_sim_pool_size_x_signal);
    src.relay_sim_strides_y_in_in(relay_sim_strides_y_in_signal);
    src.relay_sim_strides_x_in_in(relay_sim_strides_x_in_signal);
    src.relay_sim_padding_in_y_in(relay_sim_padding_in_y_signal);
    src.relay_sim_padding_in_x_in(relay_sim_padding_in_x_signal);
    src.relay_sim_layout_in_in(relay_sim_layout_in_signal);
    src.relay_sim_ceil_mode_in_in(relay_sim_ceil_mode_in_signal);


    // binding the signals for the model
    relay.relay_sim_relay_func_run_in_in(relay_sim_relay_func_run_in_signal);
    relay.relay_sim_relay_func_id_in(relay_sim_relay_func_id_signal);
    relay.relay_sim_relay_data_in_in(relay_sim_relay_data_in_signal);
    relay.relay_sim_data_in_batch_in(relay_sim_data_in_batch_signal);
    relay.relay_sim_data_in_channel_in(relay_sim_data_in_channel_signal);
    relay.relay_sim_data_in_y_in(relay_sim_data_in_y_signal);
    relay.relay_sim_data_in_x_in(relay_sim_data_in_x_signal);
    relay.relay_sim_pool_size_y_in(relay_sim_pool_size_y_signal);
    relay.relay_sim_pool_size_x_in(relay_sim_pool_size_x_signal);
    relay.relay_sim_strides_y_in_in(relay_sim_strides_y_in_signal);
    relay.relay_sim_strides_x_in_in(relay_sim_strides_x_in_signal);
    relay.relay_sim_padding_in_y_in(relay_sim_padding_in_y_signal);
    relay.relay_sim_padding_in_x_in(relay_sim_padding_in_x_signal);
    relay.relay_sim_layout_in_in(relay_sim_layout_in_signal);
    relay.relay_sim_ceil_mode_in_in(relay_sim_ceil_mode_in_signal);

    SC_THREAD(run);
  }

  void run() {
    int i = 0;
    bool done = false;

    std::ofstream fout;
    fout.open("./test_output_relay.txt", ofstream::out | ofstream::trunc);
    
    wait(10, SC_NS);
    std::cout << "@" << sc_time_stamp() << " ********* simulation start *********" << std::endl;

    while(!done) {
      if (relay.relay_sim_relay_func_id.to_int() == 3) {
        done = true;
      }

      //cout << "@" << sc_time_stamp() << '\t';
      //cout << "is write: " << (relay.relay_sim_relay_func_id.to_int() == 2) << '\t';
    }
      

    if (done) {
      wait(100, SC_NS);
      fout << "********* output for tensor memory ***********" << endl;
      
      int entry_addr;
      int index;
      int tensor_out_y = relay.relay_sim_maxpooling_data_out_height.to_int() / 16;
      int tensor_out_x = relay.relay_sim_maxpooling_data_out_width.to_int() / 16;
      int entry_max_16 = tensor_out_x * tensor_out_y;

      for (int j = 0; j < entry_max_16; j++) {
          entry_addr = 16*j;
          fout << "tensor mem @ addr:" << '\t';
          fout << hex << entry_addr << '\t';
          fout << "data:" << '\t';
          for (int k = 0; k < 16; k++) {
              index = 16*j + 15 - k;
              fout << hex << relay.decode_relay_sim_func_tensor_store_update_relay_sim_relay_tensor_mem_map[index] << ' ';
          }
          fout << endl;
      }

      wait(1000, SC_NS);
      sc_stop();
    }

  }
};

int sc_main(int argc, char *argv[]) {
  cout << "test started" << endl;
  testbench tb("tb");
  sc_start();
  return 0;
}

