#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <JSON/json.hpp>

#include <MetaStuff/Meta.h>
#include "StringCast.h"

namespace json = nlohmann;

template <typename T>
void to_json(json::json& j, const T& obj);

template <typename T>
void from_json(const json::json& j, T& obj);

namespace meta
{

  /////////////////// SERIALIZATION

  template <typename Class,
    typename = std::enable_if_t <meta::isRegistered<Class>()>>
    json::json serialize(const Class& obj);

  template <typename Class,
    typename = std::enable_if_t <!meta::isRegistered<Class>()>,
    typename = void>
    json::json serialize(const Class& obj);

  template <typename Class>
  json::json serialize_basic(const Class& obj);

  // specialization for std::vector
  template <typename T>
  json::json serialize_basic(const std::vector<T>& obj);

  // specialization for std::unodered_map
  template <typename K, typename V>
  json::json serialize_basic(const std::unordered_map<K, V>& obj);


  /////////////////// DESERIALIZATION
  //
  //template<typename Class>
  //Class deserialize(const json::json& obj);

  template <typename Class,
    typename = std::enable_if_t<meta::isRegistered<Class>()>>
    void deserialize(Class& obj, const json::json& object);

  template <typename Class,
    typename = std::enable_if_t<!meta::isRegistered<Class>()>,
    typename = void>
    void deserialize(Class& obj, const json::json& object);

  // specialization for std::vector
  template <typename T>
  void deserialize(std::vector<T>& obj, const json::json& object);

  // specialization for std::unodered_map
  template <typename K, typename V>
  void deserialize(std::unordered_map<K, V>& obj, const json::json& object);

}

#include "JsonCast.inl"