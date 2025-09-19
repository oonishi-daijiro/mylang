
#include <cstddef>
#include <cstring>
#include <string>
#include <utility>

template <typename T, size_t N> struct array_expr {
  consteval array_expr(const T (&p)[N]) {
    for (size_t i = 0; i < N; i++)
      buf[i] = p[i];
  };
  T buf[N];
  static constexpr auto size = N;
};

template <size_t N> using strexpr = array_expr<char, N>;
template <strexpr str, size_t key> struct string_enum_elm_handle {};

template <typename Seq, strexpr... strs> struct string_enum_key_impl;

template <size_t... I, strexpr... strs>
struct string_enum_key_impl<std::integer_sequence<size_t, I...>, strs...>
    : public string_enum_elm_handle<strs, I>... {};

template <strexpr... strs>
struct string_enum_key
    : public string_enum_key_impl<std::make_index_sequence<sizeof...(strs)>,
                                  strs...> {
  template <strexpr s, size_t I>
  constexpr auto cast(string_enum_elm_handle<s, I>) const {
    return I;
  }

public:
  template <strexpr s> constexpr auto get() const { return cast<s>(*this); }
};
template <strexpr... strs> class string_enum;

template <size_t I, strexpr v, strexpr... enumvals> struct string_enum_elm {
  operator string_enum<enumvals...>() {
    return string_enum<enumvals...>(*this);
  }
};

template <strexpr... strs> class string_enum {
  static constexpr inline string_enum_key<strs...> instance{};

  size_t enumVal{0};
  std::string value{""};

public:
  template <strexpr v>
  static constexpr auto of =
      string_enum_elm<instance.template get<v>(), v, strs...>{};

public:
  template <strexpr... any> string_enum(const string_enum<any...> &) = delete;
  template <strexpr... any> string_enum(string_enum<any...> &&) = delete;

  string_enum(const string_enum<strs...> &rv)
      : enumVal{rv.enumVal}, value{rv.value} {};

  string_enum(string_enum<strs...> &&rv)
      : enumVal{rv.enumVal}, value{rv.value} {};

  string_enum() = default;

  template <size_t I, strexpr S>
  string_enum(string_enum_elm<I, S, strs...> &&elm)
      : enumVal(instance.template get<S>()), value(S.buf) {}

  template <size_t I, strexpr S>
  string_enum(const string_enum_elm<I, S, strs...> &elm)
      : enumVal(instance.template get<S>()), value(S.buf) {}

  template <size_t I, strexpr v>
  bool operator==(const string_enum_elm<I, v, strs...> &rv) {
    return this->enumVal == I;
  }

  template <size_t I, strexpr v>
  bool operator==(string_enum_elm<I, v, strs...> &&rv) const {
    return this->enumVal == I;
  }

  bool operator==(const string_enum<strs...> &rv) const {
    return this->enumVal == rv.enumVal;
  }

  template <size_t I, strexpr v>
  bool operator==(string_enum<strs...> &&rv) const {
    return this->enumVal == rv.enumVal;
  }

  template <size_t I, strexpr v>
  auto operator=(const string_enum_elm<I, v, strs...> &rv) {
    this->enumVal = I;
    this->value = v.buf;
    return *this;
  }

  template <size_t I, strexpr v>
  auto operator=(string_enum_elm<I, v, strs...> &&rv) {
    this->enumVal = I;
    this->value = v.buf;
    return *this;
  }

  auto operator=(const string_enum<strs...> &rv) {
    this->enumVal = rv.enumVal;
    this->value = rv.value;
    return *this;
  }

  // template <size_t I, strexpr v> auto operator=(string_enum<strs...> &&rv) {
  //   return *this;
  // }

  const std::string to_string() const { return value; }
};
