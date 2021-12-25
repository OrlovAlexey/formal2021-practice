#include "Tests.h"
#include "../LR.h"

TEST(SimpleTest, test1) {
    Grammar G;
    G.set_term_alphabet() = "ab";
    G.set_neterm_alphabet() = "S";
    std::stringstream in;
    in << "2 S->SaSb S->";
    in >> G;

    LR lr1;
    lr1.fit(G);
    ASSERT_TRUE(lr1.predict("ab"));
    ASSERT_TRUE(lr1.predict("abab"));
    ASSERT_TRUE(lr1.predict("aabbab"));
    ASSERT_FALSE(lr1.predict("aaa"));
    ASSERT_FALSE(lr1.predict("aaabbbb"));
}

TEST(SimpleTest, test2) {
    Grammar G;
    G.set_term_alphabet() = "cd";
    G.set_neterm_alphabet() = "SC";
    std::stringstream in;
    in << "3 S->CC C->cC C->d";
    in >> G;

    LR lr1;
    lr1.fit(G);
    ASSERT_TRUE(lr1.predict("dd"));
    ASSERT_TRUE(lr1.predict("cdcd"));
    ASSERT_TRUE(lr1.predict("ccdccd"));
    ASSERT_FALSE(lr1.predict("cdcdcdcd"));
}

TEST(SimpleTest, test3) {
    Grammar G;
    G.set_term_alphabet() = "abc";
    G.set_neterm_alphabet() = "SB";
    std::stringstream in;
    in << "3 S->aB B->bc B->b";
    in >> G;

    LR lr1;
    lr1.fit(G);
    ASSERT_TRUE(lr1.predict("abc"));
    ASSERT_TRUE(lr1.predict("ab"));
    ASSERT_FALSE(lr1.predict("abab"));
}