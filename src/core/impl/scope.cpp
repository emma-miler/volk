#include "../scope.h"

namespace Volk
{
void Scope::AddBuiltinTypes()
{
    AddType(std::make_unique<Type>("int", 8, "i64"));
    AddType(std::make_unique<Type>("string", 8));
    AddType(std::make_unique<Type>("raw_ptr", 8));
}
}
