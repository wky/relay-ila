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
  fout.open("./test_input_new.csv", ofstream::out | ofstream::trunc);

  int i = 0;
  
  string temp;
  string mode;
  string addr_in, data_in;

  long long int addr_in_int;
  long long int addr_out_int;

  long long int addr_min = stoi("0x33500000", nullptr, 16);
  long long int addr_max = stoi("0x33600000", nullptr, 16);

  long long int addr_offset;

  int group_num = num_timesteps / 16;
  int group_size = num_timesteps * num_vectors;

  int group_index, block_index;
  int block_entry_num;

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
    block_index = (i % group_size) / num_vectors;
    block_entry_num = (i % group_size) % num_vectors;

    cout << group_index << '\t' << block_index << '\t' << block_entry_num << '\n';

    addr_offset = (group_index*group_size + block_entry_num*num_vectors + block_index) * 16;

    addr_out_int = addr_min + addr_offset;

    if (mode.compare("W") == 0) {
      fout << "1,2,";
    }

    fout << hex << addr_out_int << "," << data_in << '\n';
    
    i = i+1;
  }
  }
  


