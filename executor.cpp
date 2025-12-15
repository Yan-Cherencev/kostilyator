#include "executor.h"

bool executor::is_digit(char lit){
    return lit>='0'&&lit<='9';
}


void executor::execut(const std::string& rpn) {

    for (int i = 0; i < rpn.size(); ++i) {
        if (rpn[i] == ' ') {
            ++i;
            continue;
        }
        if (is_digit(rpn[i])) {
            std::string num = "";

            while (i < rpn.size() && is_digit(rpn[i])) {
                num += rpn[i];
                ++i;
            }

            stck.push(std::stod(num));
        }
        else {
            long double a = stck.top();
            stck.pop();
            long double b = stck.top();
            stck.pop();

            switch (rpn[i]) {
            case '+':
                stck.push(a + b);
                break;
            case '-':
                stck.push(b - a);
                break;
                ++i;
            }
        }
    }
}

   
