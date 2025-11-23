#pragma once
#include <string>
#include <vector>
#include <queue>

#include "token.h"

struct node{
	std::string value;
	std::string rule;
	std::vector<node*> child;

	node(const std::string &rul, const std::string & val="") :rule(rul), value(val) {}

	~node();

	void add(node *childe);
	void print(std::ofstream& out,size_t hight=0);
};


//class tree {
//	node* root;
//
//public:
//	
//};
