#include "string.h"

static std::map<char, std::string> sanitizePatterns = {
    { '\\' , "\\\\" },
    { '\n', "\\n" },
    { '\r', "\\r" },
    { '\t', "\\t" },
    { '\"', "\\\"" }
};

static std::map<std::string, std::string> desanitizePatterns = {
    { "\\\\" , "\\" },
    { "\\n", "\n" },
    { "\\r", "\r" },
    { "\\t", "\t" },
    { "\\\"", "\"" }
};

static std::map<char, std::string> llvmPatterns = {
    { '\\' , "\\5C" },
    { '\n', "\\0A" },
    { '\r', "\\0C" },
    { '\t', "\\09" },
    { '\"', "\\22" }
};


std::string string_replace( const std::string_view s, const std::string_view findS, const std::string_view replaceS )
{
    std::string result = std::string(s);
    auto pos = s.find( findS );
    if ( pos == std::string::npos ) {
        return result;
    }
    result.replace( pos, findS.length(), replaceS );
    return string_replace( result, findS, replaceS );
}

std::string string_sanitize(std::string_view input)
{

    std::string result = "";
    for (auto&& c : input) {
        auto pos = sanitizePatterns.find(c);
        if ( pos == sanitizePatterns.end()) {
            result += c;
        }
        result += pos->second;
    }
    return result;
}

std::string string_desanitize(std::string_view input)
{

    std::string result = std::string(input);
    for ( const auto & p : desanitizePatterns ) {
        result = string_replace( result, p.first, p.second );
    }
    return result;
}

std::string string_as_llvm_string(std::string_view input)
{

    std::string result = "";
    for (auto&& c : input) {
        auto pos = llvmPatterns.find(c);
        if ( pos == llvmPatterns.end()) {
            result += c;
        }
        result += pos->second;
    }
    return result;
}
