namespace Volk.Core;

public enum ValueExpressionType
{
    Immediate,
    Indirect,
    Unary,
    Binary,
    Call,
    StringConstant,
    ValueCast,
    Dot,
    ArgumentPack,
}