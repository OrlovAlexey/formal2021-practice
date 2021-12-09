#include "Grammar.h"

bool Grammar::is_terminal(char c) const {
    for (auto letter : term_alphabet) {
        if (letter == c) {
            return true;
        }
    }
    return false;
}

bool Grammar::is_neterminal(char c) const {
    for (auto letter : neterm_alphabet) {
        if (letter == c) {
            return true;
        }
    }
    return false;
}

string& Grammar::set_term_alphabet() {
    return term_alphabet;
}

string& Grammar::set_neterm_alphabet() {
    return neterm_alphabet;
}

string Grammar::get_term_alphabet() const {
    return term_alphabet;
}

string Grammar::get_neterm_alphabet() const {
    return neterm_alphabet;
}

void Grammar::add_rule(const Grammar::Rule& rule1) {
    rules.push_back(rule1);
}

void Grammar::clear() {
    rules.clear();
}

bool check_term_char(char c, const string& term_alphabet) {
    for (auto symbol : term_alphabet) {
        if (c == symbol) {
            return true;
        }
    }
    return false;
} // can also be used for checking neterminal

void check_rule(const Grammar::Rule& rule, const string& term_alphabet, const string& neterm_alphabet) {
    if (!check_term_char(rule.lhs, neterm_alphabet)) {
        throw std::invalid_argument("The symbol is not in the alphabet!");
    }
    for (char c : rule.rhs) {
        if (!check_term_char(c, term_alphabet) && !check_term_char(c, neterm_alphabet)) {
            throw std::invalid_argument("The symbol is not in the alphabet!");
        }
    }
}

std::istream& operator>>(std::istream& in, Grammar::Rule& rule) {
    string temp;
    in >> temp;
    rule.lhs = temp[0];
    rule.rhs = temp.substr(3);
    return in;
}

std::ostream& operator<< (std::ostream& out, const Grammar::Rule& rule){
    out << rule.lhs << "->" << rule.rhs << '\n';
    return out;
}

std::istream& operator>> (std::istream& in, Grammar& grammar) {
    int number_of_rules;
    in >> number_of_rules;
    grammar.clear();
    Grammar::Rule rule;
    for (int i = 0; i < number_of_rules; ++i) {
        in >> rule;
        check_rule(rule, grammar.get_term_alphabet(), grammar.get_neterm_alphabet());
        grammar.add_rule(rule);
    }
    return in;
}

void grammar_std_input(Grammar& G) { // interactive
    cout << "Enter terminal alphabet:\n";
    string term_alphabet;
    cin >> term_alphabet;
    G.set_term_alphabet() = term_alphabet;

    cout << "Enter neterminal alphabet:\n";
    string neterm_alphabet;
    cin >> neterm_alphabet;
    G.set_neterm_alphabet() = neterm_alphabet;

    cout << "Enter number of rules:\n";
    int number_of_rules;
    cin >> number_of_rules;

    cout << "Now enter rules in format \"S->AbScD\" (for empty symbol do not enter anything)\n";
    Grammar::Rule input_rule;
    for (int i = 0; i < number_of_rules; ++i) {
        cin >> input_rule;
        check_rule(input_rule, G.get_term_alphabet(), G.get_neterm_alphabet());
        G.add_rule(input_rule);
    }
}