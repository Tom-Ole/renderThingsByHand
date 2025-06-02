#include <iostream>

#include "./vec.cpp"
#include "./triangle.cpp"
#include "./renderer.cpp"


int main() {

  
  Vec2 a = Vec2(1, 2);
  Vec2 b = Vec2(3, 5);
  Vec2 c = Vec2(5, 2);

  Triangle t = Triangle(a, b, c);
  
  int width = 800;
  int height = 600;

  Renderer::createBMP("triangle.bmp", width, height, t.createTriangleData(a, b, c, width, height));

  return 0;
}
