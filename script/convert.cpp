#include <iostream>
#include <fstream>

using namespace std;

// sub matrix order in Relay
#define I_MAT 0
#define F_MAT 1
#define G_MAT 2
#define O_MAT 3


void read_16f(ifstream& inf, float* fptr) 
{
    for (int i = 15; i >= 0; i--)
        inf>>fptr[i];
}
void read_sub_mtx(ifstream& inf, float* base, int pe_num, int subscript)
{
    // Four blocks of 16x16 matrix for one subscript
    float *ptr = base + subscript * 64 * 64 + pe_num * 16 * 64;
    for (int b = 0; b < 4; b++)
        for (int i = 0; i < 16; ++i)
            read_16f(inf, ptr+(64*i + 16*b));
}

void read_sub_bias(ifstream& inf, float* base, int pe_num, int subscript)
{
    read_16f(inf, base + (subscript * 64 + pe_num * 16));
}
int main()
{
    const int size = 64;

    float Wi[4*size][size], Wh[4*size][size];
    float Bi[4*size], Bh[4*size];
    float c[size] = {}, h[size] = {}, in[size];


    ifstream inf("lstm.txt");
    

    // I-G-F-O in Flex
    // I-F-G-O in relay

    // (get_16x16_block => save )x4 x4

    for (int pe_num = 0; pe_num < 4; pe_num++)
    {
        read_sub_mtx(inf, (float*)Wi, pe_num, I_MAT);
        read_sub_mtx(inf, (float*)Wi, pe_num, G_MAT);
        read_sub_mtx(inf, (float*)Wi, pe_num, F_MAT);
        read_sub_mtx(inf, (float*)Wi, pe_num, O_MAT);

        read_sub_mtx(inf, (float*)Wh, pe_num, I_MAT);
        read_sub_mtx(inf, (float*)Wh, pe_num, G_MAT);
        read_sub_mtx(inf, (float*)Wh, pe_num, F_MAT);
        read_sub_mtx(inf, (float*)Wh, pe_num, O_MAT);

        read_sub_bias(inf, Bi, pe_num, I_MAT);
        read_sub_bias(inf, Bi, pe_num, G_MAT);
        read_sub_bias(inf, Bi, pe_num, F_MAT);
        read_sub_bias(inf, Bi, pe_num, O_MAT);
        
        read_sub_bias(inf, Bh, pe_num, I_MAT);
        read_sub_bias(inf, Bh, pe_num, G_MAT);
        read_sub_bias(inf, Bh, pe_num, F_MAT);
        read_sub_bias(inf, Bh, pe_num, O_MAT);
    }

    read_16f(inf, in);
    read_16f(inf, in+16);
    read_16f(inf, in+32);
    read_16f(inf, in+48);
    


    ofstream of("lstm-flex.bin", ofstream::binary);
    of.write((char*)&size, 4);
    of.write((char*)&size, 4);
    of.write((char*)in, sizeof(in));
    of.write((char*)c, sizeof(c));
    of.write((char*)h, sizeof(h));
    of.write((char*)Wi, sizeof(Wi));
    of.write((char*)Wh, sizeof(Wh));
    of.write((char*)Bi, sizeof(Bi));
    of.write((char*)Bh, sizeof(Bh));
    cout<<"Wrote "<<of.tellp()<<" bytes to file\n";
    of.close();

    of.open("lstm-flex.in2", ofstream::binary);
    // get second timestep input
    
    read_16f(inf, in);
    read_16f(inf, in+16);
    read_16f(inf, in+32);
    read_16f(inf, in+48);
    
    of.write((char*)in, sizeof(in));
    of.close();
    return 0;
}



// f.write(num_hidden.to_bytes(4, 'little'))
// f.write(num_hidden.to_bytes(4, 'little'))
// i_val.asnumpy().tofile(f)
// cell_val.tofile(f)
// hidden_val.tofile(f)
// i2h_w_val.asnumpy().tofile(f)
// h2h_w_val.asnumpy().tofile(f)
// i2h_b_val.asnumpy().tofile(f)
// h2h_b_val.asnumpy().tofile(f)