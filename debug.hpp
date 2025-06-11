#pragma once
#include <cstddef>

namespace Compiler {
class DebugInfo {
public:
  const size_t tokenIndex;
  DebugInfo(size_t tokenIndex);
  DebugInfo(const DebugInfo &) = default;
  virtual ~DebugInfo() = default;
};

} // namespace Compiler
