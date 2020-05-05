#include "systemc.h"
#include "relay_sim.h"
#include <iostream>
#include <math.h>

// floating point number
#define DATA_BW 32
#define DATA_BYTES 4

/** floating point operations **/
sc_biguint<DATA_BW> relay_sim::bv_tanh(sc_biguint<DATA_BW> op0)
{
  unsigned int i = op0.to_uint();
  float f = (*(float*)&i);
  float res = tanh(f);
  int ires = *(int*)&res;
  // printf("bvtanh 0x%08x (%.6f) = 0x%08x (%.6f)\n", i, f, ires, res);
  return ires;
}

sc_biguint<DATA_BW> relay_sim::bv_sigmoid(sc_biguint<DATA_BW> op0)
{
  unsigned int i = op0.to_uint();
  float f = (*(float*)&i);
  float res = 1.0/(exp(-f) + 1);
  int ires = *(int*)&res;
  // printf("bvsig 0x%08x (%.6f) = 0x%08x (%.6f)\n", i, f, ires, res);
  return ires;
}

sc_biguint<DATA_BW> relay_sim::bv_add(sc_biguint<DATA_BW> op0, sc_biguint<DATA_BW> op1)
{
  unsigned int i0 = op0.to_uint(), i1 = op1.to_uint();
  float f0 = (*(float*)&i0), f1 = (*(float*)&i1);
  
  float res = f0 + f1;
  unsigned int ires = *(int*)&res;
  // printf("bvadd 0x%08x (%.6f) + 0x%08x (%.6f) = 0x%08x (%.6f)\n", i0, f0, i1, f1, ires, res);
  return ires;
}

sc_biguint<DATA_BW> relay_sim::bv_multiply(sc_biguint<DATA_BW> op0, sc_biguint<DATA_BW> op1)
{
  unsigned int i0 = op0.to_uint(), i1 = op1.to_uint();
  float f0 = (*(float*)&i0), f1 = (*(float*)&i1);
  
  float res = f0 * f1;
  unsigned int ires = *(int*)&res;
  // printf("bvmul 0x%08x (%.6f) * 0x%08x (%.6f) = 0x%08x (%.6f)\n", i0, f0, i1, f1, ires, res);
  return ires;
}

sc_biguint<8> relay_sim::signed_gt(sc_biguint<8> arg_0, sc_biguint<8> arg_1) {
  int arg_0_int = arg_0.to_int();
  int arg_1_int = arg_1.to_int();

  bool sb_0 = arg_0_int >> 7;
  bool sb_1 = arg_1_int >> 7;

  int abs_0 = arg_0_int & 127;
  int abs_1 = arg_1_int & 127;

  sc_biguint<8> result;

  if (sb_0 && sb_1) {
    result = (abs_0 < abs_1) ? arg_0 : arg_1;
  } else if (sb_0 || sb_1) {
    result = (sb_1) ? arg_0 : arg_1; 
  } else {
    result = (abs_0 > abs_1) ? arg_0 : arg_1;
  }
  
  return result;
}
