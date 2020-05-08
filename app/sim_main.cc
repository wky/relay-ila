#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include <systemc.h>
#include <relay_sim.h>


#define DATA_BW 32
#define WORD_SIZE 4
#define INC_ADDR_BY_WORDS(__base_addr, __words) do {__base_addr += __words * WORD_SIZE;} while(false)

#define WORD_ADDR(__byte_addr) ((__byte_addr) / WORD_SIZE)

#define PRINT_BIN false
#define READ_WORDS(__fstream, __num_words, __map, __word_cntr) do{ \
            char __word_buf[WORD_SIZE]; \
            if (PRINT_BIN) std::cout<<"READING "<<__num_words<<" words....\n";\
            for (int __idx = 0; __idx < (__num_words); __idx++) { \
              (__fstream).read(__word_buf, WORD_SIZE); \
              unsigned int __word_val = *(unsigned int*)__word_buf;\
              float __fp_val = *(float*)__word_buf;\
              if (PRINT_BIN) {std::cout<<"\tread "<<WORD_SIZE<<" bytes: 0x"<<hex<<__word_val<<dec<<" float: "<<std::setprecision(6)<<__fp_val<<"\n";\
              } (__map)[(__word_cntr)] = __word_val; \
              (__word_cntr) ++; \
            }} while(false)



int in_sz, out_sz;
unsigned int next_cell_addr, next_hidden_addr;
std::ifstream file;
// source module of the testbench
// creating signals for relay_sim model
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


  sc_out< sc_biguint<32> > relay_sim_relay_lstm_in_size_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_out_size_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_input_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_cell_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_next_cell_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_hidden_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_next_hidden_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_i2h_weight_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_h2h_weight_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_i2h_bias_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_h2h_bias_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_temp_vector0_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_temp_vector1_addr_in;
  sc_out< sc_biguint<32> > relay_sim_relay_lstm_temp_vector2_addr_in;
  
  

//  void source_input();

  SC_CTOR(Source) {
    SC_THREAD(source_input);
    // sensitive << clk.pos();
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

    // reset inpusts for lstm

    relay_sim_relay_lstm_in_size_in= 0;
    relay_sim_relay_lstm_out_size_in= 0;
    relay_sim_relay_lstm_input_addr_in= 0;
    relay_sim_relay_lstm_cell_addr_in= 0;
    relay_sim_relay_lstm_next_cell_addr_in= 0;
    relay_sim_relay_lstm_hidden_addr_in= 0;
    relay_sim_relay_lstm_next_hidden_addr_in= 0;
    relay_sim_relay_lstm_i2h_weight_addr_in= 0;
    relay_sim_relay_lstm_h2h_weight_addr_in= 0;
    relay_sim_relay_lstm_i2h_bias_addr_in= 0;
    relay_sim_relay_lstm_h2h_bias_addr_in= 0;
    relay_sim_relay_lstm_temp_vector0_addr_in= 0;
    relay_sim_relay_lstm_temp_vector1_addr_in= 0;
    relay_sim_relay_lstm_temp_vector2_addr_in= 0;

    wait(10, SC_NS);

    /** generating input for LSTM  **/

    relay_sim_relay_func_run_in_in = 1;
    relay_sim_relay_func_id_in = 3; // F_LSTM_ID in relay_func_call.h

    relay_sim_relay_lstm_in_size_in= in_sz;
    relay_sim_relay_lstm_out_size_in= out_sz;
    
    unsigned int base_addr = 0x00000000;
    relay_sim_relay_lstm_input_addr_in = base_addr;
    INC_ADDR_BY_WORDS(base_addr, in_sz);

    relay_sim_relay_lstm_cell_addr_in = base_addr;
    INC_ADDR_BY_WORDS(base_addr, out_sz);

    relay_sim_relay_lstm_hidden_addr_in = base_addr;
    INC_ADDR_BY_WORDS(base_addr, out_sz);

    relay_sim_relay_lstm_i2h_weight_addr_in = base_addr;
    INC_ADDR_BY_WORDS(base_addr, 4 * out_sz * in_sz);

    relay_sim_relay_lstm_h2h_weight_addr_in = base_addr;
    INC_ADDR_BY_WORDS(base_addr, 4 * out_sz * out_sz);

    relay_sim_relay_lstm_i2h_bias_addr_in = base_addr;
    INC_ADDR_BY_WORDS(base_addr, 4 * out_sz);

    relay_sim_relay_lstm_h2h_bias_addr_in = base_addr;
    INC_ADDR_BY_WORDS(base_addr, 4 * out_sz);
    
    relay_sim_relay_lstm_temp_vector0_addr_in = base_addr;
    INC_ADDR_BY_WORDS(base_addr, 4 * out_sz);

    relay_sim_relay_lstm_temp_vector1_addr_in = base_addr;
    INC_ADDR_BY_WORDS(base_addr, 4 * out_sz);

    relay_sim_relay_lstm_temp_vector2_addr_in = base_addr;
    INC_ADDR_BY_WORDS(base_addr, 4 * out_sz);

    relay_sim_relay_lstm_next_cell_addr_in = base_addr;
    next_cell_addr = base_addr;
    INC_ADDR_BY_WORDS(base_addr, out_sz);

    relay_sim_relay_lstm_next_hidden_addr_in = base_addr;
    next_hidden_addr = base_addr;
    INC_ADDR_BY_WORDS(base_addr, out_sz);

    cout << "source created for testbench" << endl;

    wait(50, SC_NS);
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


  // lstm signals
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_in_size_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_out_size_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_input_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_cell_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_next_cell_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_hidden_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_next_hidden_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_i2h_weight_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_h2h_weight_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_i2h_bias_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_h2h_bias_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_temp_vector0_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_temp_vector1_addr_in_signal;
  sc_signal< sc_biguint<32> > relay_sim_relay_lstm_temp_vector2_addr_in_signal;
  

  testbench(sc_module_name name)
  : sc_module(name),
    clk("clk", 1, SC_NS),
    src("source"),
    relay("RELAY_LSTM_TEST")
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

    // bind lstm signals from Source

    src.relay_sim_relay_lstm_in_size_in(relay_sim_relay_lstm_in_size_in_signal);
    src.relay_sim_relay_lstm_out_size_in(relay_sim_relay_lstm_out_size_in_signal);
    src.relay_sim_relay_lstm_input_addr_in(relay_sim_relay_lstm_input_addr_in_signal);
    src.relay_sim_relay_lstm_cell_addr_in(relay_sim_relay_lstm_cell_addr_in_signal);
    src.relay_sim_relay_lstm_next_cell_addr_in(relay_sim_relay_lstm_next_cell_addr_in_signal);
    src.relay_sim_relay_lstm_hidden_addr_in(relay_sim_relay_lstm_hidden_addr_in_signal);
    src.relay_sim_relay_lstm_next_hidden_addr_in(relay_sim_relay_lstm_next_hidden_addr_in_signal);
    src.relay_sim_relay_lstm_i2h_weight_addr_in(relay_sim_relay_lstm_i2h_weight_addr_in_signal);
    src.relay_sim_relay_lstm_h2h_weight_addr_in(relay_sim_relay_lstm_h2h_weight_addr_in_signal);
    src.relay_sim_relay_lstm_i2h_bias_addr_in(relay_sim_relay_lstm_i2h_bias_addr_in_signal);
    src.relay_sim_relay_lstm_h2h_bias_addr_in(relay_sim_relay_lstm_h2h_bias_addr_in_signal);
    src.relay_sim_relay_lstm_temp_vector0_addr_in(relay_sim_relay_lstm_temp_vector0_addr_in_signal);
    src.relay_sim_relay_lstm_temp_vector1_addr_in(relay_sim_relay_lstm_temp_vector1_addr_in_signal);
    src.relay_sim_relay_lstm_temp_vector2_addr_in(relay_sim_relay_lstm_temp_vector2_addr_in_signal);


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


    // bind lstm signals to relay_sim
    relay.relay_sim_relay_lstm_in_size_in(relay_sim_relay_lstm_in_size_in_signal);
    relay.relay_sim_relay_lstm_out_size_in(relay_sim_relay_lstm_out_size_in_signal);
    relay.relay_sim_relay_lstm_input_addr_in(relay_sim_relay_lstm_input_addr_in_signal);
    relay.relay_sim_relay_lstm_cell_addr_in(relay_sim_relay_lstm_cell_addr_in_signal);
    relay.relay_sim_relay_lstm_next_cell_addr_in(relay_sim_relay_lstm_next_cell_addr_in_signal);
    relay.relay_sim_relay_lstm_hidden_addr_in(relay_sim_relay_lstm_hidden_addr_in_signal);
    relay.relay_sim_relay_lstm_next_hidden_addr_in(relay_sim_relay_lstm_next_hidden_addr_in_signal);
    relay.relay_sim_relay_lstm_i2h_weight_addr_in(relay_sim_relay_lstm_i2h_weight_addr_in_signal);
    relay.relay_sim_relay_lstm_h2h_weight_addr_in(relay_sim_relay_lstm_h2h_weight_addr_in_signal);
    relay.relay_sim_relay_lstm_i2h_bias_addr_in(relay_sim_relay_lstm_i2h_bias_addr_in_signal);
    relay.relay_sim_relay_lstm_h2h_bias_addr_in(relay_sim_relay_lstm_h2h_bias_addr_in_signal);
    relay.relay_sim_relay_lstm_temp_vector0_addr_in(relay_sim_relay_lstm_temp_vector0_addr_in_signal);
    relay.relay_sim_relay_lstm_temp_vector1_addr_in(relay_sim_relay_lstm_temp_vector1_addr_in_signal);
    relay.relay_sim_relay_lstm_temp_vector2_addr_in(relay_sim_relay_lstm_temp_vector2_addr_in_signal);    

    SC_THREAD(run);
  }

  void run() {
    int i = 0;
    bool done = false;

    std::ofstream fout;
    fout.open("relay_data_out.txt", ofstream::out | ofstream::trunc);
    // fout.basic_ios<char>::rdbuf(std::cout.rdbuf());
    relay.instr_log.open("relay_instr.log", ofstream::out | ofstream::trunc);
    
    // relay.instr_log.basic_ios<char>::rdbuf(std::cout.rdbuf());
    wait(10, SC_NS);
    std::cout << "@" << sc_time_stamp() << " ********* simulation start *********" << std::endl;

    int word_cntr = 0;
    // order: input, cell, hidden, i2h_weight, h2h_weight, i2h_bias, h2h_bias
    READ_WORDS(file, in_sz, relay.relay_sim_relay_memory, word_cntr);
    READ_WORDS(file, out_sz, relay.relay_sim_relay_memory, word_cntr);
    READ_WORDS(file, out_sz, relay.relay_sim_relay_memory, word_cntr);
    READ_WORDS(file, 4 * out_sz * in_sz, relay.relay_sim_relay_memory, word_cntr);
    READ_WORDS(file, 4 * out_sz * out_sz, relay.relay_sim_relay_memory, word_cntr);
    READ_WORDS(file, 4 * out_sz, relay.relay_sim_relay_memory, word_cntr);
    READ_WORDS(file, 4 * out_sz, relay.relay_sim_relay_memory, word_cntr);

    std::cout<<"word cntr is at : "<<dec<<word_cntr<<"\n";

    wait(100, SC_NS);
    // while(!done) {
      if (relay.relay_sim_relay_lstm_state.to_int() == 12) {
        done = true; // end state: 12 in relay_lstm.h
      }

      //cout << "@" << sc_time_stamp() << '\t';
      //cout << "is write: " << (relay.relay_sim_relay_func_id.to_int() == 2) << '\t';
    // }
      

    if (done) {
      // relay.instr_log.flush();
      wait(100, SC_NS);
      fout << "********* output for tensor memory ***********" << endl;
      fout <<"<><><><><>next_cell_state:\n";
      std::ofstream obin("relay_out.bin");
      for (int i = 0; i < out_sz; i++){
        unsigned int hex_value = relay.relay_sim_relay_memory[WORD_ADDR(next_cell_addr)+i].to_uint();
        float float_value = *(float*)&hex_value;
        fout<<"\t0x"<<hex<<hex_value<<dec<<"\t float: "<<std::setprecision(6)<<float_value<<"\n";
        obin.write((char*)&hex_value ,4);
      }

      int *output_val = new int[out_sz];
      int *benchmark = new int[out_sz];
      file.read((char*)benchmark, out_sz * WORD_SIZE);

      double rel_err_acc = 0;
      fout <<"<><><><><>next_hidden_state:\n";
      for (int i = 0; i < out_sz; i++){
        output_val[i] = relay.relay_sim_relay_memory[WORD_ADDR(next_hidden_addr) + i].to_uint();
        float float_value = ((float*)output_val)[i];
        fout<<"???? 0x"<<hex<<std::setw(8)<<std::setfill('0')<<output_val[i]<<dec<<"\t"<<std::setprecision(6)<<float_value<<"\n";
        float benchmark_value = ((float*)benchmark)[i];
        fout<<"++++ 0x"<<hex<<std::setw(8)<<std::setfill('0')<<benchmark[i]<<dec<<"\t"<<std::setprecision(6)<<benchmark_value<<"\n\n";
        if (benchmark_value != 0.0)
          rel_err_acc += abs(((double)float_value - (double)benchmark_value)/(double)benchmark_value);
      }
      fout<<"Average relative error: "<< rel_err_acc*100.0/out_sz<<"\n";
      fout<<"====== compact float32 output (F[15]...F[0] each row):\n";

      for (int i = 0; i < 4; i++) {
        for (int j = 15; j >=0; j--){
            float fval = ((float*)output_val)[i*16+j];
            fout<<std::setprecision(6)<<fval<<" ";
        }
        fout<<"\n";
      }
      fout.close();
      obin.write((char*)output_val, 4 * out_sz);
      obin.close();
      delete[] output_val;
      delete[] benchmark;
      /*
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
      }*/

      wait(1000, SC_NS);
      sc_stop();
    }

  }
};



int sc_main(int argc, char *argv[]) {
  std::cout << "test started" << endl;
  
  char buf[4];
  file = ifstream("lstm.bin", std::ifstream::binary);
  file.read(buf, 4);
  in_sz = *(int*)buf;
  file.read(buf, 4);
  out_sz = *(int*)buf;
  std::cout<<"input size: "<<in_sz<<"\noutput size: "<<out_sz<<"\n";

  testbench tb("tb");
  sc_start();
  return 0;
}

