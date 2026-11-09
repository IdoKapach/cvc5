#include <functional>

#include "util/inf_string.h"

using namespace std;

namespace cvc5::internal {

InfiniteString InfiniteString::concat(const String& new_prefix) const
{
  String new_prefix_concat = new_prefix.concat(d_prefix);
  if (new_prefix_concat == d_suffix)
  {
    new_prefix_concat = String();
  }
  return InfiniteString(new_prefix_concat, d_suffix);
}

bool InfiniteString::strncmp(const String& y, std::size_t n) const {
  const String& n_str = prefix(n);
  return n_str.strncmp(y, n);
}

bool InfiniteString::strncmp(const InfiniteString& y, std::size_t n) const {
  const String& n_str = prefix(n);
  const String& n_y = y.prefix(n);
  return n_str.strncmp(n_y, n);
}


bool InfiniteString::isRepeated() const {
  return (d_prefix.empty() || d_prefix.prefix(1) == d_suffix.prefix(1))
         && d_prefix.isRepeated() && d_suffix.isRepeated();
}

bool InfiniteString::hasPrefix(const String& y) const{
  const String& n_pre = prefix(y.size());
  return n_pre == y;
}

InfiniteString InfiniteString::update(std::size_t i, const String& t) const {
  String p = prefix(i);
  p = p.concat(t);
  if (i + t.size() <= d_prefix.size()) {
    p = p.concat(d_prefix.substr(i + t.size()));
    return InfiniteString(p, d_suffix);
  }
  size_t rotate_inx = (i + t.size() - d_prefix.size()) % d_suffix.size();
  String new_suffix = rotate(d_suffix, rotate_inx);
  return InfiniteString(p, new_suffix);  
}

InfiniteString InfiniteString::replace(const String& s, const String& t) const {
  size_t occ_i = find(s);
  if (occ_i == std::string::npos) {
    return *this;
  }
  if (occ_i + s.size() <= d_prefix.size()) {
    String new_prefix = d_prefix.substr(0, occ_i).concat(t).concat(d_prefix.substr(occ_i + s.size()));
    return InfiniteString(new_prefix, d_suffix);
  }
  String new_prefix = prefix(occ_i).concat(t);
  size_t rotate_inx = (occ_i + s.size() - d_prefix.size()) % d_suffix.size();
  String new_suffix = rotate(d_suffix, rotate_inx);
  return InfiniteString(new_prefix, new_suffix);
}

InfiniteString InfiniteString::substr(std::size_t i) const {
  if (i < d_prefix.size()) {
    String new_prefix = d_prefix.substr(i);
    return InfiniteString(new_prefix, d_suffix);
  }
  size_t rotate_inx = (i - d_prefix.size()) % d_suffix.size();
  String new_suffix = rotate(d_suffix, rotate_inx);
  return InfiniteString(new_suffix);
}

String InfiniteString::substr(std::size_t i, std::size_t j) const {
  String sub_str;
  size_t rem_j = j;
  if (i < d_prefix.size())
  {
    if (i + j <= d_prefix.size()) {
      return d_prefix.substr(i, j);
    }
    sub_str = d_prefix.substr(i);
    rem_j = j - (d_prefix.size() - i);
  }
  else {
    size_t rotate_inx = (i - d_prefix.size()) % d_suffix.size();
    if (rotate_inx + j <= d_suffix.size()) {
      return d_suffix.substr(rotate_inx, j);
    }
    sub_str = d_suffix.substr(rotate_inx);
    rem_j = j - (d_suffix.size() - rotate_inx);
  }
  while (rem_j > d_suffix.size()) {
      sub_str = sub_str.concat(d_suffix);
      rem_j -= d_suffix.size();
  }
  sub_str = sub_str.concat(d_suffix.substr(0, rem_j));
  return sub_str;
}

std::size_t InfiniteString::roverlap(const String& y) const {
  String finX = prefix(y.size());
  return finX.roverlap(y);
}

bool InfiniteString::isRatNumber() const {
  size_t prefixHasDot = d_prefix.find(String("."));
  if (prefixHasDot == std::string::npos || prefixHasDot == 0 || !d_suffix.isNumber()) {
    return false;
  }
  String prefixBeforeDot = d_prefix.substr(0, prefixHasDot);
  String prefixAfterDot = d_prefix.substr(prefixHasDot + 1);
  if (!prefixBeforeDot.isNumber() || !prefixAfterDot.isNumber()) {
    return false;
  }
  return true;
}

Rational InfiniteString::toRatNumber(size_t n) const {
  Assert(isRatNumber());
  size_t dotInx = d_prefix.find(String("."));
  String srtNum = n != 0 ? prefix(dotInx + 1 + n) : prefix(dotInx);
  return Rational(srtNum.toString());
}

unsigned InfiniteString::front() const {
  if (!d_prefix.empty()) {
    return d_prefix.front();
  }
  return d_suffix.front();
}

unsigned InfiniteString::back() const {
  return d_suffix.back();
}







unsigned InfiniteString::charAt(size_t i) const
{
  const std::vector<unsigned>& p = d_prefix.getVec();
  const std::vector<unsigned>& s = d_suffix.getVec();
  return i < p.size() ? p[i] : s[(i - p.size()) % s.size()];
}

String InfiniteString::getNChars(size_t n) const
{
  std::vector<unsigned> v;
  v.reserve(n);
  for (size_t i = 0; i < n; ++i) v.push_back(charAt(i));
  return String(v);
}

size_t InfiniteString::minSharedMul(size_t x, size_t y) {
  size_t b = x > y ? x : y;
  size_t s = x > y ? y : x;
  size_t n = b;

  while (n % s != 0) {
    n += b;
  }

  return n;
}

int InfiniteString::cmp(const InfiniteString& y) const {
  std::vector<std::reference_wrapper<const InfiniteString>> infStrs = {*this, y};
  size_t longInx = infStrs[0].get().d_prefix.size() >= infStrs[1].get().d_prefix.size() ? 0 : 1;
  const InfiniteString& longInfStr = infStrs[longInx].get();
  const InfiniteString& shortInfStr = infStrs[1 - longInx].get();
  short isThisLonger = longInx == 0 ? 1 : -1;

  String l_prefix = longInfStr.d_prefix;
  String s_prefix = shortInfStr.prefix(l_prefix.size());
  String l_suffix = longInfStr.d_suffix;
  size_t rotate_inx = (l_prefix.size() - s_prefix.size()) % shortInfStr.d_suffix.size();
  String s_suffix = rotate(shortInfStr.d_suffix, rotate_inx);

  if (l_prefix > s_prefix) {
    return isThisLonger;
  }
  if (l_prefix < s_prefix) {
    return isThisLonger * -1;
  }
  String s_expSuffix = shortInfStr.substr(s_prefix.size(), max(l_suffix.size(), s_suffix.size()));
  size_t s_rem = (s_suffix.size() - (s_expSuffix.size() % s_suffix.size())) % s_suffix.size();
  s_expSuffix = s_expSuffix.concat(s_suffix.suffix(s_rem));
  String l_expSuffix = longInfStr.substr(l_prefix.size(), max(l_suffix.size(), s_suffix.size()));

  if (l_expSuffix > s_expSuffix) {
    return isThisLonger;
  }
  if (l_expSuffix < s_expSuffix) {
    return isThisLonger * -1;
  }

  size_t indentation = s_expSuffix.size() % l_suffix.size();
  size_t mod_jumps = indentation;

  while (indentation != 0) {
    if (longInfStr.d_suffixesVec[indentation].second != 0) {
      return longInfStr.d_suffixesVec[indentation].second * isThisLonger;
    }
    indentation = (indentation + mod_jumps) % l_suffix.size();
  }

  return 0;
}

int InfiniteString::naiveCmp(const InfiniteString& y) const
{
  String prefix_y = y.getPrefix();
  String suffix_y = y.getSuffix();
  String prefix_t = d_prefix;
  String suffix_t = d_suffix;

  if (prefix_t.size() < prefix_y.size())
  {
    prefix_t = getNChars(prefix_y.size());
    size_t rotate_inx = (prefix_y.size() - prefix_t.size()) % suffix_t.size();
    suffix_t = rotate(suffix_t, rotate_inx);
  }
  else if (prefix_t.size() > prefix_y.size())
  {
    prefix_y = y.getNChars(prefix_t.size());
    size_t rotate_inx = (prefix_t.size() - prefix_y.size()) % suffix_y.size();
    suffix_y = rotate(suffix_y, rotate_inx);
  }

  if (prefix_t > prefix_y) {
    return 1;
  }
  if (prefix_t < prefix_y) {
    return -1;
  }

  size_t suffix_len =
      InfiniteString::minSharedMul(suffix_t.size(), suffix_y.size());
  suffix_t =
      getNChars(suffix_len + prefix_t.size()).suffix(suffix_len);
  suffix_y =
      y.getNChars(suffix_len + prefix_y.size()).suffix(suffix_len);

  if (suffix_t > suffix_y) {
    return 1;
  }
  if (suffix_t < suffix_y) {
    return -1;
  }
  return 0;
}

std::vector<std::pair<size_t, short>> InfiniteString::buildSuffixesVec(String& s) {
  if (s.empty()) {
    return {};
  }
  const std::vector<unsigned> s_vec = s.getVec();
  std::vector<std::pair<size_t, short>> suffixesVec(s.size(), {s.size(), 0});
  suffixesVec[0] = {0, 0};
  size_t i = 1;
  size_t j = 0;
  while (i < s.size()) {
    while (j + i < s.size() && s_vec[0 + j] == s_vec[i+j]) {
      j++;
    }
    short bigger = (j + i) == s.size() ? 0
                                 : (s_vec[0 + j] < s_vec[i + j]) - (s_vec[0 + j] > s_vec[i + j]);   

    // the order of these 2 lines doesn't matter to the inx 2 cmp, maybe to the general one.
    // j (and the .first element) represents the num of equations.
    suffixesVec[i] = {j, bigger};
    j = min(s.size() - i - 1, j);

    size_t k = 1;
    if (i == 1) {
      while (k <= j) {
        suffixesVec[i + k] = {suffixesVec[i].first - k, bigger};
        k++;
      }
    }
    else {
      while (k <= j) {
        if (suffixesVec[k].first >= suffixesVec[i].first) {
          suffixesVec[i + k] = {suffixesVec[i].first - k, bigger};
        }
        else if (suffixesVec[k].first < suffixesVec[i].first - 1) {
          suffixesVec[i + k] = {suffixesVec[k].first, suffixesVec[k].second};
        }
        else {
          i += k;
          break;
        }
        k++;
      }
    }
    j = (j > k) ? (j - k) : 0;
  }

  return suffixesVec;
}
} // namespace cvc5::internal