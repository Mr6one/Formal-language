#include "main.cpp"

TEST(Test_1, 1){
    vector<pair<char, string>> grammar;
    set<char> nonTerminated;
    grammar.push_back(make_pair('S', "T"));
    grammar.push_back(make_pair('T', "(T)T"));
    grammar.push_back(make_pair('T', ""));
    nonTerminated.insert('S');
    nonTerminated.insert('T');
    LRParser parser(grammar, nonTerminated);
    EXPECT_NO_THROW(parser.BuildTable());
    EXPECT_TRUE(parser.CheckWord("()()"));
    EXPECT_TRUE(parser.CheckWord("(())()"));
    EXPECT_FALSE(parser.CheckWord("(()()"));
    EXPECT_FALSE(parser.CheckWord(")((())()"));
};

TEST(Test_1, 2){
    vector<pair<char, string>> grammar;
    set<char> nonTerminated;
    grammar.push_back(make_pair('S', "aSa"));
    grammar.push_back(make_pair('S', ""));
    nonTerminated.insert('S');
    LRParser parser(grammar, nonTerminated);
    EXPECT_ANY_THROW(parser.BuildTable());
};

TEST(Test_1, 3){
    vector<pair<char, string>> grammar;
    set<char> nonTerminated;
    grammar.push_back(make_pair('S', "E"));
    grammar.push_back(make_pair('E', "E+T"));
    grammar.push_back(make_pair('E', "T"));
    grammar.push_back(make_pair('T', "T*F"));
    grammar.push_back(make_pair('T', "F"));
    grammar.push_back(make_pair('F', "a"));
    grammar.push_back(make_pair('F', "(E)"));
    nonTerminated.insert('S');
    nonTerminated.insert('E');
    nonTerminated.insert('T');
    nonTerminated.insert('F');
    LRParser parser(grammar, nonTerminated);
    EXPECT_NO_THROW(parser.BuildTable());
    EXPECT_TRUE(parser.CheckWord("a*(a*a+a)"));
    EXPECT_TRUE(parser.CheckWord("a*(a*a+a)+((a+a)*a+a)*a"));
    EXPECT_FALSE(parser.CheckWord("a*(a*a+a)+"));
    EXPECT_FALSE(parser.CheckWord("a*(a*a+a)+((a+a)*a+a)*"));
};

TEST(Test_2, 1){
    vector<pair<char, string>> grammar;
    set<char> nonTerminated;
    grammar.push_back(make_pair('S', "C"));
    grammar.push_back(make_pair('S', "CS"));
    grammar.push_back(make_pair('C', "Dc"));
    grammar.push_back(make_pair('D', "aDb"));
    grammar.push_back(make_pair('D', ""));
    nonTerminated.insert('S');
    nonTerminated.insert('D');
    nonTerminated.insert('C');
    LRParser parser(grammar, nonTerminated);
    EXPECT_NO_THROW(parser.BuildTable());
    EXPECT_TRUE(parser.CheckWord("aaabbbcaabbc"));
    EXPECT_TRUE(parser.CheckWord("aabbcccaaabbbccabcc"));
    EXPECT_FALSE(parser.CheckWord("aabbcccaaabbb"));
    EXPECT_FALSE(parser.CheckWord("abdccabcc"));
};

TEST(Test_2, 2){
    vector<pair<char, string>> grammar;
    set<char> nonTerminated;
    grammar.push_back(make_pair('S', "Sa"));
    grammar.push_back(make_pair('S', "Dc"));
    grammar.push_back(make_pair('S', "SaSb"));
    grammar.push_back(make_pair('S', "C"));
    grammar.push_back(make_pair('C', "dD"));
    grammar.push_back(make_pair('D', ""));
    nonTerminated.insert('S');
    nonTerminated.insert('C');
    nonTerminated.insert('D');
    LRParser parser(grammar, nonTerminated);
    EXPECT_NO_THROW(parser.BuildTable());
    EXPECT_TRUE(parser.CheckWord("caaadb"));
    EXPECT_TRUE(parser.CheckWord("daaadacbacadbbb"));
    EXPECT_FALSE(parser.CheckWord("daaada"));
    EXPECT_FALSE(parser.CheckWord("daaadacabbacadbbb"));
};
