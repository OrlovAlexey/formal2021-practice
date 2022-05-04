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

std::string& Grammar::set_term_alphabet() {
    return term_alphabet;
}

std::string& Grammar::set_neterm_alphabet() {
    return neterm_alphabet;
}

std::string Grammar::get_term_alphabet() const {
    return term_alphabet;
}

std::string Grammar::get_neterm_alphabet() const {
    return neterm_alphabet;
}

void Grammar::add_rule(Grammar::Rule& rule1) {
    rule1.number = ++number_of_rules;
    rules[rule1.lhs].push_back(rule1);
}

void Grammar::clear() {
    rules.clear();
}

Grammar::Rule Grammar::get_rule(int number) const {
    for (const auto& batch : rules) {
        for (auto rule : batch.second) {
            if (rule.number == number) {
                return rule;
            }
        }
    }
}

bool check_term_char(char c, const std::string& term_alphabet) {
    for (auto symbol : term_alphabet) {
        if (c == symbol) {
            return true;
        }
    }
    return false;
} // can also be used for checking neterminal

void check_rule(const Grammar::Rule& rule, const std::string& term_alphabet, const std::string& neterm_alphabet) {
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
    std::string temp;
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
    std::cout << "Enter terminal alphabet:\n";
    std::string term_alphabet;
    std::cin >> term_alphabet;
    G.set_term_alphabet() = term_alphabet;

    std::cout << "Enter neterminal alphabet:\n";
    std::string neterm_alphabet;
    std::cin >> neterm_alphabet;
    G.set_neterm_alphabet() = neterm_alphabet;

    std::cout << "Enter number of rules:\n";
    int number_of_rules;
    std::cin >> number_of_rules;

    std::cout << "Now enter rules in format \"S->AbScD\" (for empty symbol do not enter anything)\n";
    Grammar::Rule input_rule;
    for (int i = 0; i < number_of_rules; ++i) {
        std::cin >> input_rule;
        check_rule(input_rule, G.get_term_alphabet(), G.get_neterm_alphabet());
        G.add_rule(input_rule);
    }
}