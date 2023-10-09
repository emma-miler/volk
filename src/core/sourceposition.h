#pragma once
#include <memory>
#include "source.h"

namespace Volk
{

typedef struct
{
    int LineIndex;
    int LineOffset;
    int Length;
    std::shared_ptr<ProgramSource> Source;
} SourcePosition;

}
