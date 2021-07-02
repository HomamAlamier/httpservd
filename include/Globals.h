
#ifndef DUMP_VAR

#define DUMP_VAR(var) "(" + std::string(typeid(var).name()) + ") " + \
	#var + " = " + \
	std::to_string(var)
#define DUMP_STRING(str) std::string("(string:" + std::to_string(str.size()) + ") ") + #str + " = " + str 

#endif // !DUMP_VAR

