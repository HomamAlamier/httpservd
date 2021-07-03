#include <Core/StringUtils.h>

std::vector<std::string> stringSplit(const std::string& str, const std::string& splitBy)
{
	std::vector<std::string> tmp;
	int st = 0;
	int ot = 0;
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