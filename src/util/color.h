#pragma once

#include <string>


//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class Color
{
public:
  static constexpr std::string ANSIResetCode = "\033[39;49m";
  public:
	Color(int r, int g, int b, int a = 255)
	{
		_color[0] = (unsigned char)r;
		_color[1] = (unsigned char)g;
		_color[2] = (unsigned char)b;
		_color[3] = (unsigned char)a;
	}
	void SetColor(int _r, int _g, int _b, int _a = 0)
	{
		_color[0] = (unsigned char)_r;
		_color[1] = (unsigned char)_g;
		_color[2] = (unsigned char)_b;
		_color[3] = (unsigned char)_a;
	}
	void GetColor(int& _r, int& _g, int& _b, int& _a) const
	{
		_r = _color[0];
		_g = _color[1];
		_b = _color[2];
		_a = _color[3];
	}
	int GetValue(int index) const
	{
		return _color[index];
	}
	void SetRawColor(int color32)
	{
		*((int*)this) = color32;
	}
	int GetRawColor(void) const
	{
		return *((int*)this);
	}

	inline int r() const
	{
		return _color[0];
	}
	inline int g() const
	{
		return _color[1];
	}
	inline int b() const
	{
		return _color[2];
	}
	inline int a() const
	{
		return _color[3];
	}

	unsigned char& operator[](int index)
	{
		return _color[index];
	}

	const unsigned char& operator[](int index) const
	{
		return _color[index];
	}

	bool operator==(const Color& rhs) const
	{
		return (*((int*)this) == *((int*)&rhs));
	}

	bool operator!=(const Color& rhs) const
	{
		return !(operator==(rhs));
	}

	Color& operator=(const Color& rhs)
	{
		SetRawColor(rhs.GetRawColor());
		return *this;
	}

	std::string ToANSIColor()
	{
		std::string out = "\033[38;2;";
		out += std::to_string(_color[0]) + ";";
		out += std::to_string(_color[1]) + ";";
		out += std::to_string(_color[2]) + ";";
		out += "49m";
		return out;
	}

  private:
	unsigned char _color[4];
};

namespace Volk::Colors
{
	extern Color Main;
	extern Color Lexer;

	extern Color TRAC;
	extern Color DEBG;
	extern Color INFO;
	extern Color WARN;
	extern Color ERRR;
	extern Color CRIT;
}; // namespace NS::Colors
