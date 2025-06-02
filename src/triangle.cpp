
#include <iostream>
#include <cstring>
#include <cstdint>

#include "./vec.cpp"

class Triangle {
public:
    Vec2 a, b, c; // lengths of the sides

    Triangle(const Vec2 &a, const Vec2 &b, const Vec2 &c) : a(a), b(b), c(c) {};
    float area() const {
        return 0.5f * std::abs((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));
    };

    bool isPointInTriangle(const Vec2 &p) const {
        float areaABC = area();
        float areaPAB = Triangle(p, a, b).area();
        float areaPBC = Triangle(p, b, c).area();
        float areaPCA = Triangle(p, c, a).area();
        return std::abs(areaABC - (areaPAB + areaPBC + areaPCA)) < 1e-6f; // Using a small epsilon for floating point comparison
    };

    unsigned char* createTriangleData(Vec2 a, Vec2 b, Vec2 c, int width, int height)
    {
        unsigned char* data = new unsigned char[width * height * 3];
        std::memset(data, 0, width * height * 3); // Initialize to black

        // Simple triangle rasterization algorithm
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Vec2 p(x, y);
                if (isPointInTriangle(p)) {
                    // Set pixel color to white if the point is inside the triangle
                    int index = (y * width + x) * 3;
                    data[index] = 255;     // Red
                    data[index + 1] = 255; // Green
                    data[index + 2] = 255; // Blue
                }
            }
        }

        return data;
    };




};