#pragma once
#include "../expression_base.h"

namespace Volk
{

class CommentExpression : public Expression
{
public:
    CommentExpression(std::shared_ptr<Volk::Token> token) : Expression(ExpressionType::Comment, token)
	{}
	
public:
    std::string ToString()
    {
        return fmt::format("Comment(value='{}')", Token->Value);
    }

    void ToIR(ExpressionStack& stack)
	{
		stack.Comment(Token->Value);
	}

};

}
