#include <Core/StringUtils.h>

std::vector<std::string> stringSplit(const std::string& str, const std::string& splitBy)
{
	std::vector<std::string> tmp;
	size_t st = 0;
	size_t ot = 0;
	do
	{
		st = str.find(splitBy, st);
		if (st < str.size())
		{
			tmp.push_back(str.substr(ot, st - ot));
			ot = st + splitBy.size();
			st += splitBy.size();
		}
	} while (st < str.size());
	tmp.push_back(str.substr(ot));
	return tmp;
}
std::string stringReplace(const std::string& str, const std::string& oldStr, const std::string& newStr)
{
	size_t st = 0;
	std::string s = str;
	do
	{
		st = s.find(oldStr, st);
		if (st < s.size())
		{
			s.erase(st, oldStr.size());
			s.insert(st, newStr);
			st = 0;
		}
	} while (st < s.size());
	return s;
}