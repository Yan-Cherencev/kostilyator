#include "syntex.h"

bool syntex::is_end()const{
    return pos>tokens.size()||tokens[pos].type==END_OF_FILE;
}

bool syntex::check(token_type type) const{
    if (is_end()){
        return false;
    }
    return current().type == type;
}

bool syntex::check(const std::string& val) const{
    if (is_end()) {
        return false;
    }
    return current().value == val;
}

const token& syntex::peek() const {
    if(!is_end()){
        return tokens[pos];
    }
    return tokens[pos+1];
}

const token& syntex::current() const{
    return tokens[pos];
}

const token& syntex::next() {
    if (!is_end()) {
        ++pos;
    }
    return tokens[pos - 1];
}

node* syntex::absorb(const token_type& tok, const std::string& rule, const std::string& error){
    if (check(tok)) {
        return new node(rule, next().value);
    }
    if (is_end()) {
        errors.push_back("Line ? : Unexpected End Of File. " + error);
        //throw std::runtime_error("Line ? : Unexpected End Of File. " + error);
        return nullptr;
    }

    errors.push_back("Line " + std::to_string(current().line) + ": " + error + ", got '" + current().value + "'");
    //throw std::runtime_error("Line " + std::to_string(current().line) + ": " + error +", got '" + current().value + "'");
    return nullptr;
}

node* syntex::absorb(const std::string& val, const std::string& rule, const std::string& error){
    if (check(val)) {
        return new node(rule, next().value);
    }

    if (is_end()) {
        errors.push_back("Line ? : Unexpected End Of File. " + error);
        //throw std::runtime_error("Line ? : Unexpected End Of File. " + error);
        return nullptr;
    }
    errors.push_back("Line " + std::to_string(current().line) + ": " + error);
    //throw std::runtime_error("Line " + std::to_string(current().line) + ": " + error);
    return nullptr;
}


//--------------------------------------------------

// Program → Begin Descriptions Operators End
node* syntex::program(){

    node* node_ = new node("Program");

    node_->add(begin());
    node_->add(descriptions());
    node_->add(operators());
    node_->add(end());

    return node_;
}

// Begin → PROGRAM Id
node* syntex::begin(){

    node* node_ = new node("Begin");

    node_->add(absorb(PROGRAM, "PROGRAM", "expected PROGRAM"));
    node_->add(absorb(ID, "id", "expected Program id/name"));

    //node_->add()

    return node_;
}

// End → END Id
node* syntex::end(){
    node* node_ = new node("End");

    node_->add(absorb(END, "END", "expected end"));
    node_->add(absorb(ID, "id", "expected Program End"));


    return node_;
}

// Descriptions → Descr | Descr Descriptions
node* syntex::descriptions() {

    node* node_ = new node("Descriptions");
    node_->add(descr());

    if ((check("INT") || check("REAL"))) {
        node_->add(descriptions());
    }

    return node_; 
}

// Descr → Type VarList
node* syntex::descr(){

    node* node_ = new node("Descr");

    node_->add(type());
    node_->add(var_list());

    return node_;
}

// Type → INTEGER | REAL
node* syntex::type(){

    node* node_ = new node("Type");

    if (check("INT")){
        node_->add(absorb("INT", "INT", "exepted Type"));
    }
    else if(check("REAL")){
        node_->add(absorb("REAL", "REAL", "exepted Type"));
    }
    else {
        errors.push_back("Line " + std::to_string(current().line) + ": Expected type (INT or REAL)");
        //throw std::runtime_error("Line " + std::to_string(current().line) + ": Expected type (INT or REAL)");
        return nullptr;
    }
    //node_->add(type());
    //node_->add(var_list());

    return node_;
}

// VarList → Id | Id , VarList
node* syntex::var_list() {
    node* node_ = new node("VarList");

    node_->add(absorb(ID, "id", "expected  id"));

    if (check(",")) {

        node_->add(absorb(PUNCTUATOR, ",", "Expected <<,>>"));
        node_->add(var_list());

    }
    return node_;
}

// Operators → Op | Op Operators
node* syntex::operators(){

    node* node_ = new node("Operators");

    node_->add(op());

    if (check(ID)||check(CALL) ) {
        //node_->add(absorb(PUNCTUATOR, "punctuator", "Expected <<,>>"));
        node_->add(operators());
    }
    //node_->add(operators());

    return node_;
}

// Op → Id = Expr | CALL Id ( VarList )
node* syntex::op(){

    node* node_ = new node("Op");
    

    if (check(CALL)) {

        node_->add(absorb(CALL, "CALL", "expected  call"));
        node_->add(absorb(ID, "id", "expected funck  id"));

        node_->add(absorb(PUNCTUATOR, "(", "expected  <<(>>"));
        node_->add(var_list());
        node_->add(absorb(PUNCTUATOR, ")", "expected  <<)>>"));

        return node_;
    }
    else if(check(ID)){
        node_->add(absorb(ID, "id", "expected  id"));
        node_->add(absorb(OPERATOR, "=", "expected  <<=>>"));

        node_->add(expr());
    }
    else {
        errors.push_back("Line " + std::to_string(current().line) + ": Expected Statement (ID=... or CALL)");
        //throw std::runtime_error("Line " + std::to_string(current().line) + ": Expected Statement (ID=... or CALL)");
        return nullptr;
    }

    return node_;
}

// SimpleExpr → Id | Const | ( Expr )
node* syntex::simple_expr(){
    node* node_ = new node("simple_expr");

    if (check(ID)) {
        node_->add(absorb(ID, "id", "expected id"));
    }
    else if (check(INTEGER_LIT)) {
        node_->add(absorb(INTEGER_LIT, "const", "expected int"));
    }
    else if (check(FLOAT_LIT)) {
        node_->add(absorb(FLOAT_LIT, "const", "expected real"));
    }
    else if (check("(")) {
        node_->add(absorb(PUNCTUATOR, "(", "expected <<(>>"));
        node_->add(expr());
        node_->add(absorb(PUNCTUATOR, ")", "expected <<)>>"));
    }
    else {
        errors.push_back("Line " + std::to_string(current().line) + ": Invalid Expression start");
        //throw std::runtime_error("Line " + std::to_string(current().line) + ": Invalid Expression start");
        return nullptr;
    }

    return node_;
}

// Expr → SimpleExpr ExpTail
node* syntex::expr(){
    node* node_ = new node("Expr");

    node_->add(simple_expr());
    node_->add(expr_tail());

    return node_;
}

// ExpTail → + SimpleExpr ExpTail | - SimpleExpr ExpTail | \0
node* syntex::expr_tail(){
    node* node_ = new node("expr_tail");

    if (check("+")) {
        node_->add(absorb(OPERATOR, "+", "expected <+>"));
        node_->add(simple_expr());
        node_->add(expr_tail());
    }
    else if (check("-")) {
        node_->add(absorb(OPERATOR, "-", "expected <->"));
        node_->add(simple_expr());
        node_->add(expr_tail());
    }

    return node_;
}

// Конструктор
syntex::syntex(const std::vector<token>& tokens, std::vector<std::string>& errors) :tokens(tokens), errors(errors), pos(0) {}


// Начало анаилза
node* syntex::start(){
    return program();
}
