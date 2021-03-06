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

#include "utils.hpp"

#include "constants.hpp"

#include <algorithm>
#include <assert.h>
#include <functional>

#if (defined(WIN32) || defined(_WIN32))
  #include <windows.h>
#else
  #include <unistd.h>
#endif

namespace cass {

String opcode_to_string(int opcode) {
  switch (opcode) {
    case CQL_OPCODE_ERROR:
      return "CQL_OPCODE_ERROR";
    case CQL_OPCODE_STARTUP:
      return "CQL_OPCODE_STARTUP";
    case CQL_OPCODE_READY:
      return "CQL_OPCODE_READY";
    case CQL_OPCODE_AUTHENTICATE:
      return "CQL_OPCODE_AUTHENTICATE";
    case CQL_OPCODE_CREDENTIALS:
      return "CQL_OPCODE_CREDENTIALS";
    case CQL_OPCODE_OPTIONS:
      return "CQL_OPCODE_OPTIONS";
    case CQL_OPCODE_SUPPORTED:
      return "CQL_OPCODE_SUPPORTED";
    case CQL_OPCODE_QUERY:
      return "CQL_OPCODE_QUERY";
    case CQL_OPCODE_RESULT:
      return "CQL_OPCODE_RESULT";
    case CQL_OPCODE_PREPARE:
      return "CQL_OPCODE_PREPARE";
    case CQL_OPCODE_EXECUTE:
      return "CQL_OPCODE_EXECUTE";
    case CQL_OPCODE_REGISTER:
      return "CQL_OPCODE_REGISTER";
    case CQL_OPCODE_EVENT:
      return "CQL_OPCODE_EVENT";
    case CQL_OPCODE_BATCH:
      return "CQL_OPCODE_BATCH";
    case CQL_OPCODE_AUTH_CHALLENGE:
      return "CQL_OPCODE_AUTH_CHALLENGE";
    case CQL_OPCODE_AUTH_RESPONSE:
      return "CQL_OPCODE_AUTH_RESPONSE";
    case CQL_OPCODE_AUTH_SUCCESS:
      return "CQL_OPCODE_AUTH_SUCCESS";
  };
  assert(false);
  return "";
}

String protocol_version_to_string(int version) {
  OStringStream ss;
  if (version & DSE_PROTOCOL_VERSION_BIT) {
    ss << "DSEv" << (version & DSE_PROTOCOL_VERSION_MASK);
  } else {
    ss << "v" << version;
  }
  return ss.str();
}

void explode(const String& str, Vector<String>& vec, const char delimiter /* = ',' */) {
  IStringStream stream(str);
  while (!stream.eof()) {
    String token;
    std::getline(stream, token, delimiter);
    if (!trim(token).empty()) {
      vec.push_back(token);
    }
  }
}

String& trim(String& str) {
  // Trim front
  str.erase(str.begin(),
            std::find_if(str.begin(), str.end(),
                         std::not1(std::ptr_fun<int, int>(::isspace))));
  // Trim back
  str.erase(std::find_if(str.rbegin(), str.rend(),
                         std::not1(std::ptr_fun<int, int>(::isspace))).base(),
            str.end());
  return str;
}

static bool is_word_char(int c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
         (c >= '0' && c <= '9') || c == '_';
}

static bool is_lower_word_char(int c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= '0' && c <= '9') || c == '_';
}

bool is_valid_cql_id(const String& str) {
  for (String::const_iterator i = str.begin(),
       end = str.end(); i != end; ++i) {
    if (!is_word_char(*i)) {
      return false;
    }
  }
  return true;
}

bool is_valid_lower_cql_id(const String& str) {
  if (str.empty() || !is_lower_word_char(str[0])) {
    return false;
  }
  if (str.size() > 1) {
    for (String::const_iterator i = str.begin() + 1,
         end = str.end(); i != end; ++i) {
      if (!is_lower_word_char(*i)) {
        return false;
      }
    }
  }
  return true;
}

String& quote_id(String& str) {
  String temp(str);
  str.clear();
  str.push_back('"');
  for (String::const_iterator i = temp.begin(),
       end = temp.end(); i != end; ++i) {
    if (*i == '"') {
      str.push_back('"');
      str.push_back('"');
    } else {
      str.push_back(*i);
    }
  }
  str.push_back('"');

  return str;
}

String& escape_id(String& str) {
  return is_valid_lower_cql_id(str) ?  str : quote_id(str);
}

String& to_cql_id(String& str) {
  if (is_valid_cql_id(str)) {
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    return str;
  }
  if (str.length() > 2 && str[0] == '"' && str[str.length() - 1] == '"') {
    return str.erase(str.length() - 1, 1).erase(0, 1);
  }
  return str;
}

int32_t get_pid()
{
#if (defined(WIN32) || defined(_WIN32))
  return static_cast<int32_t>(GetCurrentProcessId());
#else
  return static_cast<int32_t>(getpid());
#endif
}

} // namespace cass
