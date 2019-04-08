/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_COMPILER_XLA_CLIENT_LIB_PRNG_H_
#define TENSORFLOW_COMPILER_XLA_CLIENT_LIB_PRNG_H_

#include <array>

#include "tensorflow/compiler/xla/client/xla_builder.h"
#include "tensorflow/compiler/xla/xla_data.pb.h"

namespace xla {

// Records the bits and state generated by a random number generator.
struct RngOutput {
  XlaOp value;
  XlaOp state;
};

// A BitGenerator returns random bits and updated random bit generator state.
//
// key: is a value input to a random number generator that can affect the
//   sequence of number it will generate. A random number generator constructs
//   its seed using the key and the initial state. The tf2xla bridge passes the
//   seed operand of a tensorflow random operation as a key to the random bit
//   generator, for example.
// initial_state: initial_state is the initial state of the current random
//   number generation. It could be 0 for a stateless random operation, and
//   the returned state from a previous execution for a stateful random
//   operation.
// shape: the shape of the random bits.
using BitGeneratorTy = std::function<RngOutput(XlaOp key, XlaOp initial_state,
                                               const xla::Shape& shape)>;

// Implements the ThreeFry counter-based PRNG algorithm.
// Salmon et al. SC 2011. Parallel random numbers: as easy as 1, 2, 3.
// http://www.thesalmons.org/john/random123/papers/random123sc11.pdf
RngOutput ThreeFryBitGenerator(XlaOp key, XlaOp initial_state,
                               const xla::Shape& shape);

// Uses the given bit generator to generate random bits and then converts the
// random bits to random numbers of uniform distribution in the given range.
// Returns the random numbers and the state of the random number generator.
// This function is for shape with float element type.
RngOutput UniformF32Distribution(XlaOp key, XlaOp initial_state,
                                 BitGeneratorTy bit_generator, XlaOp minval,
                                 XlaOp maxval, const xla::Shape& shape);

// Similar to UniformF32Distribution but for shape with integer element types.
RngOutput UniformIntDistribution(XlaOp key, XlaOp initial_state,
                                 BitGeneratorTy bit_generator, XlaOp minval,
                                 XlaOp maxval, const xla::Shape& shape);

// Uses the given bit generator to generate random bits and then converts the
// random bits to random numbers of normal distribution.
// Returns the random numbers and the state of the random number generator.
RngOutput NormalF32Distribution(XlaOp key, XlaOp initial_state,
                                BitGeneratorTy bit_generator,
                                const xla::Shape& shape);

}  // namespace xla

#endif  // TENSORFLOW_COMPILER_XLA_CLIENT_LIB_PRNG_H_
