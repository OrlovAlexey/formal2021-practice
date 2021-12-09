#include <iostream>
#include <sstream>

#include "Earley.h"

using namespace std;

int main() {
    Grammar G;
    grammar_std_input(G);

    Earley earley1;
    earley1.fit(G);
    cout << "Now enter the required word:\n";
    string word;
    cin >> word;
    cout << (earley1.predict(word) ? "The word can be deducted from this grammar!\n" : "The word can not be deducted from this grammar!\n" );
    return 0;
}
