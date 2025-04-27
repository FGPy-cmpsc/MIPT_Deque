#pragma once
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <vector>

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
 private:
  template <bool IsConst>
  class BaseIterator;
  using alloc_traits = std::allocator_traits<Allocator>;
  using alloc_type = typename alloc_traits::template rebind_alloc<T>;
  alloc_type allocator_;

 public:
  using iterator = BaseIterator<false>;
  using const_iterator = BaseIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque() = default;

  Deque(const Deque& deq);

  explicit Deque(size_t count, const Allocator& alloc = Allocator());

  Deque(size_t count, const T& value, const Allocator& alloc = Allocator());

  Deque(Deque&& other);

  Deque(std::initializer_list<T> init, const Allocator& alloc = Allocator());

  Deque& operator=(const Deque& deq);
  Deque& operator=(Deque&& deq);

  ~Deque();

  iterator begin() { return begin_; }

  iterator end() { return end_; }

  const_iterator cbegin() const { return begin_; }

  const_iterator begin() const { return cbegin(); }

  const_iterator cend() const { return end_; }

  const_iterator end() const { return cend(); }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(begin());
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(end());
  }

  Allocator get_allocator() const { return allocator_; }

  size_t size() const;

  bool empty() const;

  void push_back(const T& value);
  void push_back(T&& value);

  template <typename... Args>
  void emplace_back(Args&&... args);

  void pop_back();

  void push_front(const T& value);
  void push_front(T&& value);

  template <typename... Args>
  void emplace_front(Args&&... args);

  template <typename... Args>
  void emplace(iterator insert_it, Args&&... args);

  void pop_front();

  size_t new_data_size();

  T& top();

  const T& top() const;

  T& operator[](size_t ind) noexcept;
  const T& operator[](size_t ind) const noexcept;

  T& at(size_t ind);
  const T& at(size_t ind) const;

  void insert(iterator insert_it, const T& value);
  void insert(iterator insert_it, T&& value);
  void erase(iterator erase_it);

  void reserve();
  void swap(Deque<T, Allocator>& deq) noexcept;
  void copy_swap(Deque<T, Allocator>& value) noexcept;
  void move_swap(Deque<T, Allocator>& deq) noexcept;

 private:
  std::vector<T*> data_;
  size_t size_ = 0;

  Deque(const Deque& deq, const Allocator& alloc);

  bool fulled_front() const;
  bool fulled_back() const;

  static constexpr size_t kBucketSize = 60;
  static constexpr size_t kScale = 3;

  template <bool IsConst>
  class BaseIterator;

  BaseIterator<false> begin_;
  BaseIterator<false> end_;

  void allocate_data(const BaseIterator<false>& iterator);
};

template <typename T, typename Allocator>
template <bool IsConst>
class Deque<T, Allocator>::BaseIterator {
 public:
  using value_type = T;
  using reference = std::conditional_t<IsConst, const T&, T&>;
  using pointer = std::conditional_t<IsConst, const T*, T*>;
  using pointer_type = std::conditional_t<IsConst, const T* const, T*>;
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;

  BaseIterator(size_t ind, pointer_type* arr) : ind_j_(ind), arr_(arr) {}
  BaseIterator() : ind_j_(0), arr_(nullptr) {};
  BaseIterator(const BaseIterator& iter) = default;
  BaseIterator(BaseIterator&& iter);
  BaseIterator& operator=(const BaseIterator& iter) = default;
  BaseIterator& operator=(BaseIterator&& iter) = default;

  operator BaseIterator<true>() const;

  template <bool Const>
  difference_type operator-(const BaseIterator<Const>& iter) const;

  template <bool Const>
  bool operator>(const BaseIterator<Const>& iter) const;
  template <bool Const>
  bool operator<(const BaseIterator<Const>& iter) const;
  template <bool Const>
  bool operator>=(const BaseIterator<Const>& iter) const;
  template <bool Const>
  bool operator<=(const BaseIterator<Const>& iter) const;
  template <bool Const>
  bool operator==(const BaseIterator<Const>& iter) const;
  template <bool Const>
  bool operator!=(const BaseIterator<Const>& iter) const;

  BaseIterator& operator++();
  BaseIterator operator++(int);
  BaseIterator& operator--();
  BaseIterator operator--(int);

  BaseIterator& operator+=(int diff);
  BaseIterator& operator-=(int diff);
  BaseIterator operator+(int diff) const;
  BaseIterator operator-(int diff) const;

  pointer operator->() const;
  reference operator*() const;

  pointer_type* get_arr() const { return arr_; }
  size_t get_ind() const { return ind_j_; }

 private:
  size_t ind_j_;
  pointer_type* arr_;
};

template <typename T, typename Allocator>
template <bool IsConst>
Deque<T, Allocator>::BaseIterator<IsConst>::BaseIterator(BaseIterator&& iter)
    : arr_(iter.arr_), ind_j_(iter.ind_j_) {
  iter.arr_ = nullptr;
  iter.ind_j_ = 0;
}

template <typename T, typename Allocator>
template <bool IsConst>
Deque<T, Allocator>::BaseIterator<IsConst>::operator BaseIterator<true>()
    const {
  return {this->ind_j_, this->arr_};
}

template <typename T, typename Allocator>
template <bool IsConst>
template <bool Const>
typename Deque<T, Allocator>::template BaseIterator<IsConst>::difference_type
Deque<T, Allocator>::BaseIterator<IsConst>::operator-(
    const BaseIterator<Const>& iter) const {
  return ((arr_ - iter.arr_) * static_cast<int>(kBucketSize)) + ind_j_ -
         iter.ind_j_;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template BaseIterator<IsConst>&
Deque<T, Allocator>::BaseIterator<IsConst>::operator++() {
  if (arr_ == nullptr) {
    return *this;
  }
  if (ind_j_ == kBucketSize - 1) {
    ++arr_;
  }
  ind_j_ = (ind_j_ + 1) % kBucketSize;

  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template BaseIterator<IsConst>
Deque<T, Allocator>::BaseIterator<IsConst>::operator++(int) {
  if (arr_ == nullptr) {
    return *this;
  }
  iterator old_it = *this;
  ++*this;
  return old_it;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template BaseIterator<IsConst>&
Deque<T, Allocator>::BaseIterator<IsConst>::operator--() {
  if (arr_ == nullptr) {
    return *this;
  }
  if (ind_j_ == 0) {
    --arr_;
  }
  ind_j_ = (ind_j_ + kBucketSize - 1) % kBucketSize;
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template BaseIterator<IsConst>
Deque<T, Allocator>::BaseIterator<IsConst>::operator--(int) {
  if (arr_ == nullptr) {
    return *this;
  }
  iterator old_it = *this;
  --*this;
  return old_it;
}

template <typename T, typename Allocator>
template <bool IsConst>
template <bool Const>
bool Deque<T, Allocator>::BaseIterator<IsConst>::operator>(
    const BaseIterator<Const>& iter) const {
  return (*this - iter) > 0;
}

template <typename T, typename Allocator>
template <bool IsConst>
template <bool Const>
bool Deque<T, Allocator>::BaseIterator<IsConst>::operator<(
    const BaseIterator<Const>& iter) const {
  return iter > *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
template <bool Const>
bool Deque<T, Allocator>::BaseIterator<IsConst>::operator>=(
    const BaseIterator<Const>& iter) const {
  return !(*this < iter);
}

template <typename T, typename Allocator>
template <bool IsConst>
template <bool Const>
bool Deque<T, Allocator>::BaseIterator<IsConst>::operator<=(
    const BaseIterator<Const>& iter) const {
  return !(*this > iter);
}

template <typename T, typename Allocator>
template <bool IsConst>
template <bool Const>
bool Deque<T, Allocator>::BaseIterator<IsConst>::operator==(
    const BaseIterator<Const>& iter) const {
  return this->arr_ == iter.arr_ && this->ind_j_ == iter.ind_j_;
}

template <typename T, typename Allocator>
template <bool IsConst>
template <bool Const>
bool Deque<T, Allocator>::BaseIterator<IsConst>::operator!=(
    const BaseIterator<Const>& iter) const {
  return !(*this == iter);
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template BaseIterator<IsConst>&
Deque<T, Allocator>::BaseIterator<IsConst>::operator+=(int diff) {
  if (arr_ == nullptr) {
    return *this;
  }
  int new_index;
  int arr_diff;
  if (diff >= 0) {
    int dividend = static_cast<int>(ind_j_) + diff;
    arr_diff = dividend / static_cast<int>(kBucketSize);
    new_index = dividend % static_cast<int>(kBucketSize);
  } else {
    int dividend = static_cast<int>(kBucketSize - ind_j_) - 1 - diff;
    arr_diff = -dividend / static_cast<int>(kBucketSize);
    new_index = static_cast<int>(kBucketSize) -
                dividend % static_cast<int>(kBucketSize) - 1;
  }
  arr_ += arr_diff;
  ind_j_ = static_cast<size_t>(new_index);
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template BaseIterator<IsConst>&
Deque<T, Allocator>::BaseIterator<IsConst>::operator-=(int diff) {
  return *this += (-diff);
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template BaseIterator<IsConst>
Deque<T, Allocator>::BaseIterator<IsConst>::operator+(int diff) const {
  BaseIterator iter(ind_j_, arr_);
  iter += diff;
  return iter;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template BaseIterator<IsConst>
Deque<T, Allocator>::BaseIterator<IsConst>::operator-(int diff) const {
  BaseIterator iter(ind_j_, arr_);
  iter -= diff;
  return iter;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template BaseIterator<IsConst>::pointer
Deque<T, Allocator>::BaseIterator<IsConst>::operator->() const {
  return *arr_ + ind_j_;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename Deque<T, Allocator>::template BaseIterator<IsConst>::reference
Deque<T, Allocator>::BaseIterator<IsConst>::operator*() const {
  return *(*arr_ + ind_j_);
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(const Deque& deq, const Allocator& alloc)
    : allocator_(alloc) {
  try {
    for (auto iter = deq.begin_; iter != deq.end_; ++iter) {
      emplace_back(*iter);
    }
  } catch (...) {
    for (auto iter = begin_; iter != end_; ++iter) {
      alloc_traits::destroy(allocator_, iter.operator->());
    }
    for (size_t ind = 0; ind < data_.size(); ++ind) {
      alloc_traits::deallocate(allocator_, data_[ind], kBucketSize);
    }
    data_ = {};
    throw;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(const Deque<T, Allocator>& deq)
    : Deque(deq, alloc_traits::select_on_container_copy_construction(
                     deq.allocator_)) {}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(size_t count, const T& value, const Allocator& alloc)
    : allocator_(alloc) {
  try {
    for (size_t total_count = 0; total_count < count; ++total_count) {
      emplace_back(value);
    }
  } catch (...) {
    for (auto it = begin_; it != end_; ++it) {
      alloc_traits::destroy(allocator_, it.operator->());
    }
    for (size_t ind = 0; ind < data_.size(); ++ind) {
      alloc_traits::deallocate(allocator_, data_[ind], kBucketSize);
    }
    data_ = {};
    throw;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(size_t count, const Allocator& alloc)
    : allocator_(alloc) {
  try {
    for (size_t total_count = 0; total_count < count; ++total_count) {
      emplace_back();
    }
  } catch (...) {
    for (auto it = begin_; it != end_; ++it) {
      alloc_traits::destroy(allocator_, it.operator->());
    }
    for (size_t ind = 0; ind < data_.size(); ++ind) {
      alloc_traits::deallocate(allocator_, data_[ind], kBucketSize);
    }
    data_ = {};
    throw;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(Deque&& other) {
  move_swap(other);
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(std::initializer_list<T> init,
                           const Allocator& alloc)
    : allocator_(alloc) {
  try {
    for (auto iter = init.begin(); iter != init.end(); ++iter) {
      emplace_back(std::move(*iter));
    }
  } catch (...) {
    for (auto it = begin_; it != end_; ++it) {
      alloc_traits::destroy(allocator_, it.operator->());
    }
    for (size_t ind = 0; ind < data_.size(); ++ind) {
      alloc_traits::deallocate(allocator_, data_[ind], kBucketSize);
    }
    data_ = {};
    throw;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::~Deque() {
  for (auto it = begin(); it != end(); ++it) {
    alloc_traits::destroy(allocator_, it.operator->());
  }
  for (size_t array_index = 0; array_index < data_.size(); ++array_index) {
    alloc_traits::deallocate(allocator_, data_[array_index], kBucketSize);
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::swap(Deque<T, Allocator>& deq) noexcept {
  std::swap(data_, deq.data_);
  std::swap(begin_, deq.begin_);
  std::swap(end_, deq.end_);
  std::swap(size_, deq.size_);
  std::swap(allocator_, deq.allocator_);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::copy_swap(Deque<T, Allocator>& value) noexcept {
  swap(value);
  auto alloc = allocator_;
  if (alloc_traits::propagate_on_container_copy_assignment::value) {
    allocator_ = value.allocator_;
  }
  value.allocator_ = alloc;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::move_swap(Deque<T, Allocator>& deq) noexcept {
  swap(deq);
  auto alloc = allocator_;
  if (alloc_traits::propagate_on_container_move_assignment::value) {
    allocator_ = deq.allocator_;
  }
  deq.allocator_ = alloc;
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(const Deque& deq) {
  auto new_alloc = alloc_traits::propagate_on_container_copy_assignment::value
                       ? deq.allocator_
                       : allocator_;
  Deque new_deq(deq, new_alloc);
  swap(new_deq);
  return *this;
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(Deque&& deq) {
  if (alloc_traits::propagate_on_container_move_assignment::value ||
      allocator_ == deq.allocator_) {
    swap(deq);
    allocator_ = deq.allocator_;
  } else {
    auto alloc = deq.allocator_;
    Deque new_deq;
    new_deq.swap(deq);
    deq.allocator_ = alloc;
    try {
      for (auto iter = new_deq.begin_; iter != new_deq.end_; ++iter) {
        emplace_back(std::move(*iter));
      }
    } catch (...) {
      for (auto iter = begin_; iter != end_; ++iter) {
        alloc_traits::destroy(allocator_, iter.operator->());
      }
      for (size_t ind = 0; ind < data_.size(); ++ind) {
        alloc_traits::deallocate(allocator_, data_[ind], kBucketSize);
      }
      data_ = {};
      throw;
    }
  }
  return *this;
}

template <typename T, typename Allocator>
size_t Deque<T, Allocator>::size() const {
  return size_;
}

template <typename T, typename Allocator>
bool Deque<T, Allocator>::empty() const {
  return size_ == 0;
}

template <typename T, typename Allocator>
T& Deque<T, Allocator>::operator[](size_t ind) noexcept {
  return *(begin_ + ind);
}

template <typename T, typename Allocator>
const T& Deque<T, Allocator>::operator[](size_t ind) const noexcept {
  return *(begin_ + ind);
}

template <typename T, typename Allocator>
T& Deque<T, Allocator>::at(size_t ind) {
  if (ind >= size_) {
    throw std::out_of_range("out of range");
  }
  return *(begin_ + ind);
}

template <typename T, typename Allocator>
const T& Deque<T, Allocator>::at(size_t ind) const {
  if (ind >= size_) {
    throw std::out_of_range("out of range");
  }
  return *(begin_ + ind);
}

template <typename T, typename Allocator>
bool Deque<T, Allocator>::fulled_front() const {
  return begin_.get_ind() == 0 && *(begin_.get_arr() - 1) == nullptr;
}

template <typename T, typename Allocator>
bool Deque<T, Allocator>::fulled_back() const {
  return end_.get_ind() == 0 && *(end_.get_arr()) == nullptr;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::allocate_data(const BaseIterator<false>& iterator) {
  T* new_arr = alloc_traits::allocate(allocator_, kBucketSize);
  *(iterator.get_arr()) = new_arr;
}

template <typename T, typename Allocator>
size_t Deque<T, Allocator>::new_data_size() {
  return kScale * data_.size();
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::reserve() {
  size_t new_size = new_data_size();
  if (data_.empty()) {
    new_size = 1;
  }
  std::vector<T*> new_data(new_size);
  for (size_t ind = 0; ind < data_.size(); ++ind) {
    new_data[(kScale / 2 * data_.size()) + ind] = data_[ind];
  }
  size_t begin_offset = begin_.get_arr() - data_.data();
  size_t end_offset = end_.get_arr() - data_.data();
  data_ = new_data;
  if (data_.size() == 1) {
    begin_ = {kBucketSize / 2, data_.data()};
    end_ = begin_;
  } else {
    begin_ = {begin_.get_ind(),
              data_.data() + (data_.size() / kScale) + begin_offset};
    end_ = {end_.get_ind(),
            data_.data() + (data_.size() / kScale) + end_offset};
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back(const T& value) {
  emplace_back(value);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back(T&& value) {
  emplace_back(std::forward<T>(value));
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_back(Args&&... args) {
  if (data_.empty() ||
      end_.get_ind() == 0 && end_.get_arr() == data_.data() + data_.size()) {
    reserve();
    allocate_data(end_);
  } else if (fulled_back()) {
    allocate_data(end_);
  }
  alloc_traits::construct(allocator_, *end_.get_arr() + end_.get_ind(),
                          std::forward<Args>(args)...);
  ++end_;
  ++size_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front(const T& value) {
  emplace_front(value);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front(T&& value) {
  emplace_front(std::forward<T>(value));
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_front(Args&&... args) {
  if (data_.empty() ||
      begin_.get_ind() == 0 && begin_.get_arr() == data_.data()) {
    reserve();
    --begin_;
    allocate_data(begin_);
  } else if (fulled_front()) {
    --begin_;
    allocate_data(begin_);
  } else {
    --begin_;
  }
  alloc_traits::construct(allocator_, *begin_.get_arr() + begin_.get_ind(),
                          std::forward<Args>(args)...);
  ++size_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::pop_back() {
  --end_;
  alloc_traits::destroy(allocator_, end_.operator->());
  --size_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::pop_front() {
  alloc_traits::destroy(allocator_, *begin_.get_arr() + begin_.get_ind());
  ++begin_;
  --size_;
}

template <typename T, typename Allocator>
T& Deque<T, Allocator>::top() {
  return *rbegin();
}

template <typename T, typename Allocator>
const T& Deque<T, Allocator>::top() const {
  return *crbegin();
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::insert(iterator insert_it, const T& value) {
  emplace(insert_it, value);
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::insert(iterator insert_it, T&& value) {
  emplace(insert_it, std::move(value));
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace(iterator insert_it, Args&&... args) {
  if (insert_it == begin()) {
    emplace_front(std::forward<Args>(args)...);
    return;
  }
  if (insert_it == end()) {
    emplace_back(std::forward<Args>(args)...);
    return;
  }
  T last_element = top();
  for (iterator iter = end() - 1; iter != insert_it; --iter) {
    *iter = *(iter - 1);
  }
  *insert_it = T(std::forward<Args>(args)...);
  push_back(std::move(last_element));
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::erase(iterator erase_it) {
  for (iterator iter = erase_it; iter != end(); ++iter) {
    *iter = *(iter + 1);
  }
  pop_back();
}
