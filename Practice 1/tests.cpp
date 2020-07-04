#include "gtest/gtest.h"
#include "main.cpp"

TEST(Test_1, 1){
    EXPECT_EQ(3, LenthOfTheLCS("ab+c.aba.*.bac.+.+*", "babc"));
};

TEST(Test_1, 2){
    EXPECT_EQ(9, LenthOfTheLCS("ab+c.aba.*.bac.+.+*", "bbacabaacb"));
};

TEST(Test_2, 1){
    EXPECT_EQ(5, LenthOfTheLCS("acb..bab.c.*.ab.ba.+.+*a.", "abbaa"));
};

TEST(Test_2, 2){
    EXPECT_EQ(9, LenthOfTheLCS("acb..bab.c.*.ab.ba.+.+*a.", "acababcabac"));
};

TEST(Test_3, 1){
    EXPECT_EQ(3, LenthOfTheLCS("bab.+*bab.+*a.+", "cabaaabac"));
};

TEST(Test_3, 2){
    EXPECT_EQ(7, LenthOfTheLCS("bab.+*bab.+*a.+", "ababbbac"));
};

TEST(Test_4, 1){
    EXPECT_EQ(5, LenthOfTheLCS("ab.*a.*ca.*c.*+*ab+.c.", "ababcc"));
};

TEST(Test_4, 2){
    EXPECT_EQ(4, LenthOfTheLCS("ab.*a.*ca.*c.*+*ab+.c.", "cabcacc"));
};

TEST(Test_5, 1){
    EXPECT_EQ(5, LenthOfTheLCS("a*a*b.1aa*.b.+*a.a*.+", "aaaabbaa"));
};

TEST(Test_5, 2){
    EXPECT_EQ(8, LenthOfTheLCS("a*a*b.1aa*.b.+*a.a*.+", "aaabababb"));
};


int main(int argc,char**argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
