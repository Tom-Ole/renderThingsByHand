#include <iostream>
#include <cstring>
#include <list>
#include <cstdint>
#include <random>

class Vec3
{
public:
  float x, y, z;

  Vec3(float x, float y) : x(x), y(y), z(0) {};
  Vec3(float x, float y, float z) : x(x), y(y), z(z) {};

  float dot(Vec3 b) const
  {
    return x * b.x + y * b.y + z * b.z;
  };

  Vec3 operator-(const Vec3 &b) const
  {
    return Vec3(x - b.x, y - b.y, z - b.z);
  };
};

class Triangle
{
public:
  Vec3 a, b, c;
  unsigned char *_data;

  Triangle(Vec3 a, Vec3 b, Vec3 c) : a(a), b(b), c(c) {};

  ~Triangle()
  {
    delete[] _data; // Ensure we free the allocated memory
  }

  unsigned char *getData() const
  {
    return _data;
  }

  void rotate(float angle)
  {
    // Simple rotation logic around the origin (0, 0)
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);

    auto rotatePoint = [cosAngle, sinAngle](Vec3 &point)
    {
      float newX = point.x * cosAngle - point.y * sinAngle;
      float newY = point.x * sinAngle + point.y * cosAngle;
      point.x = newX;
      point.y = newY;
    };

    rotatePoint(a);
    rotatePoint(b);
    rotatePoint(c);
  }

  void rotateC(float angle)
  {
    // Rotate around the center of the triangle
    Vec3 center((a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3);

    auto rotatePointAroundCenter = [center, angle](Vec3 &point)
    {
      float cosAngle = cos(angle);
      float sinAngle = sin(angle);
      float translatedX = point.x - center.x;
      float translatedY = point.y - center.y;
      point.x = translatedX * cosAngle - translatedY * sinAngle + center.x;
      point.y = translatedX * sinAngle + translatedY * cosAngle + center.y;
    };

    rotatePointAroundCenter(a);
    rotatePointAroundCenter(b);
    rotatePointAroundCenter(c);
  }

  unsigned char *createTriangleData(Vec3 a, Vec3 b, Vec3 c, int width, int height, unsigned char *data = nullptr)
  {
    if (data == nullptr)
    {
      data = new unsigned char[width * height * 3];
    }

    // Random color for the triangle
    int range = 255 - 1 + 1;
    int num1 = rand() % range + 1;
    int num2 = rand() % range + 1;
    int num3 = rand() % range + 1;

    // Triangle rasterization logic
    for (int j = 0; j < height; ++j)
    {
      for (int i = 0; i < width; ++i)
      {
        Vec3 p(i + 0.5f, j + 0.5f);

        // Barycentric coordinates
        // Reference: https://en.wikipedia.org/wiki/Barycentric_coordinate_system#Triangle
        Vec3 v0 = b - a;
        Vec3 v1 = c - a;
        Vec3 v2 = p - a;

        // Compute barycentric coordinates
        float d00 = v0.dot(v0);
        float d01 = v0.dot(v1);
        float d11 = v1.dot(v1);
        float d20 = v2.dot(v0);
        float d21 = v2.dot(v1);

        float denom = d00 * d11 - d01 * d01; // Normalized area of the triangle

        if (denom == 0.0f)
          continue; // avoid division by zero

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        int index = (j * width + i) * 3;

        if ((u >= 0) && (v >= 0) && (w >= 0))
        {
          data[index + 0] = num1; // Red
          data[index + 1] = num2; // Green
          data[index + 2] = num2; // Blue
        }
      }
    }
    _data = data;
    return data;
  }
};

class Scene
{
public:
  // List of objects (triangles) in the scene
  std::list<Triangle> triangles;

  void addTriangle(const Triangle &triangle)
  {
    triangles.push_back(triangle);
  };

  std::list<Triangle> getTriangles() const
  {
    return triangles;
  };

  void clear()
  {
    triangles.clear();
  };

  unsigned char *getDate(float width, float height)
  {
    unsigned char *data = nullptr;

    for (auto &t : triangles)
    {
      data = t.createTriangleData(t.a, t.b, t.c, width, height, data);
    }

    return data;
  }
};

class Renderer
{
public:
  void init() {

  };

  void update() {

  };

  void render() {

  };
};

class Image
{
public:
  static void createBMP(const char *filename, int width, int height, unsigned char *data)
  {
    constexpr int headerSize = 14;
    constexpr int infoHeaderSize = 40;
    constexpr int pixelDataOffset = headerSize + infoHeaderSize;

    int fileSize = pixelDataOffset + width * height * 3;

    unsigned char header[headerSize + infoHeaderSize];
    std::memset(header, 0, sizeof(header));
    // BMP file header (14 bytes)
    header[0] = 'B';
    header[1] = 'M';
    writeLE<uint32_t>(header, 2, fileSize);         // bfSize
    writeLE<uint32_t>(header, 6, 0);                // bfReserved1 and bfReserved2
    writeLE<uint32_t>(header, 10, pixelDataOffset); // bfOffBits

    // Info header (BITMAPINFOHEADER, 40 bytes)
    writeLE<uint32_t>(header, 14, infoHeaderSize); // biSize
    writeLE<int32_t>(header, 18, width);           // biWidth
    writeLE<int32_t>(header, 22, height);          // biHeight
    writeLE<uint16_t>(header, 26, 1);              // biPlanes
    writeLE<uint16_t>(header, 28, 24);             // biBitCount (24 bits)
    writeLE<uint32_t>(header, 30, 0);              // biCompression (BI_RGB)
    writeLE<uint32_t>(header, 34, 0);              // biSizeImage (0 for BI_RGB)
    writeLE<int32_t>(header, 38, 0);               // biXPelsPerMeter
    writeLE<int32_t>(header, 42, 0);               // biYPelsPerMeter
    writeLE<uint32_t>(header, 46, 0);              // biClrUsed
    writeLE<uint32_t>(header, 50, 0);              // biClrImportant

    // Open file for writing
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
      throw std::runtime_error("Could not open file for writing");
    }

    // Write the header
    fwrite(header, sizeof(unsigned char), sizeof(header), file);

    // Write pixel data
    for (int y = height - 1; y >= 0; --y)
    { // BMP format stores pixels bottom to top
      for (int x = 0; x < width; ++x)
      {
        unsigned char r = data[(y * width + x) * 3 + 0];
        unsigned char g = data[(y * width + x) * 3 + 1];
        unsigned char b = data[(y * width + x) * 3 + 2];
        fwrite(&b, sizeof(unsigned char), 1, file); // Blue
        fwrite(&g, sizeof(unsigned char), 1, file); // Green
        fwrite(&r, sizeof(unsigned char), 1, file); // Red
      }
    }
  };

private:
  template <typename T>
  static void writeLE(unsigned char *buffer, int offset, T value)
  {
    static_assert(std::is_integral_v<T>, "Only integral types supported");
    for (size_t i = 0; i < sizeof(T); ++i)
    {
      buffer[offset + i] = static_cast<unsigned char>((value >> (8 * i)) & 0xFF);
    }
  }
};

Triangle generateRandomTriangle(int width, int height)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> disX(0.1f, static_cast<float>(width));
  std::uniform_real_distribution<float> disY(0.1f, static_cast<float>(height));

  Vec3 a(disX(gen), disY(gen));
  Vec3 b(disX(gen), disY(gen));
  Vec3 c(disX(gen), disY(gen));

  // Randome rotation angle
  Triangle t(a, b, c);
  t.rotateC(static_cast<float>(rand() % 360) * (3.14159f / 180.0f)); // Convert degrees to radians

  return t;
}

int main()
{

  Scene scene;
  int width = 800;
  int height = 600;

  for (int i = 0; i < 10; ++i)
  {
    Triangle t = generateRandomTriangle(width, height);
    scene.addTriangle(t);
  }

  Image::createBMP("triangle.bmp", width, height, scene.getDate(width, height));

  return 0;
}
