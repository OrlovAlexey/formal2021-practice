#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// this is CF grammar
class Grammar {
    std::string term_alphabet;
    std::string neterm_alphabet;
public:
    int number_of_rules;
    struct Rule {
        char lhs;
        std::string rhs;
        int number;
    };
    Rule get_rule(int number) const;
    std::unordered_map<char, std::vector<Rule>> rules;
    void add_rule(Rule& rule1);
    std::string& set_term_alphabet();
    std::string& set_neterm_alphabet();
    std::string get_term_alphabet() const;
    std::string get_neterm_alphabet() const;
    bool is_terminal(char c) const;
    bool is_neterminal(char c) const;
    void clear();
};

bool check_term_char(char c, const std::string& term_alphabet);

void check_rule(const Grammar::Rule& rule, const std::string& term_alphabet, const std::string& neterm_alphabet);

std::istream& operator>>(std::istream& in, Grammar::Rule& rule);

std::ostream& operator<< (std::ostream& out, const Grammar::Rule& rule);


std::istream& operator>>(std::istream& in, Grammar& grammar);

void grammar_std_input(Grammar& G);

