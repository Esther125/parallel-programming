#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  __pp_vec_float x;
  __pp_vec_int y;
  __pp_vec_int count;

  __pp_vec_float result;
  __pp_vec_int zero = _pp_vset_int(0);
  __pp_vec_int one_int = _pp_vset_int(1);
  __pp_vec_float limit = _pp_vset_float(9.999999f);  
  
  // Masks
  __pp_mask maskAll; // Active Lane
  __pp_mask maskEqZero; // y == 0
  __pp_mask maskIsNotZero; // y != 0
  __pp_mask maskGtZero; // count > 0
  __pp_mask maskClamps; // result > 9.999999

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    // Handle the tail case: compute how many elements remain in this batch
    int active = (N-i < VECTOR_WIDTH) ? (N-i) : VECTOR_WIDTH;
    
    maskAll = _pp_init_ones(active); // All ones
    maskEqZero = _pp_init_ones(0); // All zeros

    // Load vector of values
    _pp_vload_float(x, values + i, maskAll); // x = values[i];
    _pp_vload_int(y, exponents + i, maskAll); // y = exponents[i];

    // if (y == 0)
    _pp_veq_int(maskEqZero, y, zero, maskAll);
    _pp_vset_float(result, 1.f, maskEqZero); // output = 1.f

    _pp_vset_int(count, 0, maskEqZero);

    // else
    maskIsNotZero = _pp_mask_not(maskEqZero);
    maskIsNotZero = _pp_mask_and(maskIsNotZero, maskAll);
    _pp_vmove_float(result, x, maskIsNotZero);
    _pp_vsub_int(count, y, one_int, maskIsNotZero); // count = y - 1;

    // if (count > 0)
    _pp_vgt_int(maskGtZero ,count, zero, maskAll);
    while(_pp_cntbits(maskGtZero) > 0){
      _pp_vmult_float(result, result, x, maskGtZero); // result *= x
      _pp_vsub_int(count, count, one_int, maskGtZero); // count--
      // Update mask
      _pp_vgt_int(maskGtZero, count, zero, maskAll);
    }

    // Clamps the result at 9.999999
    _pp_vgt_float(maskClamps, result, limit, maskAll); // if (result > 9.999999f) 
    _pp_vset_float(result, 9.999999f, maskClamps); // result = 9.999999f
    
    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //
  __pp_vec_float acc;
  __pp_vec_float tmp;
  
  __pp_mask maskAll;

  // Initialized
  maskAll = _pp_init_ones();
  _pp_vset_float(acc, 0.f, maskAll);
  
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    _pp_vload_float(tmp, values+i, maskAll); 
    _pp_vadd_float(acc, acc, tmp, maskAll);
  }
  
  int width = VECTOR_WIDTH;
  while(width > 1){
    _pp_hadd_float(acc, acc);
    _pp_interleave_float(acc, acc);
    width = width/2;
  }

  float result[VECTOR_WIDTH];
  _pp_vstore_float(result, acc, maskAll);
  
  return result[0];
}