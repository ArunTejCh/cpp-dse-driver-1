/*
  Copyright (c) 2014-2017 DataStax

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifndef __CASS_RESULT_RESPONSE_HPP_INCLUDED__
#define __CASS_RESULT_RESPONSE_HPP_INCLUDED__

#include "constants.hpp"
#include "data_type.hpp"
#include "macros.hpp"
#include "result_metadata.hpp"
#include "response.hpp"
#include "row.hpp"
#include "string_ref.hpp"
#include "vector.hpp"

namespace cass {

class ResultIterator;

class ResultResponse : public Response {
public:
  typedef SharedRefPtr<ResultResponse> Ptr;
  typedef SharedRefPtr<const ResultResponse> ConstPtr;
  typedef Vector<size_t> PKIndexVec;

  ResultResponse()
      : Response(CQL_OPCODE_RESULT)
      , kind_(CASS_RESULT_KIND_VOID)
      , has_more_pages_(false)
      , row_count_(0) {
    first_row_.set_result(this);
  }

  int32_t kind() const { return kind_; }

  bool has_more_pages() const { return has_more_pages_; }

  int32_t column_count() const { return (metadata_ ? metadata_->column_count() : 0); }

  bool no_metadata() const { return !metadata_; }

  const ResultMetadata::Ptr& metadata() const { return metadata_; }

  void set_metadata(ResultMetadata* metadata) {
    metadata_.reset(metadata);
    decode_first_row();
  }

  const ResultMetadata::Ptr& result_metadata() const { return result_metadata_; }

  StringRef paging_state() const { return paging_state_; }
  StringRef prepared() const { return prepared_; }
  StringRef keyspace() const { return keyspace_; }
  StringRef table() const { return table_; }

  int protocol_version() const { return decoder_.protocol_version(); }
  Decoder& decoder() { return decoder_; }

  int32_t row_count() const { return row_count_; }

  const Row& first_row() const { return first_row_; }

  const PKIndexVec& pk_indices() const { return pk_indices_; }

  virtual bool decode(Decoder& decoder);

private:
  bool decode_metadata(Decoder& decoder, ResultMetadata::Ptr* metadata,
                       bool has_pk_indices = false);

  bool decode_first_row();

  bool decode_rows(Decoder& decoder);

  bool decode_set_keyspace(Decoder& decoder);

  bool decode_prepared(Decoder& decoder);

  bool decode_schema_change(Decoder& decoder);

private:
  int32_t kind_;
  bool has_more_pages_; // row data
  ResultMetadata::Ptr metadata_;
  ResultMetadata::Ptr result_metadata_;
  StringRef paging_state_; // row paging
  StringRef prepared_; // prepared result
  StringRef change_; // schema change
  StringRef keyspace_; // rows, set keyspace, and schema change
  StringRef table_; // rows, and schema change
  int32_t row_count_;
  Decoder decoder_;
  Row first_row_;
  PKIndexVec pk_indices_;

private:
  DISALLOW_COPY_AND_ASSIGN(ResultResponse);
};

} // namespace cass

EXTERNAL_TYPE(cass::ResultResponse, CassResult)

#endif
