#include <Utilities/graycode.h>

using namespace Graycode;
TEST(getHighestBitPosition, GetHighestBitSetPosition) {
	// get highest bit position
	EXPECT_EQ(priv::getHighestBitPosition(0), -1) << "no bits set should yield '-1'";
	EXPECT_EQ(priv::getHighestBitPosition(8), 3) << "int(8) or bit 4 should yield '3'";
}

TEST(ungraycode, ConvertGraycodeToInt) {
	// ungraycode
	unsigned int n;
	n = 0; EXPECT_EQ(ungraycode(n), 0) << "ungraycode(" << n << ") should be 0";
	n = 3; EXPECT_EQ(ungraycode(n), 2) << "ungraycode(" << n << ") should be 2";
	n = 5; EXPECT_EQ(ungraycode(n), 6) << "ungraycode(" << n << ") should be 6";
	n = 7; EXPECT_EQ(ungraycode(n), 5) << "ungraycode(" << n << ") should be 5";
}

TEST(graycode, ConvertIntToGraycode) {
	// graycode
	unsigned int n;
	n = 0; EXPECT_EQ(graycode(n), 0) << "graycode(" << n << ") should be 0";
	n = 3; EXPECT_EQ(graycode(n), 2) << "graycode(" << n << ") should be 2";
	n = 7; EXPECT_EQ(graycode(n), 4) << "graycode(" << n << ") should be 4";
	n = 5; EXPECT_EQ(graycode(n), 7) << "graycode(" << n << ") should be 7";
}

TEST(character, GraycodeOfChar) {
	char a;
	a = 0; EXPECT_EQ(graycode(a), 0) << "graycode(" << a << ") should be 0";
	a = 3; EXPECT_EQ(graycode(a), 2) << "graycode(" << a << ") should be 2";
	a = 7; EXPECT_EQ(graycode(a), 4) << "graycode(" << a << ") should be 4";
}

TEST(shortint, GraycodeOfShortInt) {
	short int b;
	b = 0; EXPECT_EQ(graycode(b), 0) << "graycode(" << b << ") should be 0";
	b = 3; EXPECT_EQ(graycode(b), 2) << "graycode(" << b << ") should be 2";
	b = 7; EXPECT_EQ(graycode(b), 4) << "graycode(" << b << ") should be 4";
}
