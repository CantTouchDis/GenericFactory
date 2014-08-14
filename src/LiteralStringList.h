// Copyright Mikael Persson
// http://www.daniweb.com/software-development/cpp/code/482276/c11-compile-time-string-concatenation-with-constexpr
#ifndef LITERALSTRINGLIST_H_
#define LITERALSTRINGLIST_H_
#include <cstddef>
#include <cstring>
// used to convert to std::string.
#include <string>
class literal_str_list {
 private:
  const char* const text_ptr;
  unsigned int text_size;
  const literal_str_list* const head;
  constexpr char get_char_from_head(unsigned int i, unsigned int hd_size) const {
    return (i < hd_size ? (*head)[i] : (i < (hd_size + text_size) ? text_ptr[i - hd_size] : '\0'));
  }
  static constexpr std::size_t fnv_prime = (sizeof(std::size_t) == 8 ? 1099511628211u : 16777619u);
  static constexpr std::size_t fnv_offset = (sizeof(std::size_t) == 8 ? 14695981039346656037u : 2166136261u);
  constexpr std::size_t fnv_1a_hash(unsigned int i) const {
    return (i == 0 ?
        (head != nullptr ?
         ((head->fnv_1a_hash(head->text_size-1) ^ text_ptr[0]) * fnv_prime) :
         fnv_offset) :
        ((fnv_1a_hash(i-1) ^ text_ptr[i]) * fnv_prime));
  }
  template <typename FwdIter>
  void copy_to_recurse(FwdIter& beg, FwdIter end) const {
    if (head != nullptr)
      head->copy_to_recurse(beg, end);
    for (unsigned int i = 0; (i < text_size) && (beg != end); ++i, ++beg)
      *beg = text_ptr[i];
  }
  void copy_to_recurse(char*& beg, char* end) const {
    if (head != nullptr)
      head->copy_to_recurse(beg, end);
    std::size_t sz_to_cpy = (end - beg < text_size ? end - beg : text_size);
    std::memcpy(beg, text_ptr, sz_to_cpy);
    beg += sz_to_cpy;
  }
  constexpr literal_str_list(const char* aStr, unsigned int N,
      const literal_str_list* aHead = nullptr) :
    text_ptr(aStr), text_size(N), head(aHead) { }

 public:
  template <unsigned int N>
  constexpr literal_str_list(const char(&aStr)[N],
      const literal_str_list* aHead = nullptr) :
    text_ptr(aStr), text_size(N-1), head(aHead) {
      static_assert(N >= 1, "Invalid string literal! Length is zero!");
  }
  constexpr unsigned int size() const {
    return text_size + (head != nullptr ? head->size() : 0);
  }
  constexpr char operator[](unsigned int i) const {
    return (head != nullptr ?
        get_char_from_head(i, head->size()) :
        (i < text_size ? text_ptr[i] : '\0'));
  }
  template <unsigned int N>
  constexpr literal_str_list operator+(const char(&aHead)[N]) const {
    return literal_str_list(aHead, this);
  }
  constexpr literal_str_list operator+(const literal_str_list& aHead) const {
    return literal_str_list(aHead.text_ptr, aHead.text_size, this);
  }
  constexpr std::size_t hash() const {
    return fnv_1a_hash(text_size-1);
  }
  template <typename FwdIter>
  void copy_to(FwdIter beg, FwdIter end) const {
    copy_to_recurse(beg, end);
  }
  void copy_to(char* beg, char* end) const {
    copy_to_recurse(beg, end);
  }
};
std::string convert_to_string(const literal_str_list& lit) {
  std::string result(lit.size(), ' ');
  lit.copy_to(result.begin(), result.end());
  return result;
}
#endif  // LITERALSTRINGLIST_H_
