#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <vector>

std::vector<std::string> stringSplit(const std::string& str, const std::string& splitBy);
std::string stringReplace(const std::string& str, const std::string& oldStr, const std::string& newStr);
#endif // !STRINGUTILS_H
