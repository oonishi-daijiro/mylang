#pragma once
#include <cctype>
#include <cstddef>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

template <typename T> class TextReader {
protected:
  std::unique_ptr<T[]> bufferPtr;
  size_t strSize;
  int cur = 0;
  const std::function<bool(T)> isSpaceFn;
  using isSpaceFunc_t = std::function<bool(T)>;
  static inline bool defaultIsSpaceFunc(T v) { return std::isblank(v); }

public:
  using character_t = T;
  friend std::ostream &operator<<(std::ostream &, TextReader<char> &);

  TextReader() = delete;

  TextReader(std::basic_string_view<T> string,
             isSpaceFunc_t isSpace = defaultIsSpaceFunc)
      : strSize(string.size()), bufferPtr(std::make_unique<T[]>(string.size())),
        isSpaceFn(isSpace) {
    std::memcpy(bufferPtr.get(), string.data(), strSize);
  };

  TextReader(std::unique_ptr<T> ptr, size_t size,
             isSpaceFunc_t isSpace = defaultIsSpaceFunc)
      : bufferPtr(std::move(ptr)), strSize(size), isSpaceFn(isSpace) {};

  TextReader(const TextReader<T> &) = delete;

  TextReader(TextReader<T> &&src)
      : bufferPtr(std::move(src.bufferPtr)), strSize(src.strSize),
        isSpaceFn(std::move(src.isSpaceFn)) {};

  bool isReachedEnd() const { return cur == strSize; }
  bool isBegin() const { return cur == 0; }

  size_t getCursor() const { return cur; }

  size_t size() { return strSize; }

  const TextReader<T> &operator++() {
    cur = cur + 1 > strSize ? strSize : cur + 1;
    return *this;
  };

  const TextReader<T> &operator--() {
    cur = cur - 1 < 0 ? cur : cur - 1;
    return *this;
  }

  const TextReader<T> &operator+=(size_t N) {
    cur = cur + N > strSize ? strSize : cur + N;
    return *this;
  }

  const TextReader<T> &operator-=(size_t N) {
    cur = cur - N > 0 ? cur - N : 0;
    return *this;
  }

  T operator+(size_t v) {
    return cur + v > strSize ? '\0' : bufferPtr[cur + v];
  }

  T operator-(size_t v) { return cur - v > 0 ? bufferPtr[cur - v] : '\0'; }

  TextReader<T> &operator>>(std::basic_string<T> &v) {
    cur = setSpaceSepStr(v);
    skipws();
    return *this;
  }

  const TextReader<T> &operator>>=(std::basic_string<T> &v) const {
    setSpaceSepStr(v);
    return *this;
  };

  TextReader<T> &operator>>(T &c) {
    c = bufferPtr[cur];
    cur++;
    skipws();
    return *this;
  }

  const TextReader<T> &operator>>=(char &c) const {
    c = bufferPtr[cur];
    return *this;
  };

  const T operator*() {
    if (cur >= strSize) {
      return '\0';
    }
    return bufferPtr[cur];
  }

  void skipws() {
    while (isSpaceFn(bufferPtr[cur])) {
      ++*this;
    }
  }

private:
  // set word to string reference passed.
  // if current cursor points non space character, it sets current word.
  // if current cursor points space character, it skips spaces towards end, and
  // set word after current cursor.
  // [example]
  // hoge hello      --> hoge
  //  ^cursor
  // hoge    hello   --> hello
  //      ^cursor

  int setSpaceSepStr(std::basic_string<T> &v) const {
    int newCursor = cur;

    if (isSpaceFn(bufferPtr[newCursor])) {
      while (newCursor < strSize && isSpaceFn(bufferPtr[newCursor])) {
        newCursor++;
      }
    } else {
      while (newCursor > 0 && !isSpaceFn(bufferPtr[newCursor - 1])) {
        newCursor--;
      }
    }

    int strLen = 0;
    while (newCursor + strLen < strSize &&
           !isSpaceFn(bufferPtr[newCursor + strLen])) {
      strLen++;
    }

    v.resize(strLen);
    std::memcpy(v.data(), &bufferPtr[newCursor], strLen);
    newCursor += strLen;
    return newCursor;
  }
};

template <typename T>
inline std::ostream &operator<<(std::ostream &stream,
                                TextReader<char> &reader) {
  int prefixLen = 0;
  for (size_t i = 0; i < reader.strSize; i++) {
    stream << reader.bufferPtr[i];
    prefixLen++;
    if (reader.bufferPtr[i] == '\n') {
      prefixLen = 0;
    }

    if (i == reader.cur) {
      i++;
      while (reader.bufferPtr[i] != '\n' && i < reader.strSize) {
        stream << reader.bufferPtr[i];
        i++;
      }
      stream << '\n';
      for (int l = 0; l < prefixLen - 1; l++) {
        std::cout << ' ';
      }
      stream << "↑";
    }
  }
  return stream;
}
