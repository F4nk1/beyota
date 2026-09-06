/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  test_string.cpp                                                       */
/**************************************************************************/

#include "core/string/node_path.h"
#include "core/string/string_builder.h"
#include "core/string/string_name.h"
#include "core/string/ustring.h"

#include <atomic>
#include <cassert>
#include <cmath>
#include <format>
#include <iostream>
#include <thread>
#include <vector>

using namespace Beyota;

// 1. Basic String operations
void test_string_basics() {
    String empty;
    assert(empty.is_empty());
    assert(empty.length() == 0);
    assert(empty.c_str()[0] == '\0');

    String s1 = "Hello World";
    assert(!s1.is_empty());
    assert(s1.length() == 11);
    assert(s1.size() == 11);
    assert(s1 == "Hello World");
    assert(s1 != "Hello");

    // Copy on write
    String s2 = s1;
    assert(s1 == s2);
    s2[0] = 'h'; // Triggers detach
    assert(s1[0] == 'H');
    assert(s2[0] == 'h');
    assert(s1 != s2);

    // Concatenation
    String s3 = String("Foo") + " " + String("Bar");
    assert(s3 == "Foo Bar");
    s3 += " Baz";
    assert(s3 == "Foo Bar Baz");

    // Move semantics
    String s4 = std::move(s3);
    assert(s4 == "Foo Bar Baz");

    // Comparison spaceship
    String a = "Alpha";
    String b = "Beta";
    assert(a < b);
    assert(b > a);
    assert(a == "Alpha");

    std::cout << "[PASS] String Basics (COW, memory discipline, comparisons)" << std::endl;
}

// 2. Substring, Slicing & Navigation
void test_string_search_and_sub() {
    String s = "aaaTestbbb";

    // Left and Right matching Godot tests
    assert(s.left(6) == "aaaTes");
    assert(s.left(-6) == "aaaT");
    assert(s.left(0) == "");
    assert(s.left(20) == "aaaTestbbb");

    assert(s.right(6) == "estbbb");
    assert(s.right(-6) == "tbbb");
    assert(s.right(0) == "");
    assert(s.right(20) == "aaaTestbbb");

    // Substr
    assert(s.substr(3, 4) == "Test");
    assert(s.substr(7) == "bbb");
    assert(s.substr(100) == "");

    // Find & Rfind
    assert(s.find("Test") == 3);
    assert(s.find("NotFound") == -1);
    assert(s.rfind("a") == 2);
    assert(s.contains("Test"));
    assert(!s.contains("Other"));

    // Case-insensitive find
    assert(s.findn("TEST") == 3);
    assert(s.rfindn("A") == 2);

    // Begins with & ends with
    assert(s.begins_with("aaa"));
    assert(!s.begins_with("bbb"));
    assert(s.ends_with("bbb"));
    assert(!s.ends_with("aaa"));

    std::cout << "[PASS] String Search & Navigation (left, right, find, findn, contains)" << std::endl;
}

// 3. Case Transformations & Godot Canonical Capitalize Tests
void test_string_casing_and_capitalization() {
    // to_lower & to_upper
    String mixed = "Godot Beyota Engine 2026";
    assert(mixed.to_lower() == "godot beyota engine 2026");
    assert(mixed.to_upper() == "GODOT BEYOTA ENGINE 2026");

    // Canonical Godot test cases for capitalize()
    assert(String("2D").capitalize() == "2d");
    assert(String("2d").capitalize() == "2d");
    assert(String("2db").capitalize() == "2 Db");
    assert(String("HTML5 Html5 html5 html_5").capitalize() == "Html 5 Html 5 Html 5 Html 5");
    assert(String("Node2D Node2d NODE2D NODE_2D node_2d").capitalize() == "Node 2d Node 2d Node 2d Node 2d Node 2d");
    assert(String("Node2DPosition").capitalize() == "Node 2d Position");
    assert(String("Number2Digits").capitalize() == "Number 2 Digits");
    assert(String("bytes2var").capitalize() == "Bytes 2 Var");
    assert(String("linear2db").capitalize() == "Linear 2 Db");
    assert(String("vector3").capitalize() == "Vector 3");
    assert(String("sha256").capitalize() == "Sha 256");
    assert(String("PascalCase").capitalize() == "Pascal Case");
    assert(String("PascalPascalCase").capitalize() == "Pascal Pascal Case");
    assert(String("snake_case").capitalize() == "Snake Case");
    assert(String("snake_snake_case").capitalize() == "Snake Snake Case");
    assert(String("kebab-case").capitalize() == "Kebab Case");
    assert(String("kebab-kebab-case").capitalize() == "Kebab Kebab Case");
    assert(String("sha256sum").capitalize() == "Sha 256 Sum");
    assert(String("cat2dog").capitalize() == "Cat 2 Dog");

    // Canonical Godot test cases for case conversions
    struct CaseTest {
        const char *input;
        const char *camel;
        const char *pascal;
        const char *snake;
        const char *kebab;
    };

    CaseTest cases[] = {
        {"2D", "2d", "2d", "2d", "2d"},
        {"2d", "2d", "2d", "2d", "2d"},
        {"2db", "2Db", "2Db", "2_db", "2-db"},
        {"Vector3", "vector3", "Vector3", "vector_3", "vector-3"},
        {"sha256", "sha256", "Sha256", "sha_256", "sha-256"},
        {"Node2D", "node2d", "Node2d", "node_2d", "node-2d"},
        {"RichTextLabel", "richTextLabel", "RichTextLabel", "rich_text_label", "rich-text-label"},
        {"HTML5", "html5", "Html5", "html_5", "html-5"},
        {"Node2DPosition", "node2dPosition", "Node2dPosition", "node_2d_position", "node-2d-position"},
        {"Number2Digits", "number2Digits", "Number2Digits", "number_2_digits", "number-2-digits"},
        {"bytes2var", "bytes2Var", "Bytes2Var", "bytes_2_var", "bytes-2-var"},
        {"linear2db", "linear2Db", "Linear2Db", "linear_2_db", "linear-2-db"},
        {"sha256sum", "sha256Sum", "Sha256Sum", "sha_256_sum", "sha-256-sum"},
        {"camelCase", "camelCase", "CamelCase", "camel_case", "camel-case"},
        {"PascalCase", "pascalCase", "PascalCase", "pascal_case", "pascal-case"},
        {"snake_case", "snakeCase", "SnakeCase", "snake_case", "snake-case"},
        {"kebab-case", "kebabCase", "KebabCase", "kebab_case", "kebab-case"},
        {"Test TEST test", "testTestTest", "TestTestTest", "test_test_test", "test-test-test"},
    };

    for (const auto &tc : cases) {
        String in(tc.input);
        assert(in.to_camel_case() == tc.camel);
        assert(in.to_pascal_case() == tc.pascal);
        assert(in.to_snake_case() == tc.snake);
        assert(in.to_kebab_case() == tc.kebab);
        assert(in.camelcase_to_underscore() == tc.snake);
    }

    std::cout << "[PASS] String Casing & Canonical Godot Capitalize Suite" << std::endl;
}

// 4. Trimming, Padding & Stripping
void test_string_trim_and_pad() {
    // Strip edges
    String s = "\t Test Test   ";
    assert(s.strip_edges(true, false) == "Test Test   ");
    assert(s.strip_edges(false, true) == "\t Test Test");
    assert(s.strip_edges(true, true) == "Test Test");

    // Strip escapes
    String esc = "\t\tTest Test\r\n Test";
    assert(esc.strip_escapes() == "Test Test Test");

    // Trim prefix & suffix
    String t = "aaaTestbbb";
    assert(t.trim_prefix("aaa") == "Testbbb");
    assert(t.trim_prefix("Test") == t);
    assert(t.trim_prefix("") == t);
    assert(t.trim_prefix("aaaTestbbb") == "");

    assert(t.trim_suffix("bbb") == "aaaTest");
    assert(t.trim_suffix("Test") == t);
    assert(t.trim_suffix("") == t);
    assert(t.trim_suffix("aaaTestbbb") == "");

    // Lpad & Rpad
    String pad = "42";
    assert(pad.lpad(5, '0') == "00042");
    assert(pad.rpad(5, ' ') == "42   ");
    assert(pad.lpad(2, '0') == "42");

    // Pad zeros & pad decimals
    assert(String("123").pad_zeros(5) == "00123");
    assert(String("-123").pad_zeros(5) == "-00123");
    assert(String("3.14").pad_decimals(4) == "3.1400");
    assert(String("3.141592").pad_decimals(2) == "3.14");
    assert(String("3").pad_decimals(2) == "3.00");

    std::cout << "[PASS] String Trimming, Padding & Stripping" << std::endl;
}

// 5. Split, Rsplit, Join & Replace
void test_string_split_and_join() {
    String csv = "apple,banana,orange,grape";
    auto parts = csv.split(",");
    assert(parts.size() == 4);
    assert(parts[0] == "apple");
    assert(parts[1] == "banana");
    assert(parts[2] == "orange");
    assert(parts[3] == "grape");

    // Join
    String joined = String(" | ").join(parts);
    assert(joined == "apple | banana | orange | grape");

    // Split with maxsplit
    auto limited = csv.split(",", true, 2);
    assert(limited.size() == 3);
    assert(limited[0] == "apple");
    assert(limited[1] == "banana");
    assert(limited[2] == "orange,grape");

    // Rsplit
    String path = "dir/sub/file.txt";
    auto rparts = path.rsplit("/", true, 1);
    assert(rparts.size() == 2);
    assert(rparts[0] == "dir/sub");
    assert(rparts[1] == "file.txt");

    // Split with empty delimiter (every character)
    String abc = "abc";
    auto chars = abc.split("");
    assert(chars.size() == 3);
    assert(chars[0] == "a");
    assert(chars[1] == "b");
    assert(chars[2] == "c");

    // Replace & Replacen
    String rep = "Hello World World";
    assert(rep.replace("World", "Beyota") == "Hello Beyota Beyota");
    assert(rep.replacen("world", "Beyota") == "Hello Beyota Beyota");

    std::cout << "[PASS] String Split, Rsplit, Join & Replacement" << std::endl;
}

// 6. Path Operations matching Godot
void test_string_paths() {
    String p1 = "res://scenes/main/player.tscn";
    assert(p1.get_base_dir() == "res://scenes/main");
    assert(p1.get_file() == "player.tscn");
    assert(p1.get_extension() == "tscn");
    assert(p1.get_basename() == "res://scenes/main/player");

    String p2 = "/usr/local/bin/godot";
    assert(p2.get_base_dir() == "/usr/local/bin");
    assert(p2.get_file() == "godot");
    assert(p2.get_extension() == "");

    String win_path = "C:\\Games\\Godot\\game.exe";
    assert(win_path.get_extension() == "exe");
    assert(win_path.get_file() == "game.exe");

    // Path join
    assert(String("path").path_join("to/file") == "path/to/file");
    assert(String("path/").path_join("to/file") == "path/to/file");
    assert(String("path").path_join("/to/file") == "path/to/file");

    // Simplify path
    assert(String("a/b/../c").simplify_path() == "a/c");
    assert(String("a/./b/./c").simplify_path() == "a/b/c");
    assert(String("/a/b/../../c").simplify_path() == "/c");
    assert(String("res://a/b/../c").simplify_path() == "res://a/c");

    // Absolute / Relative
    assert(String("/root").is_absolute_path());
    assert(String("C:/root").is_absolute_path());
    assert(String("res://scene").is_absolute_path());
    assert(!String("node/child").is_absolute_path());
    assert(String("node/child").is_relative_path());

    std::cout << "[PASS] String Path Operations (get_base_dir, get_file, simplify_path)" << std::endl;
}

// 7. Conversions, Escapes & Formatting
void test_string_conversions_and_escaping() {
    // Numeric conversions
    assert(String("123456").to_int() == 123456);
    assert(String("  -987").to_int() == -987);
    assert(String("3.14159").to_float() > 3.14158 && String("3.14159").to_float() < 3.14160);
    assert(String("0xFF").hex_to_int() == 255);
    assert(String("#A0").hex_to_int() == 160);
    assert(String("0b1011").bin_to_int() == 11);

    // Validation
    assert(String("12345").is_valid_int());
    assert(!String("12a45").is_valid_int());
    assert(String("3.14159").is_valid_float());
    assert(String("0x1A2B").is_valid_hex_number());
    assert(String("my_variable_1").is_valid_identifier());
    assert(!String("1_variable").is_valid_identifier());
    assert(String("file_name.txt").is_valid_filename());
    assert(!String("file/name.txt").is_valid_filename());

    // C escape & unescape
    String raw = "Hello\t\"World\"\nTest\\Path";
    String escaped = raw.c_escape();
    assert(escaped == "Hello\\t\\\"World\\\"\\nTest\\\\Path");
    assert(escaped.c_unescape() == raw);

    // JSON and XML escape
    assert(String("{\"key\": \"val\"}").json_escape() == "{\\\"key\\\": \\\"val\\\"}");
    assert(String("<tag id='1'>&</tag>").xml_escape(true) == "&lt;tag id=&apos;1&apos;&gt;&amp;&lt;/tag&gt;");

    // Static constructors
    assert(String::num_int64(42) == "42");
    assert(String::num_uint64(100ULL) == "100");
    assert(String::chr('A') == "A");

    // std::format
    std::string formatted = std::format("Result: {}", String("Beyota Engine"));
    assert(formatted == "Result: Beyota Engine");

    std::cout << "[PASS] String Conversions, Escaping & std::format Integration" << std::endl;
}

// 8. StringName Interning & Thread Safety
void test_string_name() {
    StringName s1 = "player_pos";
    StringName s2 = "player_pos";
    StringName s3 = "enemy_pos";

    // O(1) comparison via pointer equality
    assert(s1 == s2);
    assert(s1 != s3);
    assert(s1.hash() == s2.hash());
    assert(!s1.is_empty());
    assert(s1.c_str() != nullptr);

    StringName empty;
    assert(empty.is_empty());
    assert(empty.hash() == 0);

    // Multithreaded interning stress test
    constexpr int THREAD_COUNT = 8;
    constexpr int SYMBOLS_PER_THREAD = 200;
    std::vector<std::thread> threads;
    std::atomic<bool> start_flag{false};

    for (int t = 0; t < THREAD_COUNT; ++t) {
        threads.emplace_back([&start_flag]() {
            while (!start_flag.load(std::memory_order_relaxed)) {
                std::this_thread::yield();
            }
            std::vector<StringName> local_names;
            for (int i = 0; i < SYMBOLS_PER_THREAD; ++i) {
                std::string s = "Symbol_" + std::to_string(i % 20); // 20 shared symbols across all threads
                local_names.emplace_back(s);
            }
            for (size_t i = 0; i < local_names.size(); ++i) {
                StringName expected("Symbol_" + std::to_string(i % 20));
                assert(local_names[i] == expected);
            }
        });
    }

    start_flag.store(true, std::memory_order_release);
    for (auto &th : threads) {
        th.join();
    }

    std::cout << "[PASS] StringName (O(1) comparison, symbol interning & concurrency)" << std::endl;
}

// 9. NodePath Canonical Godot Parity Test (Matching Godot test_node_path.cpp)
void test_node_path() {
    // Relative path test
    const NodePath node_path_relative = NodePath("Path2D/PathFollow2D/Sprite2D:position:x");
    assert(node_path_relative.get_as_property_path() == NodePath(":Path2D/PathFollow2D/Sprite2D:position:x"));
    assert(node_path_relative.get_concatenated_subnames() == "position:x");
    assert(node_path_relative.get_name(0) == "Path2D");
    assert(node_path_relative.get_name(1) == "PathFollow2D");
    assert(node_path_relative.get_name(2) == "Sprite2D");
    assert(node_path_relative.get_name(3) == "");
    assert(node_path_relative.get_name(-1) == "");
    assert(node_path_relative.get_name_count() == 3);
    assert(node_path_relative.get_subname(0) == "position");
    assert(node_path_relative.get_subname(1) == "x");
    assert(node_path_relative.get_subname(2) == "");
    assert(node_path_relative.get_subname(-1) == "");
    assert(node_path_relative.get_subname_count() == 2);
    assert(!node_path_relative.is_absolute());
    assert(!node_path_relative.is_empty());

    // Absolute path test
    const NodePath node_path_absolute = NodePath("/root/Sprite2D");
    assert(node_path_absolute.get_as_property_path() == NodePath(":root/Sprite2D"));
    assert(node_path_absolute.get_concatenated_subnames() == "");
    assert(node_path_absolute.get_name(0) == "root");
    assert(node_path_absolute.get_name(1) == "Sprite2D");
    assert(node_path_absolute.get_name(2) == "");
    assert(node_path_absolute.get_name(-1) == "");
    assert(node_path_absolute.get_name_count() == 2);
    assert(node_path_absolute.get_subname_count() == 0);
    assert(node_path_absolute.is_absolute());
    assert(!node_path_absolute.is_empty());

    // Empty path test
    const NodePath node_path_empty = NodePath();
    assert(node_path_empty.get_as_property_path() == NodePath());
    assert(node_path_empty.get_concatenated_subnames() == "");
    assert(node_path_empty.get_name_count() == 0);
    assert(node_path_empty.get_subname_count() == 0);
    assert(!node_path_empty.is_absolute());
    assert(node_path_empty.is_empty());

    // Slice test matching Godot test_node_path.cpp exactly
    const NodePath rel_slice = NodePath("Parent/Child:prop:subprop");
    const NodePath abs_slice = NodePath("/root/Parent/Child:prop");

    assert(rel_slice.slice(0, 2) == NodePath("Parent/Child"));
    assert(rel_slice.slice(3) == NodePath(":subprop"));
    assert(rel_slice.slice(1) == NodePath("Child:prop:subprop"));
    assert(rel_slice.slice(1, 3) == NodePath("Child:prop"));
    assert(rel_slice.slice(-1) == NodePath(":subprop"));
    assert(rel_slice.slice(0, -1) == NodePath("Parent/Child:prop"));
    assert(rel_slice.slice(-2, -1) == NodePath(":prop"));
    assert(rel_slice.slice(0, 10) == NodePath("Parent/Child:prop:subprop"));
    assert(rel_slice.slice(-10, 2) == NodePath("Parent/Child"));
    assert(rel_slice.slice(1, 1) == NodePath(""));

    assert(abs_slice.slice(0, 2) == NodePath("/root/Parent"));
    assert(abs_slice.slice(1, 4) == NodePath("Parent/Child:prop"));
    assert(abs_slice.slice(3, 4) == NodePath(":prop"));

    assert(NodePath("").slice(0, 1) == NodePath(""));
    assert(NodePath("").slice(-1, 2) == NodePath(""));
    assert(NodePath("/").slice(-1, 2) == NodePath("/"));

    std::cout << "[PASS] NodePath (Canonical Godot parity, property paths, slices)" << std::endl;
}

// 10. StringBuilder Test Suite
void test_string_builder() {
    StringBuilder sb;
    assert(sb.get_string_length() == 0);
    assert(sb.num_strings_appended() == 0);

    sb.append("Hello");
    sb.append(" ");
    sb.append(String("World"));
    sb += " from ";
    sb += String("Beyota");
    sb += "!";

    assert(sb.get_string_length() == 24);
    assert(sb.num_strings_appended() == 6);

    String result = sb.as_string();
    assert(result == "Hello World from Beyota!");
    assert(result.length() == 24);

    // Implicit cast to String
    String result_cast = sb;
    assert(result_cast == "Hello World from Beyota!");

    std::cout << "[PASS] StringBuilder (Buffered single-allocation accumulator)" << std::endl;
}

int main() {
    std::cout << "Running exhaustive Beyota core/string subsystem test suite..." << std::endl;
    std::cout << "=============================================================" << std::endl;

    test_string_basics();
    test_string_search_and_sub();
    test_string_casing_and_capitalization();
    test_string_trim_and_pad();
    test_string_split_and_join();
    test_string_paths();
    test_string_conversions_and_escaping();
    test_string_name();
    test_node_path();
    test_string_builder();

    std::cout << "=============================================================" << std::endl;
    std::cout << "ALL 10 STRING SUBSYSTEM TEST SUITES PASSED SUCCESSFULLY!" << std::endl;
    std::cout << "=============================================================" << std::endl;
    return 0;
}
