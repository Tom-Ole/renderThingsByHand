#include <iostream>
#include <cstring>
#include <list>
#include <cstdint>
#include <random>
#include <fstream>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#define MKDIR(path) _mkdir(path)
#define ACCESS(path, mode) _access(path, mode)
#else
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0755)
#define ACCESS(path, mode) access(path, mode)
#endif

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
  Vec3 a, b, c, color;
  unsigned char *_data;

  Triangle(Vec3 a, Vec3 b, Vec3 c) : a(a), b(b), c(c), color(Vec3(255, 0, 0)) {};
  Triangle(Vec3 a, Vec3 b, Vec3 c, Vec3 color) : a(a), b(b), c(c), color(color) {};

  ~Triangle() {} // Manged by the Scene;

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

  void scale(float scalar)
  {
    // Scale from the center of the triangle
    Vec3 center((a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3, (a.z + b.z + c.z) / 3);
    auto scalePointFromCenter = [center, scalar](Vec3 &point)
    {
      point.x = center.x + (point.x - center.x) * scalar;
      point.y = center.y + (point.y - center.y) * scalar;
      point.z = center.z + (point.z - center.z) * scalar;
    };
    scalePointFromCenter(a);
    scalePointFromCenter(b);
    scalePointFromCenter(c);
  }

  void move(Vec3 offset)
  {
    a.x += offset.x;
    a.y += offset.y;
    a.z += offset.z;
    b.x += offset.x;
    b.y += offset.y;
    b.z += offset.z;
    c.x += offset.x;
    c.y += offset.y;
    c.z += offset.z;
  }

  void renderToBuffer(unsigned char *data, int width, int height)
  {
    // Triangle rasterization logic
    for (int j = 0; j < height; ++j)
    {
      for (int i = 0; i < width; ++i)
      {
        Vec3 p(i + 0.5f, j + 0.5f);

        // Barycentric coordinates
        Vec3 v0 = this->b - this->a;
        Vec3 v1 = this->c - this->a;
        Vec3 v2 = p - a;

        // Compute barycentric coordinates
        float d00 = v0.dot(v0);
        float d01 = v0.dot(v1);
        float d11 = v1.dot(v1);
        float d20 = v2.dot(v0);
        float d21 = v2.dot(v1);

        float denom = d00 * d11 - d01 * d01;

        if (denom == 0.0f)
          continue;

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;

        if ((u >= 0) && (v >= 0) && (w >= 0))
        {
          int index = (j * width + i) * 3;
          data[index + 0] = this->color.x; // Red
          data[index + 1] = this->color.y; // Green
          data[index + 2] = this->color.z; // Blue
        }
      }
    }
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

  unsigned char *getData(int width, int height)
  {
    unsigned char *data = new unsigned char[width * height * 3];

    // Clear to black background
    memset(data, 0, width * height * 3);

    int triIndex = 0;
    for (auto &t : triangles)
    {
      t.renderToBuffer(data, width, height);
      triIndex++;
    }

    return data;
  }
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

    fclose(file);
    //std::cout << "BMP file created: " << filename << std::endl;
  };

  // Simple GIF-like format (actually creates individual BMP frames)
  static void createAnimatedFrames(const char *folderName, Scene &scene, int width, int height, int numFrames)
  {
    // Create the folder - simple approach
    int result = MKDIR(folderName);

    // Check if folder creation was successful or if it already exists
    if (result != 0 && ACCESS(folderName, 0) != 0)
    {
      std::cerr << "Failed to create or access folder: " << folderName << std::endl;
      std::cerr << "Creating frames in current directory instead..." << std::endl;
    }
    else
    {
      std::cout << "Using folder: " << folderName << std::endl;
    }

    std::cout << "Creating " << numFrames << " animation frames..." << std::endl;

    for (int frame = 0; frame < numFrames; ++frame)
    {
      // Rotate triangles for animation
      for (auto &t : scene.triangles)
        t.rotateC(0.05f); // Smaller rotation for smoother animation

      auto data = scene.getData(width, height);

      // Create filename for this frame inside the folder
      char filename[512];
      sprintf(filename, "%s/frame_%03d.bmp", folderName, frame);

      createBMP(filename, width, height, data);

      delete[] data;

      if ((frame + 1) % 10 == 0 || frame == 0)
      {
        std::cout << "Created frame " << frame + 1 << "/" << numFrames << std::endl;
      }
    }

    // Convert frames to GIF using ffmpeg with c++
    
    auto cmd = "ffmpeg -y -loglevel quiet -framerate 25 -i " + std::string(folderName) + "/frame_%03d.bmp " + std::string(folderName) + "/animation.gif";
    system(cmd.c_str());

    // Delete individual frames
    for (int frame = 0; frame < numFrames; ++frame)
    {
      char filename[512];
      sprintf(filename, "%s/frame_%03d.bmp", folderName, frame);
      remove(filename);
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
  std::uniform_real_distribution<float> disX(50.0f, static_cast<float>(width - 50));
  std::uniform_real_distribution<float> disY(50.0f, static_cast<float>(height - 50));

  Vec3 a(disX(gen), disY(gen));
  Vec3 b(disX(gen), disY(gen));
  Vec3 c(disX(gen), disY(gen));

  // ranom color
  Vec3 color(
      static_cast<float>(rand() % 256),
      static_cast<float>(rand() % 256),
      static_cast<float>(rand() % 256)
    );

  Triangle t(a, b, c, color);
  std::uniform_real_distribution<float> angleDis(0.0f, 2.0f * 3.14159f);
  t.rotateC(angleDis(gen));

  return t;
}

int main()
{

  Scene scene;
  int width = 800;
  int height = 600;

  Triangle center = Triangle(Vec3(width / 2 - 200, height / 2 - 200), Vec3(width / 2 + 200, height / 2 - 200), Vec3(width / 2, height / 2 + 100));

  for (int i = 0; i < 10; ++i)
  {
    Triangle t = generateRandomTriangle(width, height);
    scene.addTriangle(t);
  }

  center.rotateC(3.14159f); // Rotate 180 degrees in radians

  scene.addTriangle(center);

  unsigned char *data = scene.getData(width, height);
  Image::createBMP("triangle.bmp", width, height, data);
  delete[] data;

  Image::createAnimatedFrames("triangle_anim", scene, width, height, 250);

  std::cout << "Files created successfully!" << std::endl;
  return 0;
}
