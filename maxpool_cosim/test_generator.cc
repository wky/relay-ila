#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int main() {
  cout << "please input:" << endl;
  cout << "number of timesteps" << endl;
  cout << "number of vectors" << endl;
  cout << "test file path" << endl;

  int num_timesteps, num_vectors;
  string file_in_path;
  cin >> num_timesteps >> num_vectors >> file_in_path;

  ifstream fin;
  fin.open(file_in_path, ios::in);
  ofstream fout;
  fout.open("./test_input_relay.csv", ofstream::out | ofstream::trunc);

  int i = 0;
  
  string temp;
  string mode;
  string addr_in, data_in;

  long long int addr_in_int;
  long long int addr_out_int;

  long long int addr_base = stoi("0x33000000", nullptr, 16);
  long long int addr_min = stoi("0x33500000", nullptr, 16);
  long long int addr_max = stoi("0x33600000", nullptr, 16);

  long long int addr_offset;

  int group_num = num_timesteps / 16;
  int group_size = 16 * num_vectors;

  int group_index, block_index;
  int block_entry_num;

  int block_size = 16;
  int addr_flex;

  int x_flex, y_flex;
  int x_relay, y_relay;

  while(getline(fin, temp, ',')) {
    getline(fin, mode, ',');
    getline(fin, addr_in, ',');
    getline(fin, data_in, '\n');

    addr_in_int = stoi(addr_in, nullptr, 16);
    
    if ((addr_in_int < addr_min) || (addr_in_int > addr_max)) {
      continue;
    }
    fout << "t,";

    group_index = i / group_size;
    addr_flex = i % group_size;
    y_flex = addr_flex / 16;
    x_flex = addr_flex % 16;
    
    x_relay = y_flex;
    y_relay = x_flex;

    addr_offset = (group_index * group_size + y_relay * num_vectors + x_relay) * 16;

    addr_out_int = addr_min + addr_offset;

    cout << dec << group_index << '\t' << x_relay << '\t' << y_relay << '\t';
    cout << hex << addr_offset << '\t' << addr_out_int << endl;


    if (mode.compare("W") == 0) {
      fout << "1,2,";
    }

    fout << hex << addr_out_int << "," << data_in << '\n';
    
    i = i+1;
  }
  
  long long int relay_matrix_y = addr_base + num_timesteps;
  long long int relay_matrix_x = addr_base + num_vectors*16;

  fout << hex << "t,1,1," << relay_matrix_y << "," << relay_matrix_x << ",2,1,2,1" << '\n';
  fout << "2,0,3,0x0000dead,0x0" << endl;
  }
  


