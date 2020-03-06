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

    fin.open("/u/yl29/3LA/test_input_new.csv", ios::in);

    char x;

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

        x = stride_x[(stride_x.length()-1)];
        if (int(x) == 13) {
            stride_x = stride_x.substr(0,stride_x.length()-1);
        }
       

      } else { // function tensor store
        std::getline(fin, data_in_x, ',');
        std::getline(fin, data_in_y, '\n');

        x = data_in_y[(data_in_y.length()-1)];
        if (int(x) == 13) {
            data_in_y = data_in_y.substr(0, data_in_y.length()-1);
        }
      }

//      cout << func_run << endl;
//      cout << func_id << endl;
//      cout << data_in_x << endl;
//      cout << data_in_y << endl;

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
//        cout << "data format " << data_format << endl;
        for (int i = 0; i<16; i++) {
          data_byte = data_format.substr(30-2*i, 2);
//          cout << "data byte " << data_byte << '\t';
          data_byte_c = ("0x" + data_byte).c_str();
//          cout << "data byte c " << data_byte_c << '\t';

          int data_test = std::stoi(data_byte, nullptr, 16);
          relay_sim_relay_data_in_in = data_test;
//          cout << "test mid" << '\t';
          relay_sim_data_in_x_in = relay_sim_data_in_x_base + i;
//          cout << "test out" << endl;  
  //        std::cout << "@" << sc_time_stamp() << '\t';
  //        std::cout << "addr: " << hex << relay_sim_data_in_x_in << '\t';
  //        std::cout << "data: " << hex << relay_sim_relay_data_in_in << std::endl;

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

        //std::cout << "@" << sc_time_stamp() << '\t' << "relay maxpooling instr passed!" << std::endl;
      }


      wait(50, SC_NS);
      //std::cout << "@" << sc_time_stamp() << '\t' << "src reading" << std::endl;
      //std::cout << "@" << sc_time_stamp() << '\t' << "func run: " << func_run << '\t';
      //std::cout << "func id: " << func_id << std::endl;
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

    wait(40000, SC_NS);
    //std::cout << "@" << sc_time_stamp() << '\t' << relay.relay_sim_relay_func_id.to_int() << std::endl;
    if (relay.relay_sim_relay_func_id.to_int() == 3) {
     //   std::cout << "are you ok?" << std::endl;
       done = true;
    //  cout << "what is ur problem?" << endl;
    }
    
    if (done) {
      wait(100, SC_NS);
      fout << "********* output for tensor memory ***********" << endl;

      fout << dec << "input tensor size: " << '\t';
      fout << "height: " << relay.relay_sim_data_in_y.to_int() << '\t' << "width: " << relay.relay_sim_data_in_x.to_int() << endl;
      fout << "pooling: " << '\t';
      fout << "pool_y: "  << relay.relay_sim_pool_size_y.to_int() << '\t' << "pool_x: " << relay.relay_sim_pool_size_x.to_int() << endl;
      fout << "stride: " << '\t';
      fout << "stride_y: " << relay.relay_sim_strides_y_in.to_int() << '\t' << "stride_x: " << relay.relay_sim_strides_x_in.to_int() << endl; 
      fout << endl;
      
      int entry_addr;
      int index;
      int tensor_out_y = relay.relay_sim_maxpooling_data_out_height.to_int();
      int tensor_out_x = relay.relay_sim_maxpooling_data_out_width.to_int() / 16;
      int entry_max_16 = tensor_out_x * tensor_out_y;

      for (int j = 0; j < entry_max_16; j++) {
          entry_addr = 16*j;
          fout << "@" << sc_time_stamp() << ' ';
          fout << "tensor mem @ addr:" << '\t';
          fout << hex << entry_addr << '\t';
          fout << "data:" << '\t';
          for (int k = 0; k < 16; k++) {
              index = 16*j + 15 - k;
              //cout << "index of the matrix: " << index << endl;
              fout << hex << relay.relay_sim_relay_tensor_mem[index] << ' ';
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

