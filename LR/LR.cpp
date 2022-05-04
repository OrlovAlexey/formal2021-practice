#include "LR.h"

char LR::Configuration::current_symbol() const {
    if (pos < rhs.size()) {
        return rhs[pos];
    }
    else {
        return EMPTY_SYMBOL;
    }
}

LR::Configuration::Configuration(const Grammar::Rule& rule, char b) : pos(0), lhs(rule.lhs), rhs(rule.rhs), next_terminal(b) {}

LR::Configuration::Configuration(int pos, char lhs, const std::string& rhs, char index) : pos(pos), lhs(lhs), rhs(rhs), next_terminal(index) {}

bool LR::Configuration::is_ending() const {
    return (pos >= rhs.size());
}

bool LR::Configuration::operator==(const LR::Configuration &other) const {
    return (pos == other.pos && lhs == other.lhs && rhs == other.rhs && next_terminal == other.next_terminal);
}

size_t LR::conf_hash::operator()(const LR::Configuration &conf) const {
    return std::hash<unsigned int>()(std::hash<std::string>()(conf.rhs) + std::hash<int>()(conf.pos));
}

bool LR::conf_eq::operator()(const LR::Configuration &conf1, const LR::Configuration &conf2) const {
    return (conf1 == conf2);
}

template <typename T>
std::unordered_set<T> operator+ (const std::unordered_set<T>& a, const std::unordered_set<T>& b) {
    std::unordered_set<T> copy = a;
    for (const T &value : b) {
        copy.insert(value);
    }
    return copy;
}

std::unordered_set<char> LR::FIRST(const std::string& string1, char previous_neterm) const {
    if (string1.empty()) {
        return std::unordered_set<char>();
    }
    for (char c : string1) {
        if (c == TERMINATING_SYMBOL) {
            return std::unordered_set<char>({TERMINATING_SYMBOL});
        }
        if (grammar.is_terminal(c)) {
            return std::unordered_set<char>({c});
        }
        if (grammar.is_neterminal(c)) {
            std::unordered_set<char> first;
            for (auto& rule : grammar.rules.at(c)) {
                if (rule.lhs == c && c != previous_neterm) {
                    first = first + FIRST(rule.rhs, c);
                }
            }
            if (!first.empty()) {
                return first;
            } else {
                continue;
            }
        }
    }
}

LR::set_conf LR::closure(const LR::set_conf& I) {
    set_conf J = I;
    bool flag = true;
    while (flag) {
        flag = false;
        set_conf Temp = J;
        for (auto& conf : Temp) {
            auto first = FIRST(conf.rhs.substr(std::min(conf.pos + 1, (int)conf.rhs.size())) + conf.next_terminal, EMPTY_SYMBOL);
            for (auto& rule : grammar.rules[conf.current_symbol()]) {
                for (char b : first) {
                    flag |= J.insert(Configuration(rule, b)).second;
                }
            }
        }
    }
    return J;
}

LR::set_conf LR::go_to(const LR::set_conf& I, char symbol) {
    set_conf J;
    for (auto& conf : I) {
        if (conf.current_symbol() == symbol) {
            auto new_conf = conf;
            new_conf.pos++;
            J.insert(new_conf);
        }
    }
    return closure(J);
}

size_t LR::vertex_hash::operator()(const LR::AutomatVertex &vertex) const {
    return std::hash<int>()(vertex.vertex_number);
}

bool LR::vertex_eq::operator()(const LR::AutomatVertex &vertex1, const LR::AutomatVertex &vertex2) const {
    return (vertex1.vertex_number == vertex2.vertex_number);
}

std::unordered_map<int, LR::AutomatVertex> LR::generate_situations() {
    std::unordered_map<int, AutomatVertex> Automat;
    set_conf temp;
    temp.insert(Configuration(0, PRESTARTING_NETERMINAL, STARTING_NETERMINAL, '$'));
    temp = closure(temp);
    Automat.insert(std::make_pair(0, AutomatVertex(0, temp, std::vector<int>())));
    int current_situation = 0;
    for (int cur_size = 1; current_situation < cur_size;) {
        AutomatVertex current_vertex = Automat[current_situation];
        LR_Table.emplace_back();
        for (char c : (grammar.get_neterm_alphabet() + grammar.get_term_alphabet())) {
            auto go_to1 = go_to(current_vertex.vertex_set, c);
            if (!go_to1.empty()) {
                bool is_already_a_vertex = false;
                for (int j = 0; j < current_situation; ++j) {
                    auto& other_vertex_set = Automat[j].vertex_set;
                    if (other_vertex_set == go_to1) {
                        is_already_a_vertex = true;
                        Automat[j].edges.push_back(j);
                        LR_Table[current_situation].insert(make_pair(c, "s" + std::to_string(j)));
                        break;
                    }
                }
                if (!is_already_a_vertex) {
                    Automat.insert(std::make_pair(cur_size, AutomatVertex(cur_size, go_to1, std::vector<int>())));
                    LR_Table[current_situation].insert(make_pair(c, "s" + std::to_string(cur_size)));
                    ++cur_size;
                }
            }
        }
        current_situation++;
    }
    return Automat;
}

void LR::fit(const Grammar& G) {
    grammar = G;
    std::unordered_map<int, AutomatVertex> Automat = generate_situations();
    std::string alphabet = grammar.get_neterm_alphabet() + grammar.get_term_alphabet() + TERMINATING_SYMBOL;
    auto EndingConfiguration = Configuration(1, PRESTARTING_NETERMINAL, STARTING_NETERMINAL, TERMINATING_SYMBOL);
    for (int vertex = 0; vertex < Automat.size(); ++vertex) {
        for (auto& conf : Automat[vertex].vertex_set) {
            if (conf.is_ending()) {
                if (conf.lhs != PRESTARTING_NETERMINAL) {
                    for (auto iter = grammar.rules[conf.lhs].begin(); iter != grammar.rules[conf.lhs].end(); ++iter) {
                        auto rule = *iter;
                        if (rule.lhs == conf.lhs && rule.rhs == conf.rhs) {
                            if (LR_Table[vertex].find(conf.next_terminal) != LR_Table[vertex].end()) {
                                throw std::invalid_argument("The given grammar does not fit LR(1)");
                            }
                            LR_Table[vertex][conf.next_terminal] = "r" + std::to_string(rule.number);
                        }
                    }
                }
            }
            if (EndingConfiguration == conf) {
                if (LR_Table[vertex].find(conf.next_terminal) != LR_Table[vertex].end()) {
                    throw std::invalid_argument("The given grammar does not fit LR(1)");
                }
                LR_Table[vertex][conf.next_terminal] = "acc";
            }
        }
    }
}

bool LR::predict(const std::string& word) const {
    std::string word_t = word + TERMINATING_SYMBOL;
    std::stack<std::string> path;
    path.push("&0");
    int i = 0;
    while (true) {
        char c = word_t[i]; // а от этой распаковки никак не избавится, она просто для краткости кода
        int state = stoi(path.top().substr(1));
        auto& cur_row = LR_Table[state];
        if (cur_row.find(c) == cur_row.end()) {
            return false;
        }
        std::string action = cur_row.at(c);
        if (action[0] == 'r') {
            int rule_number = stoi(action.substr(1));
            auto rule = grammar.get_rule(rule_number);
            for (int j = 0; j < (int)rule.rhs.length(); ++j) {
                path.pop();
            }
            state = stoi(path.top().substr(1));
            path.push(rule.lhs + LR_Table[state].at(rule.lhs).substr(1));
        }
        if (action[0] == 's') {
            ++i;
            path.push(c + action.substr(1));
        }
        if (action == "acc") {
            return true;
        }
    }
}

