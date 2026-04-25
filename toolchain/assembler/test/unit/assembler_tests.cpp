#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

extern "C" {
#include "assembler.h"
}

namespace fs = std::filesystem;

static fs::path cases_dir()    { return "test/golden/cases"; }
static fs::path expected_dir() { return "test/golden/expected"; }
static fs::path actual_dir()   { return "test/golden/actual"; }

static std::vector<unsigned char> read_all(const fs::path& p) {
    std::ifstream in(p, std::ios::binary);
    return std::vector<unsigned char>(std::istreambuf_iterator<char>(in),
                                      std::istreambuf_iterator<char>());
}

static std::string strip_eol(std::string s) {
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();
    return s;
}

static std::string get_line(const std::vector<unsigned char>& v, size_t line0, size_t bytes_per_line) {
    size_t start = line0 * bytes_per_line;
    size_t end   = std::min(start + bytes_per_line, v.size());
    if (start >= v.size()) return "";
    return strip_eol(std::string(v.begin() + start, v.begin() + end));
}

static void fail_with_diff(const std::string& stem,
                           const std::vector<unsigned char>& actual,
                           const std::vector<unsigned char>& expected) {
    size_t n = std::min(actual.size(), expected.size());
    size_t i = 0;
    while (i < n && actual[i] == expected[i]) ++i;

    const size_t BYTES_PER_LINE = 17; // 16 chars + '\n'
    size_t line0 = i / BYTES_PER_LINE;
    size_t col0  = i % BYTES_PER_LINE;

    std::string act_line = get_line(actual,   line0, BYTES_PER_LINE);
    std::string exp_line = get_line(expected, line0, BYTES_PER_LINE);

    FAIL() << "Mismatch: " << stem
           << " (line " << (line0 + 1) << ", bit " << (col0 + 1) << ")\n"
           << "actual word:   \"" << act_line << "\"\n"
           << "expected word: \"" << exp_line << "\"";
}

static void run_golden_case(const std::string& stem) {
    fs::create_directories(actual_dir());

    fs::path in  = cases_dir()    / (stem + ".asm");
    fs::path out = actual_dir()   / (stem + ".hack");
    fs::path exp = expected_dir() / (stem + ".hack");

    ASSERT_TRUE(fs::exists(in))  << "Missing input: "    << in.string();
    ASSERT_TRUE(fs::exists(exp)) << "Missing expected: " << exp.string();

    int rc = assemble_asm(in.c_str(), out.c_str());
    ASSERT_EQ(rc, 0) << "Assemble failed: " << in.string();

    auto a = read_all(out);
    auto b = read_all(exp);

    if (a != b) {
        fail_with_diff(stem, a, b);
    }
}

TEST(Golden, Add)   {
    run_golden_case("Add");
}

TEST(Golden, Max)   {
    run_golden_case("Max");
}

TEST(Golden, MaxL)  {
    run_golden_case("MaxL");
}

TEST(Golden, Rect)  {
    run_golden_case("Rect");
}

TEST(Golden, RectL) {
    run_golden_case("RectL");
}

TEST(Golden, Pong) {
    run_golden_case("Pong");
}
