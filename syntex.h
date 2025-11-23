#pragma once

#include <vector>

#include "token.h"
#include "drevo.h"

class syntex{
	const std::vector<token> &tokens;
    std::vector<std::string> &errors;
	size_t pos;

private:

	bool is_end()const;
    bool check(token_type type)const;
    bool check(const std::string& val)const;

    const token& peek()const;
    const token& current()const;
    const token& next();

    node* absorb(const token_type&tok, const std::string& rule, const std::string& error);
    node* absorb(const std::string& val, const std::string& rule, const std::string& error);

    node* program();
    node* begin();
    node* end();
    node* descriptions();
    node* descr();
    node* type();
    node* var_list();
    node* operators();
    node* op();

    node* simple_expr();
    node* expr();
    node* expr_tail();

public:
    syntex(const std::vector<token>& tokens, std::vector<std::string>& errors);

    node* start();

};

