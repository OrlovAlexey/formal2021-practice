#include <iostream>
#include "LR.h"

int main() {
    Grammar G;
    grammar_std_input(G);

    LR lr1;
    lr1.fit(G);
    std::cout << "Now enter the required word:\n";
    std::string word;
    std::cin >> word;
    std::cout << (lr1.predict(word) ? "The word can be deducted from this grammar!\n" : "The word can not be deducted from this grammar!\n" );

    return 0;
}
