#pragma once
#include <string>
#include <vector>
#include <queue>

#include "token.h"

struct node{

	size_t line;

	std::string value;
	std::string rule;
	std::vector<node*> child;

	node(const std::string &rul, const std::string & val="", size_t line =0) :rule(rul), value(val), line(line) {}

	~node();

	void add(node *childe);
	void print(std::ofstream& out,size_t hight=0);

	node* find_child(const std::string& name);
};


//class tree {
//	node* root;
//
//public:
//	
//};
