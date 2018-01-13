#include <algorithm>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>
#include "any_iterator.h"

#include <gtest/gtest.h>

TEST(correctness, empty)
{
    any_forward_iterator<int> a;
    any_bidirectional_iterator<int> b;
    any_random_access_iterator<int> c;
}

TEST(correctness, empty_copy)
{
    any_forward_iterator<int> a;
    (any_forward_iterator<int>(a));
    any_bidirectional_iterator<int> b;
    (any_bidirectional_iterator<int>(b));
    any_random_access_iterator<int> c;
    (any_random_access_iterator<int>(c));
}

TEST(correctness, empty_move)
{
    any_forward_iterator<int> a;
    (any_forward_iterator<int>(std::move(a)));
    any_bidirectional_iterator<int> b;
    (any_bidirectional_iterator<int>(std::move(b)));
    any_random_access_iterator<int> c;
    (any_random_access_iterator<int>(std::move(c)));
}

TEST(correctness, assign)
{
    any_forward_iterator<int> a, b;
    a = b;
}

TEST(correctness, postinc_1)
{
    std::list<int> a = {1, 2, 3};

    any_bidirectional_iterator<int> i = a.begin();
    EXPECT_TRUE(*i++ == 1);
    EXPECT_TRUE(*i++ == 2);
    EXPECT_TRUE(*i++ == 3);
    EXPECT_TRUE(i == any_bidirectional_iterator<int>(a.end()));
}

TEST(correctness, list_1)
{
    std::forward_list<int> a = {1, 2, 3, 4, 5};

    std::forward_list<int> b;
    std::copy(any_forward_iterator<int>(a.begin()),
              any_forward_iterator<int>(a.end()),
              std::front_inserter(b));

    std::forward_list<int> c = {5, 4, 3, 2, 1};
    EXPECT_TRUE(b == c);
}

TEST(correctness, list_2)
{
    std::list<int> a = {1, 2, 3, 4, 5};

    std::list<int> b;
    std::copy(any_forward_iterator<int>(a.begin()),
              any_forward_iterator<int>(a.end()),
              std::back_inserter(b));

    EXPECT_TRUE(a == b);
}

TEST(correctness, list_3)
{
    std::list<int> a = {1, 2, 3, 4, 5};

    std::list<int> b;
    using reverse_iterator = std::reverse_iterator<any_bidirectional_iterator<int> >;
    std::copy(reverse_iterator(any_bidirectional_iterator<int>(a.end())),
              reverse_iterator(any_bidirectional_iterator<int>(a.begin())),
              std::front_inserter(b));

    EXPECT_TRUE(a == b);
}

TEST(correctness, vector_1)
{
    std::vector<int> a = {5, 3, 2, 4, 1};
    std::sort(any_random_access_iterator<int>(a.begin()),
              any_random_access_iterator<int>(a.end()));

    std::vector<int> b = {1, 2, 3, 4, 5};
    EXPECT_TRUE(a == b);
}

TEST(correctness, vector_2)
{
    std::vector<int> a = {1, 2, 3, 4, 5};
    any_random_access_iterator<int> i = a.begin();
    EXPECT_TRUE(i[0] == 1);
    EXPECT_TRUE(i[1] == 2);
    EXPECT_TRUE(i[2] == 3);
    EXPECT_TRUE(i[3] == 4);
    EXPECT_TRUE(i[4] == 5);
}

template struct any_iterator<int, std::forward_iterator_tag>;
template struct any_iterator<int, std::bidirectional_iterator_tag>;
template struct any_iterator<int, std::random_access_iterator_tag>;

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
