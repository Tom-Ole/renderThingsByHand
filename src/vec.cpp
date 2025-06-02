#include <iostream>

class Vec3
{
public:
  float x, y, z;

  Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
  Vec3() : x(0), y(0), z(0) {}

  float dot(const Vec3 &other) const
  {
    return x * other.x + y * other.y + z * other.z;
  }

  void print() const
  {
    std::cout << "(" << x << ", " << y << ", " << z << ")" << std::endl;
  }

  Vec3 operator+(const Vec3 &other) const
  {
    return Vec3(x + other.x, y + other.y, z + other.z);
  }
  Vec3 operator-(const Vec3 &other) const
  {
    return Vec3(x - other.x, y - other.y, z - other.z);
  }
  Vec3 operator*(float scalar) const
  {
    return Vec3(x * scalar, y * scalar, z * scalar);
  }

  Vec3 operator/(float scalar) const
  {
    if (scalar == 0)
    {
      throw std::runtime_error("Division by zero");
    }
    return Vec3(x / scalar, y / scalar, z / scalar);
  }

  bool operator==(const Vec3 &other) const
  {
    return (x == other.x && y == other.y && z == other.z);
  }

  bool operator!=(const Vec3 &other) const
  {
    return !(*this == other);
  }

  Vec3 &operator+=(const Vec3 &other)
  {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  Vec3 &operator-=(const Vec3 &other)
  {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  Vec3 &operator*=(float scalar)
  {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
  }
  Vec3 &operator/=(float scalar)
  {
    if (scalar == 0)
    {
      throw std::runtime_error("Division by zero");
    }
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
  }
};

class Vec2
{
public:
  float &x;
  float &y;
  Vec3 v3;

  Vec2(float x_val, float y_val) : v3(x_val, y_val, 0), x(v3.x), y(v3.y) {};
  Vec2() : v3(0, 0, 0), x(v3.x), y(v3.y) {};

  Vec3 &asVec3()
  {
    return v3;
  };

  void print() const
  {
    std::cout << "(" << x << ", " << y << ")" << std::endl;
  }

  float dot(const Vec2 &other) const
  {
    return x * other.x + y * other.y;
  }

  Vec2 operator+(const Vec2 &other) const
  {
    return Vec2(x + other.x, y + other.y);
  }

  Vec2 operator-(const Vec2 &other) const
  {
    return Vec2(x - other.x, y - other.y);
  }

  Vec2 operator*(float scalar) const
  {
    return Vec2(x * scalar, y * scalar);
  }

  Vec2 operator/(float scalar) const
  {
    if (scalar == 0)
    {
      throw std::runtime_error("Division by zero");
    }
    return Vec2(x / scalar, y / scalar);
  }

  bool operator==(const Vec2 &other) const
  {
    return (x == other.x && y == other.y);
  }

  bool operator!=(const Vec2 &other) const
  {
    return !(*this == other);
  }

  Vec2 &operator+=(const Vec2 &other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vec2 &operator-=(const Vec2 &other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  Vec2 &operator*=(float scalar)
  {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  Vec2 &operator/=(float scalar)
  {
    if (scalar == 0)
    {
      throw std::runtime_error("Division by zero");
    }
    x /= scalar;
    y /= scalar;
    return *this;
  }

};