#include "systemc.h"
#include "relay_sim.h"

#include <math.h>

// floating point number
#define DATA_BW 32
#define DATA_BYTES 4

/** floating point operations **/
sc_biguint<DATA_BW> relay_sim::bv_tanh(sc_biguint<DATA_BW> op0)
{
  return tanh(op0.to_double());
}

sc_biguint<DATA_BW> relay_sim::bv_sigmoid(sc_biguint<DATA_BW> op0)
{
  return 1.0 / (exp(-op0.to_double()) + 1);
}

sc_biguint<DATA_BW> relay_sim::bv_add(sc_biguint<DATA_BW> op0, sc_biguint<DATA_BW> op1)
{
  return op0.to_double() + op1.to_double();
}

sc_biguint<DATA_BW> relay_sim::bv_multiply(sc_biguint<DATA_BW> op0, sc_biguint<DATA_BW> op1)
{
  return op0.to_double() * op1.to_double();
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
