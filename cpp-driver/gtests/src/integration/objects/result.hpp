/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef __TEST_RESULT_HPP__
#define __TEST_RESULT_HPP__
#include "cassandra.h"

#include "objects/object_base.hpp"

#include "objects/iterator.hpp"
#include "objects/future.hpp"

#include "exception.hpp"

#include <string>

#include <gtest/gtest.h>

namespace test {
namespace driver {

// Forward declarations
class Row;
class Rows;

/**
 * Wrapped result object
 */
class Result : public Object<const CassResult, cass_result_free> {
public:
  class Exception : public test::Exception {
  public:
    Exception(const std::string& message)
      : test::Exception(message) {}
  };

  /**
   * Create an empty result object
   */
  Result() {}

  /**
   * Create the result object from the native driver object
   *
   * @param result Native driver object
   */
  Result(const CassResult* result)
    : Object<const CassResult, cass_result_free>(result) {}

  /**
   * Create the result object from a shared reference
   *
   * @param result Shared reference
   */
  Result(Ptr result)
    : Object<const CassResult, cass_result_free>(result) {}

  /**
   * Create the result object from a future object
   *
   * @param future Wrapped driver object
   */
  Result(Future future)
    : Object<const CassResult, cass_result_free>(future.result())
    , future_(future) {}

  /**
   * Get the attempted host/address of the future
   *
   * @return Attempted host/address
   */
  const std::vector<std::string> attempted_hosts() {
    return future_.attempted_hosts();
  }

  /**
   * Get the error code from the future
   *
   * @return Error code of the future
   */
  CassError error_code() {
    return future_.error_code();
  }

  /**
   * Get the human readable description of the error code
   *
   * @return Error description
   */
  const std::string error_description() {
    return future_.error_description();
  }

  /**
   * Get the error message of the future if an error occurred
   *
   * @return Error message
   */
  const std::string error_message() {
    return future_.error_message();
  }

  /**
   * Get the host/address of the future
   *
   * @return Host/Address
   */
  const std::string host() {
    return future_.host();
  }

  /**
   * Get the number of columns from the result
   *
   * @return The number of columns in the result
   */
  size_t column_count() {
    return cass_result_column_count(get());
  }

  /**
   * Get the column names from the result
   *
   * @return The names of the columns
   */
  std::vector<std::string> column_names() {
    std::vector<std::string> column_names;
    for (size_t i = 0; i < column_count(); ++i) {
      const char* string;
      size_t length;
      if (CASS_OK == cass_result_column_name(get(), i, &string, &length)) {
        column_names.push_back(std::string(string, length));
      }
    }
    return column_names;
  }

  /**
   * Get the first row from the result
   *
   * @return The first row
   */
  Row first_row();

  /**
   * Get the number of rows from the result
   *
   * @return The number of rows in the result
   */
  size_t row_count() {
    return cass_result_row_count(get());
  }

  /**
   * Get the rows from the result
   *
   * @return The rows from the result
   */
  Rows rows();

private:
  /**
   * Future wrapped object
   */
  Future future_;
};

/**
 * Column object
 */
class Column {
public:
  /**
   * Create a column from a value
   *
   * @param value Native driver value
   * @param parent The result object for the column
   */
  Column(const CassValue* value, const Result& parent)
    : value_(value)
    , parent_(parent) {}

  /**
   * Get the value as a specific type
   *
   * @return The value as a value type
   */
  template<class T>
  T as() const {
    return T(value_);
  }

private:
  /**
   * The value held by this column
   */
  const CassValue* value_;
  /**
   * Parent result object
   */
  Result parent_;
};

/**
 * Wrapped row object
 */
class Row {
public:
  /**
   * Create a value from a wrapped row object
   *
   * @param row Native driver row
   * @param parent The result object for the row
   */
  Row(const CassRow* row, const Result& parent)
    : iterator_(cass_iterator_from_row(row))
    , parent_(parent) {}

  /**
   * Get the total number of columns in a row
   *
   * @return The total number of columns in a row
   */
  size_t column_count() {
    return parent_.column_count();
  }

  /**
   * Get the next column
   *
   * @return The next column
   * @throws Exception if there are no more columns available
   */
  const Column next() {
    if (cass_iterator_next(iterator_.get())) {
      return Column(cass_iterator_get_column(iterator_.get()), parent_);
    }
    throw Exception("No more columns available");
  }

private:
  /**
   * Iterator driver wrapped object
   */
  Iterator iterator_;
  /**
   * Parent result object
   */
  Result parent_;
};

/**
 * Rows object
 */
class Rows {
public:
  /**
   * Create the rows object from a wrapped result object
   *
   * @param iterator Wrapped iterator object
   * @param parent The result object for these rows
   */
  Rows(Iterator iterator, Result parent)
    : iterator_(iterator)
    , parent_(parent) {}

  /**
   * Get the total number of columns in a row
   *
   * @return The total number of columns in a row
   */
  size_t column_count() {
    return parent_.column_count();
  }

  /**
   * Get the total number of rows
   *
   * @return The total number of rows
   */
  size_t row_count() {
    return parent_.row_count();
  }

  /**
   * Get the next row
   *
   * @return The next row
   * @throws Exception if there are no more rows available
   */
  const Row next() {
    if (cass_iterator_next(iterator_.get())) {
      return Row(cass_iterator_get_row(iterator_.get()), parent_);
    }
    throw Exception("No more rows available");
  }

private:
  /**
   * Iterator driver wrapped object
   */
  Iterator iterator_;
  /**
   * Parent result object
   */
  Result parent_;
};


inline Row Result::first_row() {
  if (cass_result_row_count(get()) == 0) {
    throw Exception("No first row available");
  }
  return Row(cass_result_first_row(get()), *this);
}

inline Rows Result::rows() {
  return Rows(cass_iterator_from_result(get()), *this);
}

} // namespace driver
} // namespace test

#endif // __TEST_RESULT_HPP__
