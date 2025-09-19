#pragma once

#include <format>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
namespace Compiler {
class Type;
class Kind {
  static inline std::map<std::string, Kind *> kindset{};

  struct kind_deleter {
    ~kind_deleter() {
      for (auto &&[_, k] : kindset)
        delete k;
    }
  };

  static inline kind_deleter del{};

public:
  template <typename K>
  static void DefineNewKind(const std::string &kindName, auto &&...kc)
    requires(std::is_base_of_v<Kind, K>)
  {
    if (kindset.count(kindName)) {
      throw std::runtime_error(
          std::format("kind {} is already defined", kindName));
    } else {
      kindset.emplace(kindName, new K{std::forward<decltype(kc)>(kc)...});
    }
  }

  static const Kind &GetKind(const std::string &kindName) {
    if (!kindset.count(kindName)) {
      throw std::runtime_error(std::format("kind {} is not defined", kindName));
    } else {
      return *kindset.at(kindName);
    }
  }

  Kind() = default;
  virtual ~Kind() = default;
  virtual Type construct(Type &ct) = 0;

  template <typename T>
  bool isa()
    requires(std::is_base_of_v<Kind, T>)
  {
    return dynamic_cast<T *>(this) != nullptr;
  }

  template <typename T> T *cast() { return dynamic_cast<T *>(this); }
};

class ArrayKind : public Kind {
public:
  ArrayKind(size_t size) {}
};

class PointerKind : public Kind {};

} // namespace Compiler
