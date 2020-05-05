#!/usr/bin/env python3
import tvm
import math
from tvm import relay
from tvm.relay.testing.lstm import lstm_cell, get_workload
import numpy as np
import sys

def generate_random_tensor(ty):
    return tvm.nd.array(np.random.uniform(-1.0, 1.0, tuple([int(i) for i in ty.shape])).astype(ty.dtype))

def main(argv):

    dtype = 'float32'
    num_hidden = int(argv[1])
    batch_size = 1

    input_type = relay.TensorType((batch_size, num_hidden), dtype)
    state_type = relay.TupleType([input_type, input_type])
    weight_type = relay.TensorType((4*num_hidden, num_hidden), dtype)
    bias_type = relay.TensorType((4*num_hidden,), dtype)

    # inputs = relay.Var('inputs', input_type)
    # states = relay.Var('states', state_type)

    # cell_state = relay.Var('cell_state', input_type)
    # hidden_state = relay.Var('hidden_state', input_type)
    # i2h_weight = relay.Var('i2h_weight', weight_type)
    # i2h_bias = relay.Var('i2h_bias', bias_type)
    # h2h_weight = relay.Var('h2h_weight', weight_type)
    # h2h_bias = relay.Var('h2h_bias', bias_type)

    # mod = tvm.IRModule()
    # mod['lstm'] = lstm_cell(num_hidden)
    # mod['main'] = relay.Function([inputs, cell_state, hidden_state, 
    #                                 i2h_weight, i2h_bias, h2h_weight, h2h_bias],
    #                             mod.get_global_var('lstm')(inputs, relay.Tuple([cell_state, hidden_state]),
    #                                 i2h_weight, i2h_bias, h2h_weight, h2h_bias))

    mod, p = get_workload(batch_size, num_hidden)

    ex = relay.create_executor('vm', mod=mod, ctx=tvm.cpu(), target='llvm')

    i_val = generate_random_tensor(input_type)

    cell_val = np.zeros((batch_size, num_hidden), np.float32)
    hidden_val = np.zeros((batch_size, num_hidden), np.float32)

    i2h_w_val = generate_random_tensor(weight_type)
    i2h_b_val = generate_random_tensor(bias_type)
    h2h_w_val = generate_random_tensor(weight_type)
    h2h_b_val = generate_random_tensor(bias_type)


    # order: i_sz, o_sz,  input, cell, hidden, i2h_weight, h2h_weight, i2h_bias, h2h_bias
    f = open(argv[2], 'wb')
    f.write(num_hidden.to_bytes(4, 'little'))
    f.write(num_hidden.to_bytes(4, 'little'))


    i_val.asnumpy().tofile(f)
    cell_val.tofile(f)
    hidden_val.tofile(f)
    i2h_w_val.asnumpy().tofile(f)
    h2h_w_val.asnumpy().tofile(f)
    i2h_b_val.asnumpy().tofile(f)
    h2h_b_val.asnumpy().tofile(f)
    print("Wrote %d bytes" % f.tell())

    print("inputs:", i_val)
    print("cell:", cell_val)
    print("hidden:", hidden_val)
    print("i2h_weights:", i2h_w_val)
    print("h2h_weights:", h2h_w_val)
    print("i2h_bias:", i2h_b_val)
    print("h2h_bias:", h2h_b_val)

    # i2h_dense = np.add(i2h_w_val.asnumpy().dot(i_val.asnumpy()[0]), i2h_b_val.asnumpy())
    # h2h_dense = np.add(h2h_w_val.asnumpy().dot(hidden_val[0]), h2h_b_val.asnumpy())
    # print("i2h dense: ", i2h_dense)
    # print("h2h dense: ", h2h_dense)
    # comb_dense = np.add(i2h_dense, h2h_dense)
    # print("combined dense:", comb_dense)

    # def sig(x):
    #     return (1.0 / (1.0 + math.exp(-x)))
    # vsig = np.vectorize(sig)
    # in_gate = vsig(comb_dense[:num_hidden])
    # forget_gate = vsig(comb_dense[num_hidden:num_hidden*2])
    # in_trx = np.tanh(comb_dense[num_hidden*2:num_hidden*3])
    # out_gate = vsig(comb_dense[num_hidden*3:])
    # next_c = np.add(np.multiply(forget_gate, cell_val), np.multiply(in_gate, in_trx))
    # next_h = np.multiply(out_gate, np.tanh(next_c))
    # print("next_c:", next_c)
    # print("next_h:", next_h)



    out = ex.evaluate()(i_val, i2h_w_val, i2h_b_val, h2h_w_val, h2h_b_val)

    print("output: ", out)
    out.asnumpy().tofile(f)

    print("Wrote %d bytes" % f.tell())
    f.close()


if __name__ == '__main__':
    main(sys.argv)
