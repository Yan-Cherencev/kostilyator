#pragma once
#include <string>
#include <vector>


#include "drevo.h"
#include "hable.h"


enum data_type {
	TYPE_INT, TYPE_REAL, TYPE_ERROR
};

std::string dt_to_str(data_type type);

class seman	{

	hable <std::string, data_type> sym_map;
	std::vector <std::string>& errors;
	std::string polska;

	void check_program_name(node* root);

	void check_declaration(node* declar);
	void process_descr(node* descr);        
	void register_vars(node* var_list, data_type type);

	void check_call(node* var_list);
	void check_operators(node* prog);
	void check_op(node* op);


	data_type expr_type(node* expr);
	data_type simpl_expr_type(node* simpl_expr);
	data_type tail_type(node* tail_node, data_type prev_type);
public:
	seman(std::vector<std::string>& errors) :errors(errors) {};

	void start(node* root);

	std::string get_rpn()const;
};

