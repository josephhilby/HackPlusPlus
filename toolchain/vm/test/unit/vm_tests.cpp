#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

extern "C" {
#include "vm.h"
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

// EOL sanitize: make CRLF and LF compare equal by dropping '\r'
static std::vector<unsigned char> normalize_eol(std::vector<unsigned char> v) {
    v.erase(std::remove(v.begin(), v.end(), (unsigned char)'\r'), v.end());
    return v;
}

static std::string strip_eol(std::string s) {
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r')) s.pop_back();
    return s;
}

// Find line start/end (byte indices) containing idx
static void line_bounds(const std::vector<unsigned char>& v, size_t idx, size_t& start, size_t& end) {
    if (v.empty()) { start = end = 0; return; }
    if (idx >= v.size()) idx = v.size() - 1;

    start = idx;
    while (start > 0 && v[start - 1] != '\n') --start;

    end = idx;
    while (end < v.size() && v[end] != '\n') ++end;
    if (end < v.size() && v[end] == '\n') ++end;
}

static std::string extract_line(const std::vector<unsigned char>& v, size_t idx) {
    size_t start = 0, end = 0;
    line_bounds(v, idx, start, end);
    return strip_eol(std::string(v.begin() + start, v.begin() + end));
}

static void fail_with_diff(const std::string& stem,
                           const std::vector<unsigned char>& actual,
                           const std::vector<unsigned char>& expected) {
    size_t n = std::min(actual.size(), expected.size());
    size_t i = 0;
    while (i < n && actual[i] == expected[i]) ++i;

    size_t mismatch = (i < n) ? i : n;

    // Find line number and line start
    size_t line0 = 0;
    size_t line_start = 0;
    for (size_t k = 0; k < mismatch && k < actual.size(); ++k) {
        if (actual[k] == '\n') {
            ++line0;
            line_start = k + 1;
        }
    }

    // Find end of line
    size_t line_end = line_start;
    while (line_end < actual.size() && actual[line_end] != '\n') ++line_end;
    if (line_end < actual.size() && actual[line_end] == '\n') ++line_end;

    auto extract = [&](const std::vector<unsigned char>& v) {
        if (v.empty() || line_start >= v.size()) return std::string{};
        size_t end = std::min(line_end, v.size());
        return strip_eol(std::string(v.begin() + line_start, v.begin() + end));
    };

    std::string act_line = extract(actual);
    std::string exp_line = extract(expected);

    FAIL() << "Mismatch: " << stem
           << " (instruction " << (line0 + 1) << ")\n"
           << "actual inst:   \"" << act_line << "\"\n"
           << "expected inst: \"" << exp_line << "\"";
}

static void run_golden_case(const std::string& stem) {
    fs::create_directories(actual_dir());

    fs::path in  = cases_dir()    / (stem + ".vm");
    fs::path out = actual_dir()   / (stem + ".asm");
    fs::path exp = expected_dir() / (stem + ".asm");

    ASSERT_TRUE(fs::exists(in))  << "Missing input: "    << in.string();
    ASSERT_TRUE(fs::exists(exp)) << "Missing expected: " << exp.string();

    int rc = translate_vm(in.c_str(), out.c_str());
    ASSERT_EQ(rc, 0) << "Translate failed: " << in.string();

    auto a = normalize_eol(read_all(out));
    auto b = normalize_eol(read_all(exp));

    if (a != b) {
        fail_with_diff(stem, a, b);
    }
}

TEST(Golden, Add) {
    run_golden_case("Add");
}

TEST(Golden, Stack)  {
    run_golden_case("Stack");
}

TEST(Golden, Basic)   {
    run_golden_case("Basic");
}

TEST(Golden, Pointer)  {
    run_golden_case("Pointer");
}

TEST(Golden, Static) {
    run_golden_case("Static");
}

TEST(Golden, SimpleFunction) {
    run_golden_case("SimpleFunction");
}
