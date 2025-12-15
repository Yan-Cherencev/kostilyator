#pragma once
#include <string>
#include <stack>
#include "hable.h"

class executor{
	std::stack<double> stck;
	hable <std::string, double> memory;

	bool is_digit(char lit);
public:
	void execut(const std::string &rpn);
};

