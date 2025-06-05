#include <iostream>
#include <cstring>
#include <list>
#include <cstdint>
#include <random>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>

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

  Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {};

  float dot(const Vec3 &b) const
  {
    return x * b.x + y * b.y + z * b.z;
  };

  Vec3 operator-(const Vec3 &b) const
  {
    return Vec3(x - b.x, y - b.y, z - b.z);
  };

  Vec3 operator+(const Vec3 &b) const
  {
    return Vec3(x + b.x, y + b.y, z + b.z);
  };

  Vec3 operator*(float scalar) const
  {
    return Vec3(x * scalar, y * scalar, z * scalar);
  };

  Vec3 cross(const Vec3 &b) const
  {
    return Vec3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);
  };

  float length() const
  {
    return sqrt(x * x + y * y + z * z);
  };

  Vec3 normalize() const
  {
    float len = length();
    if (len == 0)
      return Vec3(0, 0, 0);
    return Vec3(x / len, y / len, z / len);
  };
};

// 3D Camera class for perspective projection
class Camera
{
public:
    Vec3 position;
    Vec3 target;
    Vec3 up;
    float fov;    // Field of view in radians
    float near_z; // Near clipping plane
    float far_z;  // Far clipping plane

    Camera(Vec3 pos, Vec3 tgt, Vec3 u, float field_of_view = 3.14159f / 3.0f)
        : position(pos), target(tgt), up(u), fov(field_of_view), near_z(0.1f), far_z(1000.0f) {}

    Vec3 project(const Vec3 &point, int screenWidth, int screenHeight) const
    {
        // Camera basis vectors
        Vec3 forward = (target - position).normalize();
        Vec3 right = forward.cross(up).normalize();
        Vec3 camera_up = right.cross(forward);

        Vec3 translated = point - position;
        float x = translated.dot(right);
        float y = translated.dot(camera_up);
        float z = translated.dot(forward);

        // Reject points behind camera
        if (z <= near_z) z = near_z + 0.001f;

        float aspect = float(screenWidth) / screenHeight;
        float scale = 1.0f / tanf(fov * 0.5f);

        float proj_x = (x * scale) / (z * aspect);
        float proj_y = (y * scale) / z;

        // Convert to screen pixels (origin top-left)
        float screen_x = (proj_x + 1.0f) * 0.5f * screenWidth;
        float screen_y = (1.0f - proj_y) * 0.5f * screenHeight;

        return Vec3(screen_x, screen_y, z);
    }
};

class Triangle
{
public:
  Vec3 a, b, c, color;
  Vec3 normal;

  Triangle(Vec3 a, Vec3 b, Vec3 c) : a(a), b(b), c(c), color(Vec3(255, 0, 0))
  {
    calculateNormal();
  };

  Triangle(Vec3 a, Vec3 b, Vec3 c, Vec3 color) : a(a), b(b), c(c), color(color)
  {
    calculateNormal();
  };

  void calculateNormal()
  {
    Vec3 v1 = b - a;
    Vec3 v2 = c - a;
    normal = v1.cross(v2).normalize();
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
    calculateNormal();
  }

  void rotateC(float angle)
  {
    // Rotate around the center of the triangle
    Vec3 center((a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3, (a.z + b.z + c.z) / 3);

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
    calculateNormal();
  }

  void rotateY(float angle)
  {
    // Rotate around Y axis
    Vec3 center((a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3, (a.z + b.z + c.z) / 3);

    auto rotatePointY = [center, angle](Vec3 &point)
    {
      float cosAngle = cos(angle);
      float sinAngle = sin(angle);
      float translatedX = point.x - center.x;
      float translatedZ = point.z - center.z;
      point.x = translatedX * cosAngle - translatedZ * sinAngle + center.x;
      point.z = translatedX * sinAngle + translatedZ * cosAngle + center.z;
    };

    rotatePointY(a);
    rotatePointY(b);
    rotatePointY(c);
    calculateNormal();
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
    a = a + offset;
    b = b + offset;
    c = c + offset;
  }

  // Check if triangle is facing towards camera (for backface culling)
  bool isFacingCamera(const Vec3 &cameraPos) const
  {
    Vec3 center((a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3, (a.z + b.z + c.z) / 3);
    Vec3 toCamera = (cameraPos - center).normalize();
    return normal.dot(toCamera) > 0;
  }

  void renderToBuffer(unsigned char *data, float *zbuffer, int width, int height, const Camera &camera)
  {
    // Project 3D vertices to 2D screen space
    Vec3 p1 = camera.project(a, width, height);
    Vec3 p2 = camera.project(b, width, height);
    Vec3 p3 = camera.project(c, width, height);

    // Skip triangles behind the camera
    if (p1.z <= camera.near_z || p2.z <= camera.near_z || p3.z <= camera.near_z)
      return;

    // Backface culling
    if (!isFacingCamera(camera.position))
      return;

    // Find bounding box
    int minX = std::max(0, (int)std::min({p1.x, p2.x, p3.x}));
    int maxX = std::min(width - 1, (int)std::max({p1.x, p2.x, p3.x}));
    int minY = std::max(0, (int)std::min({p1.y, p2.y, p3.y}));
    int maxY = std::min(height - 1, (int)std::max({p1.y, p2.y, p3.y}));

    // Triangle rasterization with depth testing
    for (int j = minY; j <= maxY; ++j)
    {
      for (int i = minX; i <= maxX; ++i)
      {
        Vec3 p(i + 0.5f, j + 0.5f, 0);

        // Barycentric coordinates
        Vec3 v0 = Vec3(p2.x - p1.x, p2.y - p1.y, 0);
        Vec3 v1 = Vec3(p3.x - p1.x, p3.y - p1.y, 0);
        Vec3 v2 = Vec3(p.x - p1.x, p.y - p1.y, 0);

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

        if (u >= 0 && v >= 0 && w >= 0)
        {
          // Interpolate depth
          float depth = u * p1.z + v * p2.z + w * p3.z;

          int index = j * width + i;

          // Depth test
          if (depth < zbuffer[index])
          {
            zbuffer[index] = depth;

            // Apply simple lighting based on normal
            Vec3 lightDir = Vec3(0, 0, -1).normalize();             // Light coming from camera
            float lighting = std::max(0.3f, -normal.dot(lightDir)); // Ambient + diffuse

            int pixelIndex = index * 3;
            data[pixelIndex + 0] = (unsigned char)(color.x * lighting); // Red
            data[pixelIndex + 1] = (unsigned char)(color.y * lighting); // Green
            data[pixelIndex + 2] = (unsigned char)(color.z * lighting); // Blue
          }
        }
      }
    }
  }
};

class Scene
{
public:
  std::list<Triangle> triangles;
  Camera camera;

  Scene() : camera(Vec3(0, 0, 5), Vec3(0, 0, 0), Vec3(0, 1, 0)) {}

  void addTriangle(const Triangle &triangle)
  {
    triangles.push_back(triangle);
  }

  // Return const reference to avoid copying the list
  const std::list<Triangle> &getTriangles() const
  {
    return triangles;
  }

  void clear()
  {
    triangles.clear();
  }

  void setCamera(const Camera &cam)
  {
    camera = cam;
  }

  // Render scene to raw RGB buffer
  unsigned char *getData(int width, int height)
  {
    unsigned char *data = new unsigned char[width * height * 3];
    float *zbuffer = new float[width * height];

    // Clear framebuffer to black and zbuffer to max depth
    memset(data, 0, width * height * 3);
    std::fill(zbuffer, zbuffer + width * height, std::numeric_limits<float>::max());

    for (auto &t : triangles)
    {
      t.renderToBuffer(data, zbuffer, width, height, camera);
    }

    delete[] zbuffer;
    return data; // caller must delete[] this memory
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
  };

  static void createAnimatedFrames(const char *folderName, Scene &scene, int width, int height, int numFrames)
  {
    int result = MKDIR(folderName);

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
        t.rotateY(0.05f); // Rotate around Y axis for 3D effect

      auto data = scene.getData(width, height);

      char filename[512];
      sprintf(filename, "%s/frame_%03d.bmp", folderName, frame);

      createBMP(filename, width, height, data);
      delete[] data;

      if ((frame + 1) % 10 == 0 || frame == 0)
      {
        std::cout << "Created frame " << frame + 1 << "/" << numFrames << std::endl;
      }
    }

    // Convert frames to GIF using ffmpeg
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

class Plane
{
  Vec3 pos;
  Vec3 color;
  float width, height;

public:
  Plane(Vec3 position, Vec3 color, float w, float h) : pos(position), color(color), width(w), height(h) {}

  std::vector<Triangle> toTriangles() const
  {
    std::vector<Triangle> triangles;
    Vec3 a = pos + Vec3(-width / 2, -height / 2, 0);
    Vec3 b = pos + Vec3(width / 2, -height / 2, 0);
    Vec3 c = pos + Vec3(-width / 2, height / 2, 0);
    Vec3 d = pos + Vec3(width / 2, height / 2, 0);

    triangles.emplace_back(a, b, c, color);
    triangles.emplace_back(b, d, c, color);
    return triangles;
  }
};

int main()
{
  Scene scene;
  int width = 1000;
  int height = 1000;

  // Set up camera position
  Camera camera = Camera(Vec3(0, 0, 100), Vec3(0, 0, 0), Vec3(0, 1, 0));
  scene.setCamera(camera);

  // Create cubes with different colors
  Plane plane(Vec3(0, 0, 0), Vec3(0, 255, 0), 100, 10);

  for (auto &t : plane.toTriangles())
  {
    scene.addTriangle(t);
  }

  unsigned char *data = scene.getData(width, height);
  Image::createBMP("triangle.bmp", width, height, data);
  delete[] data;

  // Image::createAnimatedFrames("cube_animation", scene, width, height, 120);

  std::cout << "3D cubes rendered successfully!" << std::endl;
  return 0;
}