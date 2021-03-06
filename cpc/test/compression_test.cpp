/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <algorithm>

#include "cpc_compressor.hpp"

namespace datasketches {

class compression_test: public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(compression_test);
  CPPUNIT_TEST(compress_and_uncompress_pairs);
  CPPUNIT_TEST_SUITE_END();

  typedef u32_table<std::allocator<void>> table;

  void compress_and_uncompress_pairs() {
    const int N = 200;
    const int MAXWORDS = 1000;

    HashState twoHashes;
    uint32_t pairArray[N];
    uint32_t pairArray2[N];
    uint64_t value = 35538947; // some arbitrary starting value
    const uint64_t golden64 = 0x9e3779b97f4a7c13ULL; // the golden ratio
    for (int i = 0; i < N; i++) {
      MurmurHash3_x64_128(&value, sizeof(value), 0, twoHashes);
      uint32_t rand = twoHashes.h1 & 0xffff;
      pairArray[i] = rand;
      value += golden64;
    }
    //table::knuth_shell_sort3(pairArray, 0, N - 1); // unsigned numerical sort
    std::sort(pairArray, &pairArray[N]);
    uint32_t prev = UINT32_MAX;
    int nxt = 0;
    for (int i = 0; i < N; i++) { // uniquify
      if (pairArray[i] != prev) {
        prev = pairArray[i];
        pairArray[nxt++] = pairArray[i];
      }
    }
    int numPairs = nxt;

    uint32_t compressedWords[MAXWORDS];

    for (size_t numBaseBits = 0; numBaseBits <= 11; numBaseBits++) {
      size_t numWordsWritten = get_compressor<std::allocator<void>>().low_level_compress_pairs(pairArray, numPairs, numBaseBits, compressedWords);
      get_compressor<std::allocator<void>>().low_level_uncompress_pairs(pairArray2, numPairs, numBaseBits, compressedWords, numWordsWritten);
      for (int i = 0; i < numPairs; i++) {
        CPPUNIT_ASSERT(pairArray[i] == pairArray2[i]);
      }
    }
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(compression_test);

} /* namespace datasketches */
