#pragma once

#include <c10/macros/Macros.h>
#include <c10/util/Exception.h>
#include <c10/util/Optional.h>
#include <c10/util/string_view.h>
#include <string>
#include <utility>
#include <ostream>

namespace c10 {

// TODO: consider storing namespace separately too
struct OperatorName final {
  std::string name;
  std::string overload_name;
  OperatorName(std::string name, std::string overload_name)
      : name(std::move(name)), overload_name(std::move(overload_name)) {}

  // TODO: These two functions below are slow!  Fix internal data structures so
  // I don't have to manually reconstruct the namespaces!

  // Return the namespace of this OperatorName, if it exists.  The
  // returned string_view is only live as long as the OperatorName
  // exists and name is not mutated
  c10::optional<c10::string_view> getNamespace() const {
    auto pos = name.find("::");
    if (pos == std::string::npos) {
      return c10::nullopt;
    } else {
      return c10::make_optional(c10::string_view(name.data(), pos));
    }
  }

  // Returns true if we successfully set the namespace
  bool setNamespaceIfNotSet(const char* ns) {
    std::ostringstream oss;
    if (!getNamespace().has_value()) {
      oss << ns << "::" << name;
      name = oss.str();
      return true;
    } else {
      return false;
    }
  }
};

inline bool operator==(const OperatorName& lhs, const OperatorName& rhs) {
  return lhs.name == rhs.name && lhs.overload_name == rhs.overload_name;
}

inline bool operator!=(const OperatorName& lhs, const OperatorName& rhs) {
  return !operator==(lhs, rhs);
}

CAFFE2_API std::string toString(const OperatorName& opName);
CAFFE2_API std::ostream& operator<<(std::ostream&, const OperatorName&);

} // namespace c10

namespace std {
  template <>
  struct hash<::c10::OperatorName> {
    size_t operator()(const ::c10::OperatorName& x) const {
      return std::hash<std::string>()(x.name) ^ (~ std::hash<std::string>()(x.overload_name));
    }
  };
}
