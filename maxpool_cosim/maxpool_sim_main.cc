#include <systemc>
#include <iostream>
#include <string>
#include <fstream>

#include "../flexnlp_sim/FlexNLP-ila/build/sim_model/flex_sim.h"
#include "../relay_sim/relay-ila/build/sim_model/relay_sim.h"

// source module of the testbench
// creating signals for flex_sim model
SC_MODULE(Source) {
  sc_in<bool> clk{"clk"};
  //sc_in<bool> rst;

  // relay simulation model ports
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

  // FlexNLP simulation model ports
  sc_out< sc_biguint<1> > flex_wr_in;
  sc_out< sc_biguint<1> > flex_rd_in;
  sc_out< sc_biguint<32> > flex_addr_in;
  sc_out< sc_biguint<8> > flex_data_in[16];


//  void source_input();

  SC_CTOR(Source) {
    SC_THREAD(source_input);
    sensitive << clk.pos();
  }

  void source_input() {
    // reset relay signals
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

    // reset flexnlp signals
    flex_wr_in = 0;
    flex_rd_in = 0;
    flex_addr_in = "0x0";
    for (int i = 0; i < 16; i++) {
      flex_data_in[i] = "0x0";
    }

    wait(10, SC_NS);

    std::ifstream fin_relay, fin_flex;
    std::string temp;
    std::string hex_hdr = "0x";

    std::string addr_x, addr_y;
    std::string addr_x_format, addr_y_format;
    std::string data_format, addr_format;

    // relay input temp
    std::string func_run, func_id;
    std::string data_in;

    std::string data_in_y, data_in_x;
    std::string pool_y, pool_x;
    std::string stride_y, stride_x;

    // flex input temp
    std::string mode, addr, data;
    
    const char *addr_x_c, *addr_y_c, *addr_c;
    const char *data_byte_c;

    sc_biguint<32> relay_sim_data_in_x_base;

    wait(10, SC_NS);

    fin_relay.open("./test_input_relay.csv", ios::in);
    fin_flex.open("./test_input_flex.csv", ios::in);
    cout << "file open relay? " << fin_relay.is_open() << endl;
    cout << "file open flex? " << fin_flex.is_open() << endl;

    char c_relay, c_flex;
    char x;

    bool read_finish = false;
    int rd_record = 0;

    while(!read_finish) {
      rd_record = 0;
      // file read for relay_sim
      if (std::getline(fin_relay, temp, ',')) {
          std::getline(fin_relay, func_run, ',');
          std::getline(fin_relay, func_id, ',');


          relay_sim_relay_func_run_in_in = func_run.c_str();
          relay_sim_relay_func_id_in = func_id.c_str();

          if (func_id.compare("1") == 0) { // function call for maxpooling
            std::getline(fin_relay, data_in_y, ',');
            std::getline(fin_relay, data_in_x, ',');
            std::getline(fin_relay, pool_y, ',');
            std::getline(fin_relay, pool_x, ',');
            std::getline(fin_relay, stride_y, ',');
            std::getline(fin_relay, stride_x, '\n');

            // delete the possible /r at the end of the line
            x = stride_x[(stride_x.length()-1)];
            if (int(x) == 13) {
                stride_x = stride_x.substr(0,stride_x.length()-1);
            }

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
          }// function call for maxpooling

          else if (func_id.compare("2") == 0) { // function call for store
            std::getline(fin_relay, data_in_x, ',');
            std::getline(fin_relay, data_in_y, '\n');

            // delete the possible /r at the end of the line
            x = data_in_y[(data_in_y.length()-1)];
            if (int(x) == 13) {
                data_in_y = data_in_y.substr(0, data_in_y.length()-1);
            }

            // extract the address
            addr_x = data_in_x.substr(data_in_x.length() - 5, 5);
            addr_x_format = "0x00" + addr_x;
//            cout << "addr_x_format" << '\t' << addr_x_format << endl;
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

            //cout << "@" << sc_time_stamp() << '\t' << "data format: " << data_format << endl;
            std::string data_byte;

            for (int i = 0; i<16; i++) {
              data_byte = data_format.substr(30-2*i, 2);
              int data_test = std::stoi(data_byte, nullptr, 16);

            //cout << "relay data in" << '\t' << hex << data_test << endl;
              relay_sim_relay_data_in_in = data_test;
              relay_sim_data_in_x_in = relay_sim_data_in_x_base + i;
              wait(1, SC_NS);
            }
            

          }// function call for store

          else { // function call for end of file
              std::getline(fin_relay, temp, '\n');
          }

          // testbench read record update
          rd_record += 1;
      } // file input read for relay

      if (std::getline(fin_flex, temp, ',')) { // file input read for FlexNLP
        std::getline(fin_flex, mode, ',');
        std::getline(fin_flex, addr, ',');
        std::getline(fin_flex, data, '\n');

        //cout << "flex read" << endl;

        if (mode.compare("W") == 0) {
          flex_wr_in = 1;
          flex_rd_in = 0;
        } else {
          flex_wr_in = 0;
          flex_rd_in = 1;
        }

        // extract the address
        //cout << "test flex" << endl;
        addr = addr.substr(2);
        addr_format = "0x00" + addr;
        addr_c = addr_format.c_str();
        flex_addr_in = addr_c;

        // extract the data
        data_format.clear();
        if (data.length() <= 34) {
          data_format.append(34 - data.length(), '0');
          data_format.append(data.substr(2));
        } else {
          data_format.append(data.substr(data.length()-32));
        }

        std::string data_byte;
        for (int i = 0; i<16; i++) {
          data_byte = data_format.substr(30-2*i, 2);
          data_byte_c = ("0x" + data_byte).c_str();
          flex_data_in[i] = data_byte_c;
        }

        wait(10, SC_NS);
        // read record update
        rd_record += 1;
      } // file input read for FlexNLP
        
      if (rd_record == 0)
        read_finish = true;


    }

    // cout << "source created for testbench" << endl;
  }
};

SC_MODULE(testbench) {
  SC_HAS_PROCESS(testbench);
  relay_sim relay;
  flex_sim flex;
  Source src;

  sc_clock clk;
  // wire for relay
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

  // wire for FlexNLP
  sc_signal< sc_biguint<1> > flex_wr_signal;
  sc_signal< sc_biguint<1> > flex_rd_signal;
  sc_signal< sc_biguint<32> > flex_addr_signal;
  sc_signal< sc_biguint<8> > flex_data_signal[16];

  testbench(sc_module_name name)
  : sc_module(name),
    clk("clk", 1, SC_NS),
    src("source"),
    relay("relay"),
    flex("flex")
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

    src.flex_wr_in(flex_wr_signal);
    src.flex_rd_in(flex_rd_signal);
    src.flex_addr_in(flex_addr_signal);
    for (int i = 0; i < 16; i++) {
      src.flex_data_in[i](flex_data_signal[i]);
    }


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

    flex.flex_sim_if_axi_rd_in(flex_rd_signal);
    flex.flex_sim_if_axi_wr_in(flex_wr_signal);
    flex.flex_sim_addr_in_in(flex_addr_signal);
    flex.flex_sim_data_in_0_in(flex_data_signal[0]);
    flex.flex_sim_data_in_1_in(flex_data_signal[1]);
    flex.flex_sim_data_in_2_in(flex_data_signal[2]);
    flex.flex_sim_data_in_3_in(flex_data_signal[3]);
    flex.flex_sim_data_in_4_in(flex_data_signal[4]);
    flex.flex_sim_data_in_5_in(flex_data_signal[5]);
    flex.flex_sim_data_in_6_in(flex_data_signal[6]);
    flex.flex_sim_data_in_7_in(flex_data_signal[7]);
    flex.flex_sim_data_in_8_in(flex_data_signal[8]);
    flex.flex_sim_data_in_9_in(flex_data_signal[9]);
    flex.flex_sim_data_in_10_in(flex_data_signal[10]);
    flex.flex_sim_data_in_11_in(flex_data_signal[11]);
    flex.flex_sim_data_in_12_in(flex_data_signal[12]);
    flex.flex_sim_data_in_13_in(flex_data_signal[13]);
    flex.flex_sim_data_in_14_in(flex_data_signal[14]);
    flex.flex_sim_data_in_15_in(flex_data_signal[15]);

    SC_THREAD(run);
  }

  void run() {
    int i = 0;
    bool done = false;
    int stop_addr = 0xdead;

    std::ofstream fout;
    fout.open("./test_output.txt", ofstream::out | ofstream::trunc);
    
    wait(10, SC_NS);
    std::cout << "@" << sc_time_stamp() << " ********* simulation start *********" << std::endl;

    while (!done) {
      if ((relay.relay_sim_relay_func_id.to_int() == 3) &&
            (flex.flex_sim_addr_in.to_int() == stop_addr)) {
              done = true;
            }
      
      wait(100, SC_NS);
    }
    
    if (done) {
      int entry_addr_relay;
      int entry_addr_flex;
      int index_relay, index_flex;
      int tensor_out_y = relay.relay_sim_maxpooling_data_out_height.to_int();
      int tensor_out_x = relay.relay_sim_maxpooling_data_out_width.to_int() / 16;
      int entry_max_16 = tensor_out_x * tensor_out_y;
      int entry_in_group;

      int num_vectors = flex.flex_sim_gb_layer_reduce_num_vector_1.to_int();
      int group_size = 8 * num_vectors; // group size in entry numbers
      int group_index;

      int total, error, passed;

      int x_relay, y_relay, x_flex, y_flex;
      
      fout << "********* output for tensor memory ***********" << endl;

      fout << dec << "input tensor size: " << '\t';
      fout << "height: " << relay.relay_sim_data_in_y.to_int() << '\t' << "width: " << relay.relay_sim_data_in_x.to_int() << endl;
      fout << "pooling: " << '\t';
      fout << "pool_y: "  << relay.relay_sim_pool_size_y.to_int() << '\t' << "pool_x: " << relay.relay_sim_pool_size_x.to_int() << endl;
      fout << "stride: " << '\t';
      fout << "stride_y: " << relay.relay_sim_strides_y_in.to_int() << '\t' << "stride_x: " << relay.relay_sim_strides_x_in.to_int() << endl; 
      fout << endl;
      
      total = error = passed = 0;

      for (int j = 0; j < entry_max_16; j++) {
        entry_in_group = j % group_size;
        group_index = j / group_size;

        y_relay = entry_in_group / tensor_out_x;
        x_relay = entry_in_group % tensor_out_x;

        x_flex = y_relay;
        y_flex = x_relay;
        
        entry_addr_relay = 16*j;

        int entry_g_offset_flex = 2 * group_size * (group_index / 2);
        entry_addr_flex = 16*(entry_g_offset_flex + 16*y_flex + x_flex + (group_index % 2) * 8);
        
        fout << "comparing data at addr: " << hex << 16*j << '\t';
        int err = 0;
        for (int k = 0; k < 16; k++) {
            index_relay = entry_addr_relay + 15 - k;
            index_flex = entry_addr_flex + 15 -k;
            //cout << "index of the matrix: " << index << endl;
            int dat_relay = relay.relay_sim_relay_tensor_mem[index_relay].to_int();
            int dat_flex = flex.flex_sim_gb_core_large_buffer[index_flex].to_int(); 

            total += 1;
            if (dat_relay == dat_flex) {
              passed += 1;
            } else {
              error += 1;
              err += 1;
            }
        }
        if (err == 0) {
            fout << "correct!" << endl;
        } else {
            fout << "error!" << endl;
            fout << "group index: " << group_index << '\t' << "g_offset_flex: " << hex << entry_g_offset_flex << endl;
            fout << "relay @ addr: " << entry_addr_relay << " ";
            fout << "data: ";
            for (int k = 0; k < 16; k++) {
                index_relay = entry_addr_relay + 15 - k;
                int dat_relay = relay.relay_sim_relay_tensor_mem[index_relay].to_int();
                fout << relay.relay_sim_relay_tensor_mem[index_relay] << ' ';       
            }
            fout << endl;
            fout << "flex @ addr: " << entry_addr_flex << " ";
            fout << "data: ";
            for (int k = 0; k < 16; k++) {
                index_flex = entry_addr_flex + 15 -k;
                int dat_flex = flex.flex_sim_gb_core_large_buffer[index_flex].to_int(); 
                fout << dat_flex << ' ';       
            }
            fout << endl;
        }
      }

      fout << dec << "compared #total: " << total << '\t';
      fout << "#correct: " << passed << '\t';
      fout << "#failed: " << error << endl;

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
