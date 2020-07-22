#include <iostream>
#include "buffer_pool.h"
#include "gtest/gtest.h"
using namespace std;

int add(int a, int b) {
	return a + b;
}
TEST(testCase, test0) {
	EXPECT_EQ(add(2, 3), 5);
}

int main(int argc, char** argv) {
	cout << "Hello CMake." << endl;

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
