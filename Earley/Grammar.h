#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// this is CF grammar
class Grammar {
    string term_alphabet;
    string neterm_alphabet;
public:
    struct Rule {
        char lhs;
        string rhs;
    };
    vector<Rule> rules;
    void add_rule(const Rule& rule1);
    string& set_term_alphabet();
    string& set_neterm_alphabet();
    string get_term_alphabet() const;
    string get_neterm_alphabet() const;
    bool is_terminal(char c) const;
    bool is_neterminal(char c) const;
    void clear();
};

bool check_term_char(char c, const string& term_alphabet);

void check_rule(const Grammar::Rule& rule, const string& term_alphabet, const string& neterm_alphabet);

std::istream& operator>>(std::istream& in, Grammar::Rule& rule);

std::ostream& operator<< (std::ostream& out, const Grammar::Rule& rule);


std::istream& operator>>(std::istream& in, Grammar& grammar);

void grammar_std_input(Grammar& G);

