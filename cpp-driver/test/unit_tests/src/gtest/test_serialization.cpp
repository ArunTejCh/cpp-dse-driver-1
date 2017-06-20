/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#include <gtest/gtest.h>

#include "cpp-driver/src/serialization.hpp"
#include "buffer.hpp"

TEST(SerializationTest, DecodeZigZag) {
  ASSERT_EQ(1LL << 63, cass::decode_zig_zag((long) -1));
}

TEST(SerializationTest, DecodeByte) {
  const char input[2] = { -1, 0 };
  uint8_t value = 0;

  const char* pos = cass::decode_byte(&input[0], value);
  ASSERT_EQ(&input[1], pos);
  ASSERT_EQ(std::numeric_limits<uint8_t>::max(), value);
  pos = cass::decode_byte(pos, value);
  ASSERT_EQ(std::numeric_limits<uint8_t>::min(), value);
}

TEST(SerializationTest, DecodeInt8) {
  const char input[2] = { -128, 127 };
  int8_t value = 0;

  const char* pos = cass::decode_int8(&input[0], value);
  ASSERT_EQ(&input[1], pos);
  ASSERT_EQ(std::numeric_limits<int8_t>::min(), value);
  pos = cass::decode_int8(pos, value);
  ASSERT_EQ(std::numeric_limits<int8_t>::max(), value);
}

TEST(SerializationTest, DecodeUInt16) {
  const char input[4] = { -1, -1,
                          0, 0 };
  uint16_t value = 0;

  const char* pos = cass::decode_uint16(&input[0], value);
  ASSERT_EQ(&input[2], pos);
  ASSERT_EQ(std::numeric_limits<uint16_t>::max(), value);
  pos = cass::decode_uint16(pos, value);
  ASSERT_EQ(std::numeric_limits<uint16_t>::min(), value);
}

TEST(SerializationTest, DecodeInt16) {
  const char input[4] = { -128, 0,
                          127, -1 };
  int16_t value = 0;

  // SUCCESS
  const char *pos = cass::decode_int16(&input[0], value);
  ASSERT_EQ(&input[2], pos);
  ASSERT_EQ(std::numeric_limits<int16_t>::min(), value);
  pos = cass::decode_int16(pos, value);
  ASSERT_EQ(std::numeric_limits<int16_t>::max(), value);
}

TEST(SerializationTest, DecodeUInt32) {
  const char input[8] = { -1, -1, -1, -1,
                          0, 0, 0, 0 };
  uint32_t value = 0;

  const char* pos = cass::decode_uint32(&input[0], value);
  ASSERT_EQ(&input[4], pos);
  ASSERT_EQ(std::numeric_limits<uint32_t>::max(), value);
  pos = cass::decode_uint32(pos, value);
  ASSERT_EQ(std::numeric_limits<uint32_t>::min(), value);
}

TEST(SerializationTest, DecodeInt32) {
  const char input[8] = { -128, 0, 0, 0,
                          127, -1, -1, -1 };
  int32_t value = 0;

  const char *pos = cass::decode_int32(&input[0], value);
  ASSERT_EQ(&input[4], pos);
  ASSERT_EQ(std::numeric_limits<int32_t>::min(), value);
  pos = cass::decode_int32(pos, value);
  ASSERT_EQ(std::numeric_limits<int32_t>::max(), value);
}

TEST(SerializationTest, DecodeInt64) {
  const char input[16] = { -128, 0, 0, 0,0, 0, 0, 0,
                           127, -1, -1, -1, -1, -1, -1, -1 };
  int64_t value = 0;

  const char *pos = cass::decode_int64(&input[0], value);
  ASSERT_EQ(&input[8], pos);
  ASSERT_EQ(std::numeric_limits<int64_t>::min(), value);
  pos = cass::decode_int64(pos, value);
  ASSERT_EQ(std::numeric_limits<int64_t>::max(), value);
}

TEST(SerializationTest, DecodeFloat) {
  const char input[8] = { 0, -128, 0, 0,
                          127, 127, -1, -1 };
  float value = 0;

  const char *pos = cass::decode_float(&input[0], value);
  ASSERT_EQ(&input[4], pos);
  ASSERT_EQ(std::numeric_limits<float>::min(), value);
  pos = cass::decode_float(pos, value);
  ASSERT_EQ(std::numeric_limits<float>::max(), value);
}

TEST(SerializationTest, DecodeDouble) {
  const char input[16] = { 0, 16, 0, 0,0, 0, 0, 0,
                           127, -17, -1, -1, -1, -1, -1, -1 };
  double value = 0;

  const char *pos = cass::decode_double(&input[0], value);
  ASSERT_EQ(&input[8], pos);
  ASSERT_EQ(std::numeric_limits<double>::min(), value);
  pos = cass::decode_double(pos, value);
  ASSERT_EQ(std::numeric_limits<double>::max(), value);
}

TEST(SerializationTest, DecodeUuid) {
  const char input[32] = { -1, -1, -1, -1, -1, -1, -1, -1,-1, -1, -1, -1, -1, -1, -1, -1,
                           0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0 };
  CassUuid value;

  const char *pos = cass::decode_uuid(&input[0], &value);
  ASSERT_EQ(&input[16], pos);
  ASSERT_EQ(std::numeric_limits<uint64_t>::max(), value.clock_seq_and_node);
  ASSERT_EQ(std::numeric_limits<uint64_t>::max(), value.time_and_version);
  pos = cass::decode_uuid(pos, &value);
  ASSERT_EQ(std::numeric_limits<uint64_t>::min(), value.clock_seq_and_node);
  ASSERT_EQ(std::numeric_limits<uint64_t>::min(), value.time_and_version);
}

TEST(SerializationTest, DecodeSize) {
  const char input[12] = { -128, 0, 0, 0,
                           0, 0,
                           127, -1, -1, -1,
                           -1, -1 };
  int32_t value = 0;

  const char *pos = cass::decode_size(3, &input[0], value);
  ASSERT_EQ(&input[4], pos);
  ASSERT_EQ(std::numeric_limits<int32_t>::min(), value);
  pos = cass::decode_size(1, pos, value);
  ASSERT_EQ(&input[6], pos);
  ASSERT_EQ(std::numeric_limits<uint16_t>::min(), value);
  pos = cass::decode_size(4, pos, value);
  ASSERT_EQ(&input[10], pos);
  ASSERT_EQ(std::numeric_limits<int32_t>::max(), value);
  pos = cass::decode_size(2, pos, value);
  ASSERT_EQ(std::numeric_limits<uint16_t>::max(), value);
}