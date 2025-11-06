#include "Vec2.h"
#include <cmath>

Vec2::Vec2() = default;

Vec2::Vec2(float xin, float yin) : x(xin), y(yin) {}

bool Vec2::operator==(const Vec2& rhs) const
{
	return x == rhs.x && y == rhs.y;
}

bool Vec2::operator!=(const Vec2& rhs) const
{
	return x != rhs.x || y != rhs.y;
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
	return { x + rhs.x, y + rhs.y };
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
	return { x - rhs.x, y - rhs.y };
}

Vec2 Vec2::operator*(const float val) const
{
	return { x * val, y * val };
}

Vec2 Vec2::operator/(const float val) const
{
	return { x / val, y / val };
}

Vec2& Vec2::operator+=(const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;

	return *this;
}

Vec2& Vec2::operator-=(const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;

	return *this;
}

Vec2& Vec2::operator/=(const float val)
{
	x /= val;
	y /= val;

	return *this;
}

Vec2& Vec2::operator*=(const float val)
{
	x *= val;
	y *= val;

	return *this;
}

float Vec2::dist(const Vec2& rhs) const
{
	// float dx = rhs.x - x;
	// float dy = rhs.y - y;
	// return std::sqrt(dx * dx + dy * dy);
	return (rhs - *this).length();
}

float Vec2::length() const
{
	return std::sqrt(x * x + y * y);
}

Vec2 Vec2::normalize() const
{
	Vec2 result;

	float len = length();

	if (len > 0.0f)
	{
		result.x = this->x / len;
		result.y = this->y / len;
	}

	return result;
}
