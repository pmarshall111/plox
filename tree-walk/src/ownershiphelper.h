#ifndef TREEWALK_OWNERSHIP_HELPER_H
#define TREEWALK_OWNERSHIP_HELPER_H

#include <memory>
#include <string>
#include <variant>

namespace plox {
namespace treewalk {

// A class responsible for switching between shared and weak pointers depending
// on who owns the component.

template <typename T> class OwnershipHelper {
public:
  // Constructors
  OwnershipHelper(std::shared_ptr<T> strong);
  OwnershipHelper(std::weak_ptr<T> weak);

  OwnershipHelper(const OwnershipHelper &) = delete;
  OwnershipHelper &operator=(const OwnershipHelper &) = delete;
  OwnershipHelper(OwnershipHelper &&) noexcept = default;
  OwnershipHelper &operator=(OwnershipHelper &&) noexcept = default;

  // Operations
  std::shared_ptr<T> getStrong() const;
  bool becomeOwner();
  bool becomeBorrower();
  void hold(std::shared_ptr<T> strong);
  void hold(std::weak_ptr<T> weak);

  // Operators
  bool operator==(const OwnershipHelper &other) const;
  bool operator<(const OwnershipHelper &other) const;
  bool operator!=(const OwnershipHelper &other) const;
  bool operator>(const OwnershipHelper &other) const;
  bool operator<=(const OwnershipHelper &other) const;
  bool operator>=(const OwnershipHelper &other) const;

private:
  std::variant<std::shared_ptr<T>, std::weak_ptr<T>> d_value;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const OwnershipHelper<T> &fn);

// Definitions

template <typename T>
OwnershipHelper<T>::OwnershipHelper(std::shared_ptr<T> strong)
    : d_value(strong) {}

template <typename T>
OwnershipHelper<T>::OwnershipHelper(std::weak_ptr<T> weak) : d_value(weak) {}

template <typename T> std::shared_ptr<T> OwnershipHelper<T>::getStrong() const {
  if (std::holds_alternative<std::shared_ptr<T>>(d_value)) {
    return std::get<std::shared_ptr<T>>(d_value);
  }

  auto weak = std::get<std::weak_ptr<T>>(d_value);
  return weak.lock();
}

template <typename T> bool OwnershipHelper<T>::becomeOwner() {
  if (std::holds_alternative<std::shared_ptr<T>>(d_value)) {
    return true;
  }

  auto weak = std::get<std::weak_ptr<T>>(d_value);
  if (auto strong = weak.lock()) {
    d_value = strong;
    return true;
  }
  return false;
}

template <typename T> bool OwnershipHelper<T>::becomeBorrower() {
  if (std::holds_alternative<std::shared_ptr<T>>(d_value)) {
    d_value = std::weak_ptr<T>(std::get<std::shared_ptr<T>>(d_value));
    return true;
  }

  return true;
}

template <typename T> void OwnershipHelper<T>::hold(std::shared_ptr<T> strong) {
  d_value = strong;
}

template <typename T> void OwnershipHelper<T>::hold(std::weak_ptr<T> weak) {
  d_value = weak;
}

template <typename T>
bool OwnershipHelper<T>::operator==(const OwnershipHelper &other) const {
  // N.B. This will result in 2 expired weak_ptrs being equal even if they
  // originally pointed to different things.
  return getStrong() == other.getStrong();
}

template <typename T>
bool OwnershipHelper<T>::operator<(const OwnershipHelper &other) const {
  return getStrong() < other.getStrong();
}

template <typename T>
bool OwnershipHelper<T>::operator!=(const OwnershipHelper<T> &other) const {
  return !(*this == other);
}

template <typename T>
bool OwnershipHelper<T>::operator>(const OwnershipHelper<T> &other) const {
  return other < *this;
}

template <typename T>
bool OwnershipHelper<T>::operator<=(const OwnershipHelper<T> &other) const {
  return !(*this > other);
}

template <typename T>
bool OwnershipHelper<T>::operator>=(const OwnershipHelper<T> &other) const {
  return !(*this < other);
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const OwnershipHelper<T> &ownHlpr) {
  // Component should be invisible to client
  os << *ownHlpr.getStrong();
  return os;
}

} // namespace treewalk
} // namespace plox

#endif