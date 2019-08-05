/******************************************************************************/
/*  Probability - A fast implementation of probabilities using logarithms     */
/*  Copyright (C) 2018 Renato Cordeiro Ferreira                               */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/******************************************************************************/

#ifndef MULTIVECTOR_MULTIVECTOR_
#define MULTIVECTOR_MULTIVECTOR_

// Standard headers
#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <type_traits>
#include <vector>

#include <iostream>

namespace multivector {

// Foward declarations
struct Range;
template<typename T, typename Allocator> class MultiVector;
template<typename MultiVector> class View;

struct Range {
  // Instance variables
  size_t begin = 0;
  size_t end = 0;

  // Constructors
  Range() = default;

  explicit Range(size_t p)
      : begin(p), end(p+1) {}

  Range(size_t b, size_t e)
      : begin(b), end(e) {
    assert(begin <= end);
  }

  // Operator overloads
  bool operator==(const Range& rhs) const {
    return begin == rhs.begin && end == rhs.end;
  }

  bool operator!=(const Range& rhs) const {
    return !operator==(rhs);
  }
};

template<typename T, typename Allocator = std::allocator<T>>
class MultiVector {
 public:
  // Aliases
  using range_type = Range;
  using view_type = View<MultiVector>;
  using const_view_type = const View<const MultiVector>;

  using value_type      = T;
  using pointer         = T*;
  using const_pointer   = const T*;
  using reference       = T&;
  using const_reference = const T&;
  using rvalue          = T&&;
  using size_type       = size_t;
  using difference_type = size_t;

  // Constructors
  MultiVector() = default;

  MultiVector(std::initializer_list<size_type> dimensions)
      : dimensions_(dimensions), buffer_(buffer_size()) {
  }

  MultiVector(const T& default_value,
              std::vector<size_type> dimensions)
      : dimensions_(std::move(dimensions)),
        buffer_(buffer_size(), default_value) {
  }

  // Operator overloads
  bool operator==(const MultiVector& rhs) const {
    return dimensions_ == rhs.dimensions_ && buffer_ == rhs.buffer_;
  }

  bool operator!=(const MultiVector& rhs) const {
    return !operator==(rhs);
  }

  const_view_type operator[](std::vector<size_type> indices) const {
    assert(check_num_indices(indices));

    std::vector<range_type> ranges(dimensions_.size() - indices.size());

    std::transform(
        std::begin(dimensions_) + indices.size(),
        std::end(dimensions_),
        std::begin(ranges),
        [](const auto& dimension) { return range_type(0, dimension); });

    return { *this, std::move(indices), std::move(ranges) };
  }

  view_type operator[](std::vector<size_type> indices) {
    assert(check_num_indices(indices));

    std::vector<range_type> ranges(dimensions_.size() - indices.size());

    std::transform(
        std::begin(dimensions_) + indices.size(),
        std::end(dimensions_),
        std::begin(ranges),
        [](const auto& dimension) { return range_type(0, dimension); });

    return { *this, std::move(indices), std::move(ranges) };
  }

  const_view_type operator[](size_type index) const {
    return operator[](std::vector<size_type>{ std::move(index) });
  }

  view_type operator[](size_type index) {
    return operator[](std::vector<size_type>{ std::move(index) });
  }

  // Concrete methods
  size_type num_dimensions() const {
    return dimensions_.size();
  }

  size_type dimension_size(size_type dimension) const {
    assert(check_dimension(dimension));
    return dimensions_[dimension];
  }

  size_type buffer_size() const {
    return std::accumulate(
        std::begin(dimensions_),
        std::end(dimensions_),
        1,
        std::multiplies<>());
  }

  difference_type buffer_offset(const std::vector<size_type>& indices) const {
    assert(check_as_many_indices_as_dimensions(indices));

    size_type offset = 0, jump = 1;
    for (size_type i = 0; i < indices.size(); ++i) {
      size_type k = indices.size()-1 - i;
      assert(check_index(indices, k));

      offset += jump * indices[k];
      jump *= dimensions_[k];
    }

    return offset;
  }

  const_reference buffer_value(difference_type offset) const & {
    assert(check_offset(offset));
    return buffer_[offset];
  }

  reference buffer_value(difference_type offset) & {
    assert(check_offset(offset));
    return buffer_[offset];
  }

  rvalue buffer_value(difference_type offset) && {
    assert(check_offset(offset));
    return std::move(buffer_[offset]);
  }

 private:
  // Instance variables
  std::vector<size_type> dimensions_;
  std::vector<T, Allocator> buffer_;

  // Concrete methods
  bool check_dimension(const size_type& dimension) const {
    return dimension < dimensions_.size();
  }

  bool check_num_indices(const std::vector<size_type>& indices) const {
    return indices.size() > 0 && indices.size() <= num_dimensions();
  }

  bool check_as_many_indices_as_dimensions(
      const std::vector<size_type>& indices) const {
    return indices.size() == dimensions_.size();
  }

  bool check_index(const std::vector<size_type>& indices,
                   const size_type& dimension) const {
    return indices[dimension] < dimensions_[dimension];
  }

  bool check_offset(const difference_type& offset) const {
    return offset < buffer_.size();
  }
};

template<typename MultiVector>
class View {
 public:
  // Aliases
  using range_type = Range;
  using container_type = MultiVector;

  using value_type      = typename container_type::value_type;
  using pointer         = typename container_type::pointer;
  using const_pointer   = typename container_type::const_pointer;
  using reference       = typename container_type::reference;
  using const_reference = typename container_type::const_reference;
  using rvalue          = typename container_type::rvalue;
  using size_type       = typename container_type::size_type;
  using difference_type = typename container_type::difference_type;

  // Constructors
  explicit View(container_type& container)
      : container_(container) {
  }

  View(container_type& container,
                  std::vector<range_type> ranges)
      : container_(container), ranges_(std::move(ranges)) {
    assert(check_constraints());
  }

  View(container_type& container,
                  std::vector<size_type> indices,
                  std::vector<range_type> ranges)
      : container_(container),
        indices_(std::move(indices)),
        ranges_(std::move(ranges)) {
    assert(check_constraints());
  }

  // Attribution operators
  const View& operator=(value_type value) const & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) = value;
    return *this;
  }

  View& operator=(value_type value) & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) = value;
    return *this;
  }

  View&& operator=(value_type value) && {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) = value;
    return std::move(*this);
  }

  // Sum operators
  const View& operator+=(value_type value) const & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) += value;
    return *this;
  }

  View& operator+=(value_type value) & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) += value;
    return *this;
  }

  View&& operator+=(value_type value) && {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) += value;
    return std::move(*this);
  }

  // Subtraction operators
  const View& operator-=(value_type value) const & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) -= value;
    return *this;
  }

  View& operator-=(value_type value) & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) -= value;
    return *this;
  }

  View&& operator-=(value_type value) && {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) -= value;
    return std::move(*this);
  }

  // Multiplication operators
  const View& operator*=(value_type value) const & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) *= value;
    return *this;
  }

  View& operator*=(value_type value) & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) *= value;
    return *this;
  }

  View&& operator*=(value_type value) && {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) *= value;
    return std::move(*this);
  }

  // Division operators
  const View& operator/=(value_type value) const & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) /= value;
    return *this;
  }

  View& operator/=(value_type value) & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) /= value;
    return *this;
  }

  View&& operator/=(value_type value) && {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) /= value;
    return std::move(*this);
  }

  // Remainder operators
  const View& operator%=(value_type value) const & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) %= value;
    return *this;
  }

  View& operator%=(value_type value) & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) %= value;
    return *this;
  }

  View&& operator%=(value_type value) && {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    container_.buffer_value(offset) %= value;
    return std::move(*this);
  }

  // Type cast operators
  operator const value_type&() const & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    return container_.buffer_value(offset);
  }

  operator value_type&() & {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    return container_.buffer_value(offset);
  }

  operator value_type&&() && {
    assert(ranges_.size() == 0);
    auto offset = container_.buffer_offset(indices_);
    return std::move(container_.buffer_value(offset));
  }

  // Element access operators
  const View operator[](std::vector<size_type> indices) const & {
    assert(check_num_indices(indices));

    auto num_indices = indices.size();

    indices.insert(
        std::begin(indices), std::begin(indices_), std::end(indices_));

    std::vector<range_type> ranges(
        std::begin(ranges_) + num_indices,
        std::end(ranges_));

    return { container_, std::move(indices), std::move(ranges) };
  }

  View operator[](std::vector<size_type> indices) & {
    assert(check_num_indices(indices));

    auto num_indices = indices.size();

    indices.insert(
        std::begin(indices), std::begin(indices_), std::end(indices_));

    std::vector<range_type> ranges(
        std::begin(ranges_) + num_indices,
        std::end(ranges_));

    return { container_, std::move(indices), std::move(ranges) };
  }

  View&& operator[](std::vector<size_type> indices) && {
    assert(check_num_indices(indices));

    indices_.insert(
        std::begin(indices_),
        std::begin(indices),
        std::end(indices));

    ranges_.erase(
        std::begin(ranges_),
        std::begin(ranges_) + indices.size());

    assert(check_constraints());
    return std::move(*this);
  }

  const View operator[](size_type index) const & {
    assert(check_remaining_ranges());
    return operator[](std::vector<size_type>{ std::move(index) });
  }

  View operator[](size_type index) & {
    assert(check_remaining_ranges());
    return operator[](std::vector<size_type>{ std::move(index) });
  }

  View&& operator[](size_type index) && {
    assert(check_remaining_ranges());

    indices_.push_back(std::move(index));

    ranges_.erase(std::begin(ranges_), std::begin(ranges_) + 1);

    assert(check_constraints());
    return std::move(*this);
  }

  // Comparison operators
  // TODO(renatocf): Use iterator to calculate
  bool operator==(const View& rhs) const {
    if (indices_ != rhs.indices_ || ranges_ != rhs.ranges_) return false;

    if (ranges_.size() == 0) {
      auto lhs_offset = container_.buffer_offset(indices_);
      auto rhs_offset = rhs.container_.buffer_offset(indices_);
      return container_.buffer_value(lhs_offset)
        == rhs.container_.buffer_value(rhs_offset);
    }

    std::vector<size_type> remaining_indices(ranges_.size());

    std::transform(
        std::begin(ranges_),
        std::end(ranges_),
        std::begin(remaining_indices),
        [](const auto& range) { return range.begin; });

    size_t i = remaining_indices.size() - 1;
    while (true) {
      if (remaining_indices[i] != ranges_[i].end) {
        if ((*this)[remaining_indices] != rhs[remaining_indices]) return false;
        i = remaining_indices.size() - 1;
      } else {
        remaining_indices[i] = ranges_[i].begin;
        if (--i >= remaining_indices.size()) break;
      }

      remaining_indices[i]++;
    }

    return true;
  }

  bool operator!=(const View& rhs) const {
    return !operator==(rhs);
  }

  // Concrete methods
  container_type& container() {
    return container_;
  }

  const container_type& container() const {
    return container_;
  }

  size_type num_dimensions() const {
    return ranges_.size();
  }

  range_type dimension_range(size_type dimension) const {
    assert(check_dimension(dimension));
    return ranges_[dimension];
  }

 private:
  // Instance variables
  container_type& container_;
  std::vector<size_type> indices_;
  std::vector<range_type> ranges_;

  // Concrete methods
  bool check_constraints() const {
    assert(check_num_dimensions());
    assert(check_indices());
    assert(check_ranges());
    return true;
  }

  bool check_num_dimensions() const {
    return ranges_.size() + indices_.size() == container_.num_dimensions();
  }

  bool check_indices() const {
    for (size_t i = 0; i < indices_.size(); i++) {
      if (indices_[i] > container_.dimension_size(i)) return false;
    }
    return true;
  }

  bool check_ranges() const {
    for (size_t i = 0; i < ranges_.size(); i++) {
      size_t j = indices_.size() + i;
      if (ranges_[i].end > container_.dimension_size(j)) return false;
    }
    return true;
  }

  bool check_dimension(const size_type& dimension) const {
    return dimension < ranges_.size();
  }

  bool check_remaining_ranges() const {
    return ranges_.size() > 0;
  }

  bool check_num_indices(const std::vector<size_type>& indices) const {
    return indices.size() > 0 && indices.size() <= num_dimensions();
  }
};

// Aliases
template<typename T>
using multivector_t = MultiVector<T>;

template<typename T>
using view_t = View<multivector_t<T>>;

using range_t = Range;

}  // namespace multivector

#endif  // MULTIVECTOR_MULTIVECTOR_
