#include "Tests.h"
#include "../Earley.h"

TEST(SimpleTest, test1) {
    Grammar G;
    G.set_term_alphabet() = "ab";
    G.set_neterm_alphabet() = "S";
    stringstream in;
    in << "2 S->aSbS S->";
    in >> G;
    Earley earley1;
    earley1.fit(G);
    ASSERT_TRUE(earley1.predict(""));
    ASSERT_TRUE(earley1.predict("ab"));
    ASSERT_TRUE(earley1.predict("aaaaaabbbbbb"));
    ASSERT_TRUE(earley1.predict("abababab"));
    ASSERT_FALSE(earley1.predict("aab"));
}

TEST(SimpleTest, test2) {
    Grammar G;
    G.set_term_alphabet() = "abc";
    G.set_neterm_alphabet() = "STR";
    stringstream in;
    in << "5 S->aT T->bT T->cR R->cR R->a";
    in >> G;
    Earley earley1;
    earley1.fit(G);
    ASSERT_TRUE(earley1.predict("abca"));
    ASSERT_TRUE(earley1.predict("abbbbbcca"));
    ASSERT_FALSE(earley1.predict("aba"));
    ASSERT_FALSE(earley1.predict(""));
}

TEST(SimpleTest, test3) {
    Grammar G;
    G.set_term_alphabet() = "(){}[]";
    G.set_neterm_alphabet() = "S";
    stringstream in;
    in << "5 S->(S) S->{S} S->[S] S->SS S->";
    in >> G;
    Earley earley1;
    earley1.fit(G);
    ASSERT_TRUE(earley1.predict("({[]})"));
    ASSERT_TRUE(earley1.predict("(()[]{}){}"));
    ASSERT_FALSE(earley1.predict("[{(})]"));
}

TEST(GrammarTests, is_terminal_test) {
    Grammar G;
    G.set_term_alphabet() = "abcd+*123";
    ASSERT_TRUE(G.is_terminal('a'));
    ASSERT_TRUE(G.is_terminal('+'));
    ASSERT_TRUE(G.is_terminal('3'));
    ASSERT_FALSE(G.is_terminal('4'));
    ASSERT_FALSE(G.is_terminal('-'));
    ASSERT_FALSE(G.is_terminal('e'));
}

TEST(GrammarTests, is_neterminal_test) {
    Grammar G;
    G.set_neterm_alphabet() = "SABCD";
    ASSERT_TRUE(G.is_neterminal('S'));
    ASSERT_TRUE(G.is_neterminal('A'));
    ASSERT_FALSE(G.is_neterminal('9'));
    ASSERT_FALSE(G.is_neterminal('E'));
}

TEST(GrammarTests, get_and_set_term_alphabet_test) {
    Grammar G;
    G.set_term_alphabet() = "abcd";
    ASSERT_EQ("abcd", G.get_term_alphabet());
    ASSERT_NE("abcde", G.get_term_alphabet());
    G.set_term_alphabet() = "123";
    ASSERT_EQ("123", G.get_term_alphabet());
    ASSERT_NE("abcd123", G.get_term_alphabet());
}

TEST(GrammarTests, get_and_set_neterm_alphabet_test) {
    Grammar G;
    G.set_neterm_alphabet() = "STMN";
    ASSERT_EQ("STMN", G.get_neterm_alphabet());
    ASSERT_NE("STMNR", G.get_neterm_alphabet());
    G.set_neterm_alphabet() = "S123";
    ASSERT_EQ("S123", G.get_neterm_alphabet());
    ASSERT_NE("STMNS123", G.get_neterm_alphabet());
}


TEST(GrammarTests, add_rule_test) {
    Grammar G;
    Grammar::Rule rule1({'S', "Sabc"});
    Grammar::Rule rule2({'S', "SaSaS"});
    Grammar::Rule rule3({'T', "ST"});
    G.add_rule(rule1);
    G.add_rule(rule2);
    G.add_rule(rule3);
    ASSERT_EQ(rule1.lhs, G.rules[0].lhs);
    ASSERT_EQ(rule1.rhs, G.rules[0].rhs);
    ASSERT_EQ(rule2.lhs, G.rules[1].lhs);
    ASSERT_EQ(rule2.rhs, G.rules[1].rhs);
    ASSERT_EQ(rule3.lhs, G.rules[2].lhs);
    ASSERT_EQ(rule3.rhs, G.rules[2].rhs);
}

TEST(GrammarTests, check_term_char_test) {
    ASSERT_TRUE(check_term_char('a', "abc"));
    ASSERT_TRUE(check_term_char('f', "agsfda"));
    ASSERT_FALSE(check_term_char('p', "agsfda"));
}

TEST(GrammarTests, Rule_input_output_test) {
    stringstream in;
    string rulestr = "S->SaSb";
    in << rulestr;
    Grammar::Rule rule;
    in >> rule;
    stringstream out;
    out << rule;
    string outstr;
    out >> outstr;
    ASSERT_EQ(outstr, rulestr);
}

TEST(GrammarTests, Grammar_input_test) {
    Grammar G;
    G.set_term_alphabet() = "abc";
    G.set_neterm_alphabet() = "S";
    stringstream in;
    in << "2 S-> S->Sabc";
    in >> G;
    ASSERT_EQ(G.rules[0].lhs, Grammar::Rule({'S', ""}).lhs);
    ASSERT_EQ(G.rules[0].rhs, Grammar::Rule({'S', ""}).rhs);
    ASSERT_EQ(G.rules[1].lhs, Grammar::Rule({'S', "Sabc"}).lhs);
    ASSERT_EQ(G.rules[1].rhs, Grammar::Rule({'S', "Sabc"}).rhs);
}

TEST(EarleyAlgoTests, Scan_test) {
    Grammar G;
    G.set_term_alphabet() = "abc";
    G.set_neterm_alphabet() = "S";
    stringstream in;
    in << "2 S->aSbScS S->";
    in >> G;

    Earley earley;
    earley.fit(G);
    string word = "abc";
    earley.handling_word = word;
    vector<Earley::set_conf> D(word.length() + 1);
    D[0].insert(Earley::Configuration({0, PRESTARTING_NETERMINAL, STARTING_NETERMINAL, 0}));
    D[0].insert(Earley::Configuration({0, STARTING_NETERMINAL[0], "", 0}));
    D[0].insert(Earley::Configuration({0, STARTING_NETERMINAL[0], "aSbScS", 0}));
    D[0].insert(Earley::Configuration({1, PRESTARTING_NETERMINAL, STARTING_NETERMINAL, 0}));
    earley.Scan(D, 1);
    ASSERT_EQ(1, D[1].size());
    for (auto& conf : D[1]) {
        if (!(conf == Earley::Configuration({1, STARTING_NETERMINAL[0], "aSbScS", 0}))) {
            ASSERT_EQ(0, 1);
        }
    }
}

TEST(EarleyAlgoTests, Predict_test) {
    Grammar G;
    G.set_term_alphabet() = "a+*()";
    G.set_neterm_alphabet() = "S";
    stringstream in;
    in << "4 S->S+S S->S*S S->(S) S->a";
    in >> G;

    Earley earley;
    earley.fit(G);
    string word = "(a+a)*a";
    earley.handling_word = word;
    vector<Earley::set_conf> D(word.length() + 1);
    D[0].insert(Earley::Configuration({0, PRESTARTING_NETERMINAL, STARTING_NETERMINAL, 0}));
    D[0].insert(Earley::Configuration({0, 'S', "S+S", 0}));
    D[0].insert(Earley::Configuration({0, 'S', "S*S", 0}));
    D[0].insert(Earley::Configuration({0, 'S', "(S)", 0}));
    D[0].insert(Earley::Configuration({0, 'S', "a", 0}));
    D[1].insert(Earley::Configuration({1, 'S', "(S)", 0}));
    earley.Predict(D, 1);
    ASSERT_EQ(5, D[1].size());
    bool flag;
    for (auto& conf : D[1]) {
        flag = false;
        if (conf == Earley::Configuration({1, 'S', "(S)", 0})) {
            flag = true;
        }
        if (conf == Earley::Configuration({0, 'S', "S+S", 1})) {
            flag = true;
        }
        if (conf == Earley::Configuration({0, 'S', "S*S", 1})) {
            flag = true;
        }
        if (conf == Earley::Configuration({0, 'S', "(S)", 1})) {
            flag = true;
        }
        if (conf == Earley::Configuration({0, 'S', "a", 1})) {
            flag = true;
        }
        if (!flag) {
            ASSERT_EQ(0, 1);
        }
    }
}

TEST(EarleyAlgoTests, Complete_test) {
    Grammar G;
    G.set_term_alphabet() = "a+*()";
    G.set_neterm_alphabet() = "S";
    stringstream in;
    in << "4 S->S+S S->S*S S->(S) S->a";
    in >> G;

    Earley earley;
    earley.fit(G);
    string word = "(a+a)*a";
    earley.handling_word = word;
    vector<Earley::set_conf> D(word.length() + 1);
    D[0].insert(Earley::Configuration({0, PRESTARTING_NETERMINAL, STARTING_NETERMINAL, 0}));
    D[0].insert(Earley::Configuration({0, 'S', "S+S", 0}));
    D[0].insert(Earley::Configuration({0, 'S', "S*S", 0}));
    D[0].insert(Earley::Configuration({0, 'S', "(S)", 0}));
    D[0].insert(Earley::Configuration({0, 'S', "a", 0}));
    D[1].insert(Earley::Configuration({1, 'S', "(S)", 0}));
    D[1].insert(Earley::Configuration({0, 'S', "S+S", 1}));
    D[1].insert(Earley::Configuration({0, 'S', "S*S", 1}));
    D[1].insert(Earley::Configuration({0, 'S', "(S)", 1}));
    D[1].insert(Earley::Configuration({0, 'S', "a", 1}));
    D[2].insert( Earley::Configuration({1, 'S', "a", 1}));
    earley.Complete(D, 2);
    ASSERT_EQ(4, D[2].size());
    bool flag;
    for (auto& conf : D[2]) {
        flag = false;
        if (conf == Earley::Configuration({1, 'S', "a", 1})) {
            flag = true;
        }
        if (conf == Earley::Configuration({1, 'S', "S+S", 1})) {
            flag = true;
        }
        if (conf == Earley::Configuration({1, 'S', "S*S", 1})) {
            flag = true;
        }
        if (conf == Earley::Configuration({2, 'S', "(S)", 0})) {
            flag = true;
        }
        if (!flag) {
            ASSERT_EQ(0, 1);
        }
    }
}