/******************************************************************************
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2026 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * The infinite string data type.
 */

#include "cvc5_public.h"

#ifndef CVC5__UTIL__INFINITE_STRING_H
#define CVC5__UTIL__INFINITE_STRING_H

#include <iosfwd>
#include <string>
#include <vector>
#include <assert.h>

#include "util/rational.h"
#include "util/string.h"

namespace cvc5::internal {

namespace infinite_strings {

class InfiniteStringStream {
  public:
  // InfiniteStringStream() = default;
  explicit InfiniteStringStream(const String& prefix, const String& suffix)
      : d_prefix(prefix), d_suffix(suffix), d_string(String())
  {
    assert(!suffix.empty());
  }

  
  /** Return the first n characters of the infinite string stream */
  const String& getNChars(std::size_t n) {
    expand(n);
    return d_string.substr(0, n);
  }


 private:
  /** If n > d_string.size(), expand the infinite string stream to include at least 2 ^ log(n) characters
   * (square of the next power of 2 greater than n).
   * Otherwise, do nothing. 
   * Returns the number of characters in the stream after expansion.
   */
  size_t expand(std::size_t n) {
    if (d_string.size() >= n) {
      return d_string.size();
    }
    size_t new_size = 1;
    while (new_size < n) {
      new_size *= 2;
    }
    String new_string = String();
    if (new_size <= d_prefix.size()) {
      new_string = d_prefix.substr(0, new_size);
    } else {
      new_string = d_prefix;
      size_t suffix_needed = new_size - d_prefix.size();
      size_t suffix_repeats = (suffix_needed + d_suffix.size() - 1) / d_suffix.size();
      for (size_t i = 0; i < suffix_repeats; ++i) {
        new_string = new_string.concat(d_suffix);
      }
      new_string = new_string.substr(0, new_size);
    }
    d_string = new_string;
    return d_string.size();
  }

  String d_string;
  String d_prefix;
  String d_suffix;
};
} // namespace infinite_strings

/** The cvc5 infinite string class
 *
 * This data structure is the domain of values for the infinite string type.
 */
class InfiniteString
{

 public:
  /**
   * This is the cardinality of the alphabet that is representable by this
   * class. Notice that this must be greater than or equal to the cardinality
   * of the alphabet that the infinite string theory reasons about.
   * This value is the same as String::num_codes().
   */
  static inline unsigned num_codes() { return String::num_codes(); }
  /** constructors for InfiniteString
   *
   * Internally, a cvc5::internal::InfiniteString is represented by a pair of 2 strings - 
   * the prefix and the suffix.
   * The prefix is a finite string, while the suffix is a non-empty finite string that is repeated infinitely.
   */
  // InfiniteString() = default;
  explicit InfiniteString(const String& prefix,const String& suffix)
      : d_prefix(prefix), d_suffix(suffix), d_stream(d_prefix, d_suffix)
  {
    assert(!suffix.empty());
  }
  // explicit InfiniteString(const std::string& prefix, bool useEscSequencesP = false,
  //    const std::string& suffix, bool useEscSequencesS = false)
  //     : d_prefix(String(prefix, useEscSequencesP)), d_suffix(String(suffix, useEscSequencesS))
  // {
  // }
  /** lambda constructor */
  explicit InfiniteString(const String& suffix)
			: d_prefix(String()), d_suffix(suffix), d_stream(d_prefix, d_suffix)
	{
    assert(!suffix.empty());
	}
  
  /** Return the prefix of this infinite string */
  const String& getPrefix() const { return d_prefix; }
  /** Return the suffix of this infinite string */
  const String& getSuffix() const { return d_suffix; }
  /** Return the concat of new_prefix with this infinite string (new_prefix + this) */
  InfiniteString concat(const String& new_prefix) const;

  bool operator==(InfiniteString& y) { return cmp(y) == 0; }
  bool operator!=(InfiniteString& y) { return cmp(y) != 0; }
  bool operator<(InfiniteString& y) { return cmp(y) < 0; }
  bool operator>(InfiniteString& y) { return cmp(y) > 0; }
  bool operator<=(InfiniteString& y) { return cmp(y) <= 0; }
  bool operator>=(InfiniteString& y) { return cmp(y) >= 0; }

  /**
   * Returns true if this infinite string is equal to y for their first n characters.
   * If n is larger than the length of y (under the string case), this method returns
   * false.
   */
  bool strncmp(const String& y, std::size_t n);
  bool strncmp(InfiniteString& y, std::size_t n);

  bool isRepeated() const;


  /**
   * Return the first position y occurs in this infinite string
   * (or equal to it starting from this position, in the infinite case), 
   * or std::string::npos otherwise.
   */
  std::size_t find(const String& y, const std::size_t start = 0) const;
  std::size_t find(const InfiniteString& y, const std::size_t start = 0) const;

  /** Returns true if y is a prefix of this */
  bool hasPrefix(const String& y) const;
  /** Replace the character at index i in this infinite string with t */
  InfiniteString update(std::size_t i, const String& t) const;
  /** Replace the first occurrence of s in this infinite string with t */
  InfiniteString replace(const String& s, const String& t) const;
  /** Return the infinite substring of this infinite string starting at index i */
  InfiniteString substr(std::size_t i) const;
  /** Return the substring of this infinite string starting at index i with size at most j */
  String substr(std::size_t i, std::size_t j) const;
  /** Return the prefix of this infinite string of size i */
  String prefix(std::size_t i) const { return substr(0, i); }


  /** infinite string reverse overlap
   *
   * if roverlap returns m>0,
   * then the maximal prefix of this infinite string that is a suffix of y is of length
   * m.
   *
   * For example, if x is "abcdef...", then:
   * x.roverlap("aaabc") = 3
   * x.roverlap("def") = 0
   * x.roverlap("d") = 0
   * x.roverlap("defabcde") = 5
   *
   */
  std::size_t roverlap(const String& y) const;
	

  /**
   * Returns true if this infinite string corresponds in text to a rational number, for example
   * this returns true for strings "7.55555...", "12.66000...", "004.7878...", "4343.4343..." and false for strings
   * "abc...", "4a...", "-4...", "444...", "".
   */
  bool isRatNumber() const;
  /** Returns the corresponding rational for the text of this infinite string.
   *  The n parameter is the number of digits to consider in the conversion. For example, if this infinite string is "7.55555...",
   *  then toRatNumber(3) returns 7.555, and toRatNumber(5) returns 7.55555.
   */
  Rational toRatNumber(size_t n) const;
  /** Get the pair of unsigned representation (code points) of this infinite string */
  const std::pair<const std::vector<unsigned>, const std::vector<unsigned>> getVecPair() const { return std::make_pair(d_prefix.getVec(), d_suffix.getVec()); }
  /**
   * Get the unsigned (code point) value of the first character in this infinite string
   * (This is the first character of the prefix if the prefix is non-empty, and the first character of the suffix otherwise.)
   */
  unsigned front() const;
  /**
   * Get the unsigned (code point) value of the last character in this infinite string
   * (This is the last character of the suffix string.)
   */
  unsigned back() const;

  /**
   * Returns the maximum length of infinite string representable by this class.
   * Corresponds to the maximum size of d_prefix and d_suffix (which are of equal size and equal to String::maxSize()).
   */
  static size_t maxSize() { return 2 * String::maxSize(); }

  infinite_strings::InfiniteStringStream d_stream;

 private:
    /**
   * Returns a negative number if *this < y, 0 if *this and y are equal and a
   * positive number if *this > y.
   */
  int cmp(InfiniteString& y);

  /** find minimal n such that n % x = 0 and n % y = 0 */
  static size_t minSharedMul(size_t x, size_t y);

  String d_prefix;
  String d_suffix;
}; /* class InfiniteString */

} // namespace cvc5::internal

#endif /* CVC5__UTIL__INFINITE_STRING_H */
