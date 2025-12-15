#include "drevo.h"

node::~node(){
	for (auto& it : child) {
		delete it;
	}
}


void node::add(node* childe){
	if (childe) {
		child.push_back(childe);
	}
}

void node::print(std::ofstream& out, size_t height){

    for (int i = 0; i < height; ++i){
        out << "  ";
    }

    out << this->rule;

    if (!this->value.empty()) {
        out << " (" << this->value << ")";
    }

    out << "\n";

    for (auto child : this->child) {
        child->print(out, height + 1);
    }
}

node* node::find_child(const std::string& name){ 
    for (auto it : child) { 
        if (it->rule == name) {
            return it;
        }
    }

    return nullptr;
}
