#include "seman.h"



std::string dt_to_str(data_type type) {
	switch (type)
	{
	case TYPE_INT:
		return "INT";
		break;
	case TYPE_REAL:
		return "REAL";
		break;
	default:
		return "ERROR_T";
		break;
	}

}


//-------------------------------
void seman::check_program_name(node* root){

	node* begin_node = root->find_child("Begin");
	node* end_node = root->find_child("End");

	if (begin_node != nullptr && end_node != nullptr) {
		node* start_id = begin_node->find_child("id");
		node* end_id = end_node->find_child("id");

		if (start_id!= nullptr  &&end_id != nullptr) {
			if (start_id->value != end_id->value) {
				errors.push_back("Semantic Error (Line " + std::to_string(end_id->line) +
					"): Program name ("+ start_id->value +") differs from End name (" + end_id->value +
					"')." );
			}
		}
	}
}

void seman::check_declaration(node* declar){

	node* cur = declar->find_child("Descriptions");

	while (cur) {

		node* temp_descrip = cur->find_child("Descr");
		if (temp_descrip) {
			process_descr(temp_descrip);
		}

		cur = cur->find_child("Descriptions");
	}
}

void seman::process_descr(node* descr){

	node* type_node = descr->find_child("Type");
	node* var_list = descr->find_child("VarList");

	if (type_node && var_list) {

		data_type type = TYPE_ERROR;
		
		if (type_node->find_child("INT")){
			type = TYPE_INT;
		}
		else if (type_node->find_child("REAL")){
			type = TYPE_REAL;
		}

		register_vars(var_list, type);
	}
}

void seman::register_vars(node* var_list, data_type type){
	if (!var_list) {
		return;
	}

	node* ids = var_list->find_child("id");

	if (ids) {
		std::string& temp = ids->value;

		if (sym_map.find(temp)!=nullptr) {
			errors.push_back("Semantic Error (Line " + std::to_string(ids->line) +
				"): Variable '" + temp + "' declared yet.");
		}
		else {
			sym_map[temp] = type;
			polska += temp + " " + dt_to_str(type) + " DECL \n"; /////-----------------------------------------ÔÎÐÌÀÒÈÐÎÂÀÍÈÅ ÐÏÍ
		}
	}

	node* next_list = var_list->find_child("VarList");

	if (next_list){
		register_vars(next_list, type);
	}

}

void seman::check_call(node* var_list){
	if (!var_list){
		return;
	}

	
	node* id_node = var_list->find_child("id");
	if (id_node) {

		std::string name = id_node->value;

		if (sym_map.find(name) == nullptr) {
			errors.push_back("Semantic Error (Line " + std::to_string(id_node->line) +
				"): Undeclared variable '" + name + "' in function call ");
		}
		else {
			polska += name + " ";
		}
	}

	
	node* next_list = var_list->find_child("VarList");
	if (next_list) {
		check_call(next_list);
	}
}

void seman::check_operators(node* prog){

	node* operators = prog->find_child("Operators");

	while (operators) {

		node* op = operators->find_child("Op");

		if (op){
			check_op(op);
		}

		operators = operators->find_child("Operators");
	}
}

void seman::check_op(node* op){


	if (op->find_child("CALL")) {
		node* func_id = op->find_child("id");
		node* var_list = op->find_child("VarList");

		if (var_list) {
			check_call(var_list);
		}

		if (func_id) {

			std::string func_name = func_id->value;
			polska += func_name + " CALL \n";

		}
		return;
	}

	node* target = op->find_child("id");
	node* expr = op->find_child("Expr");

	if (target && expr) {
		std::string name = target->value;

		
		if (sym_map.find(name) == nullptr) {
			errors.push_back("Semantic Error (Line " + std::to_string(target->line) +
				"): Undeclared variable '" + name + "'.");
			return;
		}

		data_type lhs_type = sym_map[name];
		data_type rhs_type = expr_type(expr);

		
		if (lhs_type != rhs_type && rhs_type != UNKNOW) {
			errors.push_back("Semantic Error (Line " + std::to_string(target->line) +
				"): Type mismatch. Cannot assign " + dt_to_str(rhs_type) +
				" to " + dt_to_str(lhs_type) + " variable '" + name + "'.");
		}

		polska += name + " = \n";
	}

}

data_type seman::expr_type(node* expr){

	node* simple = expr->find_child("simple_expr");
	node* tail = expr->find_child("expr_tail");

	data_type left_type = simpl_expr_type(simple);

	if (tail) {
		return tail_type(tail, left_type);
	}

	return left_type;
}

data_type seman::simpl_expr_type(node* simpl_expr){

	if (!simpl_expr){
		return TYPE_ERROR;
	}

	
	node* id_node = simpl_expr->find_child("id");

	if (id_node) 	{
		
		std::string name = id_node->value;
		if (sym_map.find(name) == nullptr) {

			errors.push_back("Semantic Error (Line " + std::to_string(id_node->line) +
				"): Undeclared variable '" + name + "'.");

			return TYPE_ERROR;
		}

		polska += name + " "; 

		return sym_map[name];
	}

	node* const_node = simpl_expr->find_child("const"); 

	if (const_node) {

		std::string val = const_node->value;
		polska += val + " ";

		if (val.find('.') != std::string::npos){
			return TYPE_REAL;
		}
		return TYPE_INT;
	}

	
	node* bracket = simpl_expr->find_child("Expr");
	if (bracket) {
		return expr_type(bracket);
	}

	return TYPE_ERROR;
}

data_type seman::tail_type(node* tail_node, data_type prev_type){

	std::string op_str = "";
	node* op_node = nullptr;

	
	for (auto oper : tail_node->child) {
		if (oper->value == "+" || oper->value == "-") {
			op_node = oper;
			op_str = oper->value;
			break;
		}
	}

	if (!op_node){
		return prev_type;
	}

	node* simple = tail_node->find_child("simple_expr");
	data_type right_type = simpl_expr_type(simple);

	if (prev_type != right_type && prev_type != UNKNOW && right_type != UNKNOW) {
		errors.push_back("Semantic Error (Line " + std::to_string(op_node->line) +
			"): Operation '" + op_str + "' requires equal types. Got " +
			dt_to_str(prev_type) + " and " + dt_to_str(right_type) + ".");
	}

	polska += op_str + " ";

	
	node* next_tail = tail_node->find_child("expr_tail");
	if (next_tail) {
		return tail_type(next_tail, prev_type);
	}

	return prev_type;
}

void seman::start(node* root){

	check_program_name(root);
	check_declaration(root);
	check_operators(root);

}

std::string seman::get_rpn() const{
	return polska;
}


