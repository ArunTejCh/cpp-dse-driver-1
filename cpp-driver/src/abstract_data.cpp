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

#include "abstract_data.hpp"

#include "collection.hpp"
#include "constants.hpp"
#include "request.hpp"
#include "tuple.hpp"
#include "user_type_value.hpp"

namespace cass {

CassError AbstractData::set(size_t index, CassNull value) {
  CASS_CHECK_INDEX_AND_TYPE(index, value);
  elements_[index] = Element(value);
  return CASS_OK;
}

CassError AbstractData::set(size_t index, const Collection* value) {
  CASS_CHECK_INDEX_AND_TYPE(index, value);
  if (value->type() == CASS_COLLECTION_TYPE_MAP &&
      value->items().size() % 2 != 0) {
    return CASS_ERROR_LIB_INVALID_ITEM_COUNT;
  }
  elements_[index] = value;
  return CASS_OK;
}

CassError AbstractData::set(size_t index, const Tuple* value) {
  CASS_CHECK_INDEX_AND_TYPE(index, value);
  elements_[index] = value->encode_with_length();
  return CASS_OK;
}

CassError AbstractData::set(size_t index, const UserTypeValue* value) {
  CASS_CHECK_INDEX_AND_TYPE(index, value);
  elements_[index] = value->encode_with_length();
  return CASS_OK;
}

Buffer AbstractData::encode() const {
  Buffer buf(get_buffers_size());
  encode_buffers(0, &buf);
  return buf;
}

Buffer AbstractData::encode_with_length() const {
  size_t buffers_size = get_buffers_size();
  Buffer buf(sizeof(int32_t) + buffers_size);

  size_t pos = buf.encode_int32(0, buffers_size);
  encode_buffers(pos, &buf);

  return buf;
}

size_t AbstractData::get_buffers_size() const {
  size_t size = 0;
  for (ElementVec::const_iterator i = elements_.begin(),
       end = elements_.end(); i != end; ++i) {
    if (!i->is_unset()) {
      size += i->get_size(CASS_HIGHEST_SUPPORTED_PROTOCOL_VERSION);
    } else {
      size += sizeof(int32_t); // null
    }
  }
  return size;
}

void AbstractData::encode_buffers(size_t pos, Buffer* buf) const {
  for (ElementVec::const_iterator i = elements_.begin(),
       end = elements_.end(); i != end; ++i) {
    if (!i->is_unset()) {
      pos = i->copy_buffer(CASS_HIGHEST_SUPPORTED_PROTOCOL_VERSION, pos, buf);
    } else {
      pos = buf->encode_int32(pos, -1); // null
    }
  }
}

size_t AbstractData::Element::get_size(int version) const {
  if (type_ == COLLECTION) {
    return collection_->get_size_with_length(version);
  } else {
    assert(type_ == BUFFER || type_ == NUL);
    return buf_.size();
  }
}

size_t AbstractData::Element::copy_buffer(int version, size_t pos, Buffer* buf) const {
  if (type_ == COLLECTION) {
    Buffer encoded(collection_->encode_with_length(version));
    return buf->copy(pos, encoded.data(), encoded.size());
  } else {
    assert(type_ == BUFFER || type_ == NUL);
    return buf->copy(pos, buf_.data(), buf_.size());
  }
}

Buffer AbstractData::Element::get_buffer_cached(int version, Request::EncodingCache* cache, bool add_to_cache) const {
  if (type_ == COLLECTION) {
    Request::EncodingCache::const_iterator i = cache->find(collection_.get());
    if (i != cache->end()) {
      return i->second;
    } else {
      Buffer buf(collection_->encode_with_length(version));
      if (add_to_cache) {
        // TODO: Is there a size threshold where it might be faster to alway re-encode?
        cache->insert(std::make_pair(collection_.get(), buf));
      }
      return buf;
    }
  } else {
    assert(type_ == BUFFER || type_ == NUL);
    return buf_;
  }
}

} // namespace cass
