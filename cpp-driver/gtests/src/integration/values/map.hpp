/*
  Copyright (c) 2017 DataStax, Inc.

  This software can be used solely with DataStax Enterprise. Please consult the
  license at http://www.datastax.com/terms/datastax-dse-driver-license-terms
*/

#ifndef __TEST_MAP_HPP__
#define __TEST_MAP_HPP__
#include "value_interface.hpp"
#include "test_utils.hpp"

namespace test {
namespace driver {

/**
 * Map wrapped value
 */
template<typename K, typename V>
class Map : public Collection, KeyValueInterface<K, V> {
public:
  Map(const std::map<K, V>& map)
    : Collection(CASS_COLLECTION_TYPE_MAP, map.size())
    , map_(map) {
    // Create the collection
    for (typename std::map<K, V>::const_iterator iterator = map.begin();
      iterator != map.end(); ++iterator) {
      K key = iterator->first;
      V value = iterator->second;
      Collection::append<K>(key);
      Collection::append<V>(value);
      primary_sub_type_ = key.value_type();
      secondary_sub_type_ = value.value_type();
    }
  }

  Map(const CassValue* value)
    : Collection(CASS_COLLECTION_TYPE_MAP) {
    initialize(value);
  }

  void append(Collection collection) {
    Collection::append(collection);
  }

  std::string cql_type() const {
    typename std::map<K, V>::const_iterator iterator = map_.begin();
    std::string cql_type = "map<" + iterator->first.cql_type() + ", "
      + iterator->second.cql_type() + ">";
    return cql_type;
  }

  std::string cql_value() const {
    return str();
  }

  CassCollectionType collection_type() const {
    return collection_type_;
  }

  void set(Tuple tuple, size_t index) {
    Collection::set(tuple, index);
  }

  void set(UserType user_type, const std::string& name) {
    Collection::set(user_type, name);
  }

  /**
   * Get the size of the map
   *
   * @return The number of key/value in the map
   */
  size_t size() const {
    return map_.size();
  }

  void statement_bind(Statement statement, size_t index) {
    if (is_null_) {
      ASSERT_EQ(CASS_OK, cass_statement_bind_null(statement.get(), index));
    } else {
      ASSERT_EQ(CASS_OK,
        cass_statement_bind_collection(statement.get(), index, get()));
    }
  }

  bool is_null() const {
    return is_null_;
  }

  std::vector<K> keys() const {
    std::vector<K> keys;
    for (typename std::map<K, V>::const_iterator iterator = map_.begin();
      iterator != map_.end(); ++iterator) {
      keys.push_back(iterator->first);
    }
    return keys;
  }

  CassValueType key_type() const {
    return secondary_sub_type_;
  }

  std::string str() const {
    if (is_null_) {
      return "null";
    } else if (map_.empty()) {
      return "{}";
    } else {
      std::stringstream map_string;
      map_string << "{";
      for (typename std::map<K, V>::const_iterator iterator = map_.begin();
        iterator != map_.end(); ++iterator) {
        K key = iterator->first;
        V value = iterator->second;
        map_string << key.cql_value() << ":" << value.cql_value();

        // Add a comma separation to the list (unless the last element)
        if (iterator != --map_.end()) {
          map_string << ", ";
        }
      }
      map_string << "}";
      return map_string.str();
    }
  }

  std::map<K, V> value() const {
    return map_;
  }

  std::vector<V> values() const {
    std::vector<V> values;
    for (typename std::map<K, V>::const_iterator iterator = map_.begin();
      iterator != map_.end(); ++iterator) {
      values.push_back(iterator->second);
    }
    return values;
  }

  CassValueType value_type() const {
    return primary_sub_type_;
  }

private:
  /**
   * Key/Value used in the map
   */
  std::map<K, V> map_;

  void initialize(const CassValue* value) {
    // Call the parent class
    Collection::initialize(value);

    // Add the values to the map
    const CassValue* current_value = next();
    while (current_value) {
      K key = K(current_value);
      current_value = next();
      V value = V(current_value);
      map_.insert(std::pair<K, V>(key, value));
      current_value = next();
    }
  }
};

template <class K, class V>
inline std::ostream& operator<<(std::ostream& os, const Map<K, V>& map) {
  os << map.cql_value();
  return os;
}

} // namespace driver
} // namespace test

#endif // __TEST_MAP_HPP__
