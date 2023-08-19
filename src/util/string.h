#pragma once

#include <string>
#include <map>

std::string string_replace( const std::string_view s, const std::string_view findS, const std::string_view replaceS );
std::string string_sanitize(std::string_view input);
std::string string_desanitize(std::string_view input);
std::string string_as_llvm_string(std::string_view input);
