#include "../src/zupply.hpp"
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#ifdef _MSC_VER
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

using namespace zz;


TEST_CASE("Size struct", "[size_struct]") {
	CHECK(Size(100, 200) == Size(100, 200));
	CHECK_FALSE(Size(100, 200) == Size(200, 100));
}

TEST_CASE("math::clip", "[math-clip]") {
	CHECK(math::clip(300, 0, 256) == 256);
	CHECK(math::clip(-0.2, 0.0, 256.0) == Approx(0));
}

TEST_CASE("math::round", "[math-round]") {
	CHECK(math::round(0.0) == 0);
	CHECK(math::round(0.499999) == 0);
	CHECK(math::round(0.5) == 0); // banker's rounding!
	CHECK(math::round(1.5) == 2); // banker's rounding!
	CHECK(math::round(-0.5) == 0);
	CHECK(math::round(-0.0) == 0);
	CHECK(math::round(-0.1) == 0);
	CHECK(math::round(-0.4) == 0);
	CHECK(math::round(-0.5) == 0);
	CHECK(math::round(99999999.99999999) == 100000000);
	CHECK(math::round(-99999999.999999999) == -100000000);
	CHECK(math::round(2.3333333333) == 2);
}

TEST_CASE("filesystem", "[filesystem]") {
	const char* testFile = "unittest.txt";
	const char* renameFile = "unittest_rename.md";
	REQUIRE(os::path_exists("."));
	fs::FileEditor fe(testFile, true);
	fe.close();
	CHECK(fs::get_file_size(testFile) == 0);
	fe.open();
	fe << "unit test" << os::endl();
	fe << "\u5355\u5143\u6D4B\u8BD5" << os::endl();
	fe << "\u30E6\u30CB\u30C3\u30C8\u30C6\u30B9\u30C8";
	fe.close();
	CHECK(fs::get_file_size(testFile) > 0);
	REQUIRE(os::path_exists(testFile, true));
	REQUIRE(os::is_file(testFile));
	REQUIRE_FALSE(os::is_directory(testFile));

	SECTION("File reader")
	{
		fs::FileReader fr(testFile);
		CHECK(fr.count_lines() == 3);
		CHECK(fr.next_line() == "unit test");
		CHECK(fr.goto_line(3) == 3);
		CHECK(fr.next_line() == "\u30E6\u30CB\u30C3\u30C8\u30C6\u30B9\u30C8");
	}

	SECTION("Rename the file")
	{
		REQUIRE(os::remove_all(renameFile));
		REQUIRE(os::rename(testFile, renameFile));
		REQUIRE_FALSE(os::path_exists(testFile, true));
		REQUIRE(os::path_exists(renameFile, true));
		REQUIRE(os::remove_file(renameFile));
		REQUIRE_FALSE(os::path_exists(renameFile, true));
	}

	SECTION("PATH")
	{
		CHECK(os::current_working_directory() == os::absolute_path("."));
		CHECK(os::path_split("/first/second/third/last") == std::vector<std::string>({ "first", "second", "third", "last" }));
		std::string testName = "/a/b/c/d/e/f/filename.extension";
		CHECK(os::path_split_filename(testName) == "filename.extension");
		CHECK(os::path_split_basename(testName) == "filename");
		CHECK(os::path_split_extension(testName) == "extension");
		CHECK(os::path_split_directory(testName) == "/a/b/c/d/e/f");
		CHECK(os::path_split_extension("/a/noextension") == "");
		CHECK(os::path_append_basename("regularfile.ext", "_001") == "regularfile_001.ext");
	}

	SECTION("Deep directory operation")
	{
		const char* deepFile = "./tmp/tmp2/tmp3/tmp.txt";
		fe.open(deepFile, true);
		fe.close();
		REQUIRE(os::path_exists("./tmp/tmp2/tmp3", false));
		REQUIRE(os::path_exists(deepFile, true));
		REQUIRE_FALSE(os::path_exists(deepFile, false));
		REQUIRE(os::is_file(deepFile));
		REQUIRE_FALSE(os::is_directory(deepFile));
		REQUIRE(os::is_directory("./tmp"));
		REQUIRE_FALSE(os::is_file("./tmp"));
		REQUIRE(os::remove_dir("./tmp"));
		REQUIRE_FALSE(os::path_exists(deepFile, true));
		REQUIRE_FALSE(os::path_exists("./tmp", true));
	}
}

TEST_CASE("filesystem-path", "fs-path")
{
	fs::Path path(".");
	REQUIRE_FALSE(path.empty());
	REQUIRE(path.exist());
	REQUIRE(path.is_dir());
	CHECK(path.abs_path() == os::current_working_directory());
	CHECK(path.filename() == "");
	
	std::string filename = "path_test.txt";
	fs::FileEditor fe(filename);
	fe.close();
	path = fs::Path(filename);
	CHECK(path.exist());
	CHECK(path.is_file());
	CHECK(path.filename() == filename);
	os::remove_file(filename);
}

TEST_CASE("filesystem-directory", "fs-directory")
{
	fs::FileEditor fe;
	fe.open("dir_test/1.txt");
	fe.open("dir_test/tmp/2.txt");
	fe.open("dir_test/tmp/3.jpg");
	fe.open("dir_test/tmp/tmp2/4.md");
	fe.open("dir_test/tmp2/5.bin");
	fe.open("dir_test/tmp2/tmp3/tmp4/tmp5/6.log");
	fe.close();

	REQUIRE(os::is_directory("dir_test"));
	fs::Directory dir("dir_test", "*.jpg", true);
	REQUIRE(dir.size() == 1);
	CHECK(dir.cbegin()->filename() == "3.jpg");
	
	os::remove_all("dir_test");
}

TEST_CASE("os-console", "os-console")
{
	zz::Size consoleSize = os::console_size();
	CHECK(consoleSize.width > 0);
	CHECK(consoleSize.height > 0);
}

TEST_CASE("Date and Timer", "time")
{
	time::DateTime d1;
	time::DateTime d2;
	CHECK(d1.to_string("%y-%m-%d") == d2.to_string("%y-%m-%d"));
	time::Timer t1;
	time::Timer t2;
	t1.pause();
	auto r1 = t1.elapsed_ns();
	CHECK(Approx(t1.elapsed_sec_double()) == t1.elapsed_ms() / 1000.0);
	CHECK(Approx(t1.elapsed_sec_double()) == t1.elapsed_sec() / 1.0);
	CHECK(Approx(t1.elapsed_sec_double()) == t1.elapsed_us() / 1e6);
	CHECK(Approx(t1.elapsed_sec_double()) == t1.elapsed_ns() / 1e9);
	t1.resume();
	CHECK(t1.elapsed_ns() >= r1);
	auto r2 = t1.elapsed_ns();
	CHECK(r2 >= r1);
	CHECK(t2.elapsed_ns() >= t1.elapsed_ns());
	r1 = t2.elapsed_ms();
	time::sleep(50);
	CHECK(t2.elapsed_ms() >= (50 + r1));
}

TEST_CASE("fmt::int_to_zero_pad_str", "[fmt-int_to_zero_pad_str]") {
	CHECK(fmt::int_to_zero_pad_str(1, 2) == "01");
	CHECK(fmt::int_to_zero_pad_str(2, 3) == "002");
	CHECK(fmt::int_to_zero_pad_str(3, 4) == "0003");
	CHECK(fmt::int_to_zero_pad_str(4, 5) == "00004");
	CHECK(fmt::int_to_zero_pad_str(5, 6) == "000005");
	CHECK(fmt::int_to_zero_pad_str(6, 7) == "0000006");
	CHECK(fmt::int_to_zero_pad_str(7, 8) == "00000007");
	CHECK(fmt::int_to_zero_pad_str(8, 9) == "000000008");
	CHECK(fmt::int_to_zero_pad_str(9, 10) == "0000000009");
	CHECK(fmt::int_to_zero_pad_str(10, 11) == "00000000010");
	CHECK(fmt::int_to_zero_pad_str(999, 20) == "00000000000000000999");
}

TEST_CASE("fmt::is_digit", "[fmt-is_digit]") {
	CHECK(fmt::is_digit('0') == true);
	CHECK(fmt::is_digit('1') == true);
	CHECK(fmt::is_digit('2') == true);
	CHECK(fmt::is_digit('3') == true);
	CHECK(fmt::is_digit('4') == true);
	CHECK(fmt::is_digit('5') == true);
	CHECK(fmt::is_digit('6') == true);
	CHECK(fmt::is_digit('7') == true);
	CHECK(fmt::is_digit('8') == true);
	CHECK(fmt::is_digit('9') == true);
	CHECK(fmt::is_digit('`') == false);
	CHECK(fmt::is_digit(' ') == false);
	CHECK(fmt::is_digit('\n') == false);
	for (char c = 'A'; c <= 'Z'; ++c)
	{
		CHECK(fmt::is_digit(c) == false);
	}
	for (char c = 'a'; c <= 'z'; ++c)
	{
		CHECK(fmt::is_digit(c) == false);
	}
}

TEST_CASE("fmt::wild_card_match", "[fmt-wild_card_match]") {
	CHECK(fmt::wild_card_match("abc.jpg", "*.jpg") == true);
	CHECK(fmt::wild_card_match("abc.jpg", "*") == true);
	CHECK(fmt::wild_card_match("abcccd", "*ccd") == true);
	CHECK(fmt::wild_card_match("mississipissippi", "*issip*ss*") == true);
	CHECK(fmt::wild_card_match("xxxx*zzzzzzzzy*f", "xxxx*zzy*fffff") == false);
	CHECK(fmt::wild_card_match("xxxx*zzzzzzzzy*f", "xxx*zzy*f") == true);
	CHECK(fmt::wild_card_match("xxxxzzzzzzzzyf", "xxxx*zzy*fffff") == false);
	CHECK(fmt::wild_card_match("xxxxzzzzzzzzyf", "xxxx*zzy*f") == true);
	CHECK(fmt::wild_card_match("xyxyxyzyxyz", "xy*z*xyz") == true);
	CHECK(fmt::wild_card_match("mississippi", "*sip*") == true);
	CHECK(fmt::wild_card_match("xyxyxyxyz", "xy*xyz") == true);
	CHECK(fmt::wild_card_match("mississippi", "mi*sip*") == true);
	CHECK(fmt::wild_card_match("ababac", "*abac*") == true);
	CHECK(fmt::wild_card_match("ababac", "*abac*") == true);
	CHECK(fmt::wild_card_match("aaazz", "a*zz*") == true);
	CHECK(fmt::wild_card_match("a12b12", "*12*23") == false);
	CHECK(fmt::wild_card_match("a12b12", "a12b") == false);
	CHECK(fmt::wild_card_match("a12b12", "*12*12*") == true);

	// Additional cases where the '*' char appears in the tame string.
	CHECK(fmt::wild_card_match("*", "*") == true);
	CHECK(fmt::wild_card_match("a*abab", "a*b") == true);
	CHECK(fmt::wild_card_match("a*r", "a*") == true);
	CHECK(fmt::wild_card_match("a*ar", "a*aar") == false);

	// More double wildcard scenarios.
	CHECK(fmt::wild_card_match("XYXYXYZYXYz", "XY*Z*XYz") == true);
	CHECK(fmt::wild_card_match("missisSIPpi", "*SIP*") == true);
	CHECK(fmt::wild_card_match("mississipPI", "*issip*PI") == true);
	CHECK(fmt::wild_card_match("xyxyxyxyz", "xy*xyz") == true);
	CHECK(fmt::wild_card_match("miSsissippi", "mi*sip*") == true);
	CHECK(fmt::wild_card_match("miSsissippi", "mi*Sip*") == false);
	CHECK(fmt::wild_card_match("abAbac", "*Abac*") == true);
	CHECK(fmt::wild_card_match("abAbac", "*Abac*") == true);
	CHECK(fmt::wild_card_match("aAazz", "a*zz*") == true);
	CHECK(fmt::wild_card_match("A12b12", "*12*23") == false);
	CHECK(fmt::wild_card_match("a12B12", "*12*12*") == true);
	CHECK(fmt::wild_card_match("oWn", "*oWn*") == true);

	// Completely tame (no wildcards) cases.
	CHECK(fmt::wild_card_match("bLah", "bLah") == true);
	CHECK(fmt::wild_card_match("bLah", "bLaH") == false);

	// Simple mixed wildcard tests suggested by IBMer Marlin Deckert.
	CHECK(fmt::wild_card_match("a", "*?") == true);
	CHECK(fmt::wild_card_match("ab", "*?") == true);
	CHECK(fmt::wild_card_match("abc", "*?") == true);

	// More mixed wildcard tests including coverage for false positives.
	CHECK(fmt::wild_card_match("a", "??") == false);
	CHECK(fmt::wild_card_match("ab", "?*?") == true);
	CHECK(fmt::wild_card_match("ab", "*?*?*") == true);
	CHECK(fmt::wild_card_match("abc", "?**?*?") == true);
	CHECK(fmt::wild_card_match("abc", "?**?*&?") == false);
	CHECK(fmt::wild_card_match("abcd", "?b*??") == true);
	CHECK(fmt::wild_card_match("abcd", "?a*??") == false);
	CHECK(fmt::wild_card_match("abcd", "?**?c?") == true);
	CHECK(fmt::wild_card_match("abcd", "?**?d?") == false);
	CHECK(fmt::wild_card_match("abcde", "?*b*?*d*?") == true);

	// Single-character-match cases.
	CHECK(fmt::wild_card_match("bLah", "bL?h") == true);
	CHECK(fmt::wild_card_match("bLaaa", "bLa?") == false);
	CHECK(fmt::wild_card_match("bLah", "bLa?") == true);
	CHECK(fmt::wild_card_match("bLaH", "?Lah") == false);
	CHECK(fmt::wild_card_match("bLaH", "?LaH") == true);

	// Many-wildcard scenarios.
	CHECK(fmt::wild_card_match("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
		"a*a*a*a*a*a*aa*aaa*a*a*b") == true);
	CHECK(fmt::wild_card_match("abababababababababababababababababababaacacacacacacacadaeafagahaiajakalaaaaaaaaaaaaaaaaaffafagaagggagaaaaaaaab",
		"*a*b*ba*ca*a*aa*aaa*fa*ga*b*") == true);
	CHECK(fmt::wild_card_match("abababababababababababababababababababaacacacacacacacadaeafagahaiajakalaaaaaaaaaaaaaaaaaffafagaagggagaaaaaaaab",
		"*a*b*ba*ca*a*x*aaa*fa*ga*b*") == false);
	CHECK(fmt::wild_card_match("abababababababababababababababababababaacacacacacacacadaeafagahaiajakalaaaaaaaaaaaaaaaaaffafagaagggagaaaaaaaab",
		"*a*b*ba*ca*aaaa*fa*ga*gggg*b*") == false);
	CHECK(fmt::wild_card_match("abababababababababababababababababababaacacacacacacacadaeafagahaiajakalaaaaaaaaaaaaaaaaaffafagaagggagaaaaaaaab",
		"*a*b*ba*ca*aaaa*fa*ga*ggg*b*") == true);
	CHECK(fmt::wild_card_match("aaabbaabbaab", "*aabbaa*a*") == true);
	CHECK(fmt::wild_card_match("a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*",
		"a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*") == true);
	CHECK(fmt::wild_card_match("aaaaaaaaaaaaaaaaa",
		"*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*") == true);
	CHECK(fmt::wild_card_match("aaaaaaaaaaaaaaaa",
		"*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*") == false);
	CHECK(fmt::wild_card_match("abc*abcd*abcde*abcdef*abcdefg*abcdefgh*abcdefghi*abcdefghij*abcdefghijk*abcdefghijkl*abcdefghijklm*abcdefghijklmn",
		"abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*a            bc*") == false);
	CHECK(fmt::wild_card_match("abc*abcd*abcde*abcdef*abcdefg*abcdefgh*abcdefghi*abcdefghij*abcdefghijk*abcdefghijkl*abcdefghijklm*abcdefghijklmn", 
		"abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*") == true);
	CHECK(fmt::wild_card_match("abc*abcd*abcd*abc*abcd", "abc*abc*abc*abc*abc") == false);
	CHECK(fmt::wild_card_match(
		"abc*abcd*abcd*abc*abcd*abcd*abc*abcd*abc*abc*abcd",
		"abc*abc*abc*abc*abc*abc*abc*abc*abc*abc*abcd") == true);
	CHECK(fmt::wild_card_match("abc", "********a********b********c********") == true);
	CHECK(fmt::wild_card_match("********a********b********c********", "abc") == false);
	CHECK(fmt::wild_card_match("abc", "********a********b********b********") == false);
	CHECK(fmt::wild_card_match("*abc*", "***a*b*c***") == true);
}

TEST_CASE("fmt::starts_with", "[fmt-starts_with]") {
	CHECK(fmt::starts_with("", "") == true);
	CHECK(fmt::starts_with("123456", "1") == true);
	CHECK(fmt::starts_with("123456", "12") == true);
	CHECK(fmt::starts_with("123456", "123") == true);
	CHECK(fmt::starts_with("123456", "1234") == true);
	CHECK(fmt::starts_with("123456", "12345") == true);
	CHECK(fmt::starts_with("123456", "123456") == true);
	CHECK(fmt::starts_with("123456", "1234567") == false);
	CHECK(fmt::starts_with("123456", "01") == false);
	CHECK(fmt::starts_with("123456", " 1") == false);
	CHECK(fmt::starts_with("123456", "12 3456") == false);
}

TEST_CASE("fmt::ends_with", "[fmt-ends_with]") {
	CHECK(fmt::ends_with("", "") == true);
	CHECK(fmt::ends_with("123456", "6") == true);
	CHECK(fmt::ends_with("123456", "56") == true);
	CHECK(fmt::ends_with("123456", "456") == true);
	CHECK(fmt::ends_with("123456", "3456") == true);
	CHECK(fmt::ends_with("123456", "23456") == true);
	CHECK(fmt::ends_with("123456", "123456") == true);
	CHECK(fmt::ends_with("123456", "0123456") == false);
	CHECK(fmt::ends_with("123456", "67") == false);
	CHECK(fmt::ends_with("123456", "56 ") == false);
	CHECK(fmt::ends_with("123456\n", "56") == false);
}

TEST_CASE("fmt::replace_x", "[fmt-replace_x]") {
	std::string str;
	SECTION("replace all {} with quotes") {
		str = "{},{},{}";
		fmt::replace_all(str, "{}", "quotes");
		CHECK(str == "quotes,quotes,quotes");
	}
	SECTION("replace all _ with ,") {
		str = "_ _ _ _ _ _ _";
		fmt::replace_all(str, "_", ",");
		CHECK(str == ", , , , , , ,");
	}
	SECTION("replace all a with A, char version") {
		str = "abcabcaaabbbccc";
		fmt::replace_all(str, 'a', 'A');
		CHECK(str == "AbcAbcAAAbbbccc");
	}
	SECTION("replace all %d with 9") {
		str = "original string: %d, %d, escape this: %%d";
		fmt::replace_all_with_escape(str, "%d", "9");
		CHECK(str == "original string: 9, 9, escape this: %d");
	}
	SECTION("replace first %d with various number") {
		str = "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d";
		for (int i = 0; i < 10; ++i) fmt::replace_first_with_escape(str, "%d", std::to_string(i));
		CHECK(str == "0, 1, 2, 3, 4, 5, 6, 7, 8, 9");
	}
	SECTION("replace sequentially %d with various number") {
		str = "escape this: %%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, leave this %d";
		fmt::replace_sequential_with_escape(str, "%d", { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" });
		CHECK(str == "escape this: %d, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, leave this %d");
	}
}

TEST_CASE("fmt::str_equals", "[fmt-str_equals]") {
	CHECK(fmt::str_equals("", "") == true);
	CHECK(fmt::str_equals("123abc", "123abc") == true);
	CHECK(fmt::str_equals("123abc\0", "123abc") == true);
	CHECK(fmt::str_equals("123", "321") == false);
}

TEST_CASE("fmt::trim", "[fmt-trim]") {
	CHECK(fmt::ltrim("   \t\r\n  [abc]    \t\n\r") == "[abc]    \t\n\r");
	CHECK(fmt::rtrim("   \t\r\n  [abc]    \t\n\r") == "   \t\r\n  [abc]");
	CHECK(fmt::trim("   \t\r\n  [abc]    \t\n\r") == "[abc]");
	CHECK(fmt::trim("{   \t\r\n  [abc]    \t\n\r}") == "{   \t\r\n  [abc]    \t\n\r}");
}

TEST_CASE("fmt::strip", "[fmt-strip]") {
	CHECK(fmt::lstrip("123abc456", "123") == "abc456");
	CHECK(fmt::lstrip(" 123abc456", "123") == " 123abc456"); // strict, no whitespace
	CHECK(fmt::rstrip("123abc456", "456") == "123abc");
	CHECK(fmt::rstrip("123abc456 ", "456") == "123abc456 ");
}

TEST_CASE("fmt::skip", "[fmt-skip]") {
	CHECK(fmt::rskip("/tmp/tmp2/tmp.txt", ".") == "/tmp/tmp2/tmp");
	CHECK(fmt::lskip("   #comment string", "#") == "comment string");
	CHECK(fmt::rskip_all("123 # ; ##   \t", "#") == "123 ");
}

TEST_CASE("fmt::split", "[fmt-split]") {
	CHECK(fmt::split("/tmp/tmp2/tmp.txt", "/") == std::vector<std::string>({"tmp", "tmp2", "tmp.txt"}));
	CHECK(fmt::split("abc,123,def,456,ghi,789", ",") == std::vector<std::string>({ "abc", "123", "def", "456", "ghi", "789" }));
	CHECK(fmt::split("abc,123,def,456,ghi,789", ',') == std::vector<std::string>({ "abc", "123", "def", "456", "ghi", "789" }));
	CHECK(fmt::split("abc,123,def,456,ghi,789", " ") == std::vector<std::string>({ "abc,123,def,456,ghi,789" }));
	CHECK(fmt::split("abc&&123&&def&&456&&ghi&&789", "&&") == std::vector<std::string>({ "abc", "123", "def", "456", "ghi", "789" }));
	CHECK(fmt::split(" ", " ") == std::vector<std::string>());
	CHECK(fmt::split_multi_delims("abc  \t  \n\n123  , def , \t, 456,,,,ghi  ,  789", ", \t\n") == std::vector<std::string>({ "abc", "123", "def", "456", "ghi", "789" }));
	CHECK(fmt::split_whitespace("abc  \t  \n\n123   def  \t 456    ghi  \r\n  789") == std::vector<std::string>({ "abc", "123", "def", "456", "ghi", "789" }));
	CHECK(fmt::split_first_occurance("abc , \t  \n\n123", ',').first == "abc ");
	CHECK(fmt::split_first_occurance("abc , \t  \n\n123", ',').second == " \t  \n\n123");
}

TEST_CASE("fmt::join", "[fmt-join]") {
	CHECK(fmt::join({ "/tmp", "tmp2", "tmp3", "file.ext" }, '/') == "/tmp/tmp2/tmp3/file.ext");
	CHECK(fmt::join({ "tmp", "tmp2", "tmp3", "file.ext" }, '\\') == "tmp\\tmp2\\tmp3\\file.ext");
}

TEST_CASE("fmt::erase_empty", "[fmt-erase_empty]") {
	std::vector<std::string> v({ "/tmp", "", "", "tmp2", "tmp3", "", "file.ext", "" });
	CHECK(fmt::erase_empty(v) == std::vector<std::string>({ "/tmp", "tmp2", "tmp3", "file.ext" }));
	v = { "", "", "", "" };
	CHECK(fmt::erase_empty(v) == std::vector<std::string>());
}

TEST_CASE("fmt::case_convert", "[fmt-case_convert]") {
	CHECK(fmt::to_lower_ascii("") == "");
	CHECK(fmt::to_lower_ascii("ABC") == "abc");
	CHECK(fmt::to_lower_ascii("aBc") == "abc");
	CHECK(fmt::to_lower_ascii("AbC") == "abc");
	CHECK(fmt::to_lower_ascii("abc") == "abc");
	CHECK(fmt::to_lower_ascii("123ABC") == "123abc");
	CHECK(fmt::to_lower_ascii("__mixEDCaSe__StRing") == "__mixedcase__string");
	CHECK(fmt::to_lower_ascii(" whiTeSpace\r\n\tAcceptED") == " whitespace\r\n\taccepted");
	CHECK(fmt::to_upper_ascii("") == "");
	CHECK(fmt::to_upper_ascii("ABC") == "ABC");
	CHECK(fmt::to_upper_ascii("aBc") == "ABC");
	CHECK(fmt::to_upper_ascii("AbC") == "ABC");
	CHECK(fmt::to_upper_ascii("abc") == "ABC");
	CHECK(fmt::to_upper_ascii("123ABC") == "123ABC");
	CHECK(fmt::to_upper_ascii("__mixEDCaSe__StRing") == "__MIXEDCASE__STRING");
	CHECK(fmt::to_upper_ascii(" whiTeSpace\r\n\tAcceptED") == " WHITESPACE\r\n\tACCEPTED");
}

TEST_CASE("cfg:Value", "cfg-value")
{
	cfg::Value v;
	CHECK(v.empty());

	SECTION("String")
	{
		v = "plain string";
		CHECK(v == "plain string");
		CHECK(v.str() == "plain string");
	}

	SECTION("parse integer")
	{
		v = "123456";
		REQUIRE_FALSE(v.empty());
		CHECK(v.load<int>() == 123456);
		v.store("654321");
		REQUIRE_FALSE(v.empty());
		int buf = -1;
		v.load(buf);
		CHECK(buf == 654321);
		v = "-99";
		CHECK(v.load<long>() == -99);
	}

	SECTION("parse float number")
	{
		v = "123456.7890";
		REQUIRE_FALSE(v.empty());
		CHECK(v.load<int>() == 123456);
		CHECK(v.load<float>() == Approx(123456.7890));
		CHECK(v.load<double>() == Approx(123456.7890));
		v.store("654321.987");
		REQUIRE_FALSE(v.empty());
		double buf = -1;
		v.load(buf);
		CHECK(buf == Approx(654321.987));
		v = "-0.00001";
		CHECK(v.load<double>() == Approx(-0.00001));
	}

	SECTION("parse boolean (true, false)")
	{
		v = "true";
		REQUIRE_FALSE(v.empty());
		CHECK(v.load<bool>() == true);
		v.store("tRuE");
		REQUIRE_FALSE(v.empty());
		bool buf = false;
		v.load(buf);
		CHECK(buf == true);
		v = "FalSE";
		CHECK(v.load<bool>() == false);
	}

	SECTION("parse vector")
	{
		v = "1.2 2.333 4.5 6.7 -8.9";
		std::vector<double> values;
		REQUIRE_FALSE(v.empty());
		v.load(values);
		REQUIRE(values.size() == 5);
		CHECK(values[0] == Approx(1.2));
		CHECK(values[1] == Approx(2.333));
		CHECK(values[2] == Approx(4.5));
		CHECK(values[3] == Approx(6.7));
		CHECK(values[4] == Approx(-8.9));
	}
}

TEST_CASE("cfg:cfgParser", "cfg-parser")
{
	std::stringstream ss;
	ss << "a=1\n"
		"b=1\n\n"
		"enabled = TruE \n"
		"[e]\n"
		"ea=1\n"
		"eb=1\n\n"
		"[c]\n"
		"ca=2\n"
		"cb=2\n\n"
		"[a.d]\n"
		"extra.da=3 # this is a comment \n"
		"db=3\n\n"
		"[A]\n"
		" # new line comment \n"
		"Aa=4\n"
		"Ab=4\n"
		"[floats]\n"
		"f1 = 1.1111\n"
		"f2 : 2.33333\n";

	cfg::CfgParser p(ss);
	CHECK(p["a"].str() == "1");
	CHECK(p["a"].load<int>() == 1);
	CHECK(p["b"].load<int>() == 1);
	CHECK(p["enabled"].load<bool>() == true);
	CHECK(p("e")["ea"].load<int>() == 1);
	CHECK(p("e")["eb"].load<int>() == 1);
	CHECK(p("c")["ca"].load<int>() == 2);
	CHECK(p("c")["cb"].load<int>() == 2);
	CHECK(p("a")("d")("extra")["da"].load<int>() == 3);
	CHECK(p("a")("d")["db"].load<int>() == 3);
	CHECK(p("A")["Aa"].load<int>() == 4);
	CHECK(p("A")["Ab"].load<int>() == 4);
	CHECK(p("floats")["f1"].load<int>() == 1);
	CHECK(p("floats")["f1"].load<double>() == Approx(1.1111));
	CHECK(p("floats")["f2"].str() == "2.33333");
	CHECK(p("floats")["f2"].load<double>() == Approx(2.33333));
}

TEST_CASE("arg-argParser", "arg-parser")
{
	cfg::ArgParser p;
	int input;
	p.add_opt_value('i', "input", input, -1, "input name", "INT");
	bool bbb = false;
	p.add_opt_flag('b', "brief", "brief intro", &bbb);
	p.add_opt_help('h', "help");
	p.add_opt_version(-1, "version", "0.0.1");
	std::vector<int> v = { 1, 2, 3, 4 };
	p.add_opt_value('a', "values", v, v, "vector of int to do", "INT").require().set_min(4).set_max(4);
	std::vector<double> f;
	std::vector<double> g = {0.0, 0.0};
	p.add_opt_value(-1, "float", f, g, "vector of double values", "DOUBLE").set_max(2);
	double d;
	p.add_opt_value(-1, "doublevalue", d, -0.5);

	// write a test case for argc and argv
	int argc = 14;
	char* argv[] = { 
		(char*)"unittest", (char*)"-b", (char*)"--brief", 
		(char*)"-i", (char*)"999", (char*)"--values", 
		(char*)"9", (char*)"8", (char*)"7", (char*)"6", 
		(char*)"5", (char*)"--float=1.1", (char*)"'-1.2'", 
		(char*)"--unknown" };

	p.parse(argc, argv, false);
	REQUIRE(p.count_error() == 1);
	SECTION("parse flag")
	{
		CHECK(p.count('b') == 2);
		CHECK(bbb == true);
	}
	
	SECTION("parse integer")
	{
		CHECK(p.count('i') == 1);
		CHECK(input == 999);
	}
	
	SECTION("parse integer vector")
	{
		CHECK(p.count("values") == 1);
		REQUIRE(v.size() == 4);
		CHECK(v[0] == 9);
		CHECK(v[1] == 8);
		CHECK(v[2] == 7);
		CHECK(v[3] == 6);
	}

	SECTION("parse double vector")
	{
		CHECK(p.count("float") == 1);
		REQUIRE(f.size() == 2);
		CHECK(f[0] == Approx(1.1));
		CHECK(f[1] == Approx(-1.2));
	}

	REQUIRE(p.arguments().size() == 1);
	CHECK(p.arguments()[0].load<int>() == 5);
	CHECK(p.count("doublevalue") == 0);
	CHECK(d == Approx(-0.5));
}

TEST_CASE("logger", "logger")
{
	const char* fn1 = "test1.log";
	const char* fn2 = "test2_rotate.log";
	auto logger = log::get_logger("default");
	auto f1 = log::new_simple_file_sink(fn1, true);
	logger->attach_sink(f1);
	REQUIRE(os::remove_all(fn2));
	logger->attach_sink(log::new_rotate_file_sink(fn2, 1024 * 1024 * 10, false));
	logger->detach_console();

	std::vector<std::thread> vt;
	auto functor = []()
	{
		auto threadLogger = log::get_logger("default");
		for (int i = 0; i < 100; ++i) threadLogger->info("Sequence increment {}", i);
		threadLogger->warn("thread finished: ") << os::thread_id();
	};

	for (int i = 0; i < 4; ++i)
	{
		vt.push_back(std::thread(functor));
	}
	for (int i = 0; i < 4; ++i)
	{
		vt[i].join();
	}

	REQUIRE(os::path_exists(fn1, true));
	REQUIRE(os::path_exists(fn2, true));
	CHECK(fs::get_file_size(fn1) == fs::get_file_size(fn2));
}

TEST_CASE("Image", "Image")
{
	Image image;
	CHECK(image.empty());
	image.create(10, 10, 3);
	REQUIRE(image.rows() == 10);
	REQUIRE(image.cols() == 10);
	REQUIRE(image.channels() == 3);
	unsigned char buf[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	image.import(buf, 3, 3, 1);
	CHECK(image.at(1, 1, 0) == 5);
	Image image2 = image;
	image(1, 1, 0) = 15;
	CHECK(image.at(1, 1, 0) == 15);
	CHECK(image2.at(1, 1, 0) == 5);
	image.create(100, 100, 3);
	for (int r = 0; r < 100; ++r)
	{
		for (int c = 0; c < 100; ++c)
		{
			image(r, c, 0) = 255;
			image(r, c, 1) = 0;
			image(r, c, 2) = 0;
		}
	}
	image.save("save_test.bmp");
	Image image3("save_test.bmp");
	CHECK(image3.at(0, 0, 0) == 255);

	Image img4 = image;
	img4.resize(640, 480);
	CHECK(img4.rows() == 480);
	CHECK(img4.cols() == 640);

	ImageHdr imghdr("save_test.bmp");
	CHECK(imghdr.at(0, 0, 0) == Approx(1.0));
	ImageHdr imghdr2(image, 255.f);
	CHECK(imghdr2.at(0, 0, 0) == Approx(255.0));
	imghdr.to_normal().save("save_test2.bmp");

	// test crop function
	Image cropTest(100, 100, 4);
	int tcnt = 0;
	for (int r = 0; r < 100; ++r)
	{
		for (int c = 0; c < 100; ++c)
		{
			for (int k = 0; k < 4; ++k)
			{
				cropTest(r, c, k) = static_cast<unsigned char>(tcnt % 255);
				++tcnt;
			}
		}
	}
	Image cropRef = cropTest;
	cropTest.crop(10, 10, 50, 50);
	int testCnt = 0;
	for (int r = 0; r < cropTest.rows(); ++r)
	{
		for (int c = 0; c < cropTest.cols(); ++c)
		{
			for (int k = 0; k < cropTest.channels(); ++k)
			{
				if (testCnt < 10)
				{
					CHECK((int)cropTest.at(r, c, k) == (int)cropRef.at(r + 10, c + 10, k));
					++testCnt;
				}
			}
		}
	}
}


int main(int argc, char** argv)
{
#ifdef _MSC_VER
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	int result = Catch::Session().run(argc, argv);

	return result;
}

