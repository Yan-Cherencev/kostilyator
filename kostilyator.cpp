//PROGRAM test
//INT a, b, res
//REAL x, y
//a = 10
//b = 20
//x = 5.5
//y = 4.5
//res = a + b - 5
//x = x + y
//END test


//PROGRAM errors
//INT a, a
//REAL x
//b = 10
//a = 5.5
//END noerosr

#include <iostream>
#include <fstream>
#include <string>

//#include "dvaque.h"
#include "lexan.h"
#include "hable.h"
#include "syntex.h"
#include "seman.h"
//#include "hable.h"




int main() {
    //std::cout << "Hello World!\n";
    lexan analizator;
    //hable<std::string,token, hash1> lexems;

    std::vector<token> tokens;
    std::vector<std::string> synt_errors;


    std::ifstream in("in.txt");

    /*if (!in.is_open()) {
        std::cout << "File is not found\n";
        return 0;
    }*/

    std::ofstream out("out.txt");

    /*if (!out.is_open()) {
        std::cout << "File is not found\n";
        return 0;
    }*/

    std::string temp;
    size_t line_num = 1;
    while (std::getline(in, temp)) {
        analizator.load(temp);

        token cur_token;

        while ((cur_token = analizator.get_next_token()).type != END_OF_FILE) {


            if (cur_token.type == UNKNOW) {
                out << "Lex error on line " << line_num << ": Invalid token <<" << cur_token.value << ">>\n";
            }
            else {
                cur_token.line = line_num;
                tokens.push_back(cur_token);
            }
        }
        ++line_num;
    }

    tokens.push_back(token(END_OF_FILE, "", line_num));
    in.close();

    syntex synt_analiz(tokens, synt_errors);

    node* root = synt_analiz.start();

    for (auto& it : synt_errors) {
        /*if (it[5] == '?') {
            it[5] = line_num;
        }*/
        out << it << '\n';
    }

    if (synt_errors.empty()) {
        //root->print(out);

        std::vector<std::string> semant_errors;

        if (root) {
            seman semanaliz(semant_errors);

            semanaliz.start(root);
            if (!semant_errors.empty()) {
                std::cerr << "Semantyk ERRORS" << "\n";
                //out << "\n";
                for (const auto& err : semant_errors) {
                    out << err << "\n";
                    //std::cerr << err << "\n";
                }
            }
            else {

                std::cout << "Compilation Successful!\n";


                out << "Polska: \n";
                out << semanaliz.get_rpn() << "\n";
            }
        }



        //dvaque<hable<std::string, token, hash1>::para> lexeme_list = lexems.get_all();

        /*for (size_t i = 0; i < lexeme_list.size(); ++i) {
            const token& t = lexeme_list[i].val;
            out << t.type << " | " << t.value << " | " << lexems.index(t.value) << "\n";
        }*/



        out.close();

        //std::cout << "Success\n";
        return 0;
    }
}

