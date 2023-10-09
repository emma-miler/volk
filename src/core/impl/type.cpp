#include "../type.h"

namespace Volk
{

std::shared_ptr<VKType> BUILTIN_INT       = std::make_shared<VKType>("int", 8, "i64");
std::shared_ptr<VKType> BUILTIN_FLOAT     = std::make_shared<VKType>("float", 8, "float");
std::shared_ptr<VKType> BUILTIN_DOUBLE    = std::make_shared<VKType>("double", 8, "double");
std::shared_ptr<VKType> BUILTIN_BOOL      = std::make_shared<VKType>("bool", 8, "i1");
std::shared_ptr<VKType> BUILTIN_STRING    = std::make_shared<VKType>("string", 8);
std::shared_ptr<VKType> BUILTIN_FUNCTION  = std::make_shared<VKType>("function", 8);
std::shared_ptr<VKType> BUILTIN_VOID      = std::make_shared<VKType>("void", 8, "void");
std::shared_ptr<VKType> BUILTIN_VARARGS   = std::make_shared<VKType>("__var_args", 8, "...");

}
