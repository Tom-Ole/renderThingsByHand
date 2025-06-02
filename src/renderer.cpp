#include "./vec.cpp"
#include <cstring>
#include <cstdint>

template<typename T>
void writeLE(unsigned char* buffer, int offset, T value) {
    static_assert(std::is_integral_v<T>, "Only integral types supported");
    for (size_t i = 0; i < sizeof(T); ++i) {
        buffer[offset + i] = static_cast<unsigned char>((value >> (8 * i)) & 0xFF);
    }
}

class Renderer
{
public:
    static auto createBMP(const char *filename, int width, int height, unsigned char* data) -> void
    {

        constexpr int headerSize = 14;
        constexpr int infoHeaderSize = 40;
        constexpr int pixelDataOffset = headerSize + infoHeaderSize;


        int fileSize = pixelDataOffset + width * height * 3;

        // Header for BMP file
        // Offset 0: bfType (2 bytes) "BM" (uint16_t)
        // Offset 2: bfSize (4 bytes) size of the file in bytes (uint32_t)
        // Offset 6: bfReserved1 (4 bytes) reserved, must be 0 (uint32_t)
        // Offset 10: bfOffBits (4 bytes) offset to the pixel data (uint32_t) (14 Byte Header + 40 Byte Infoblock)
        // InfoHeader for BMP file
        // Offset 14: biSize (4 bytes) size of this BITMAPINFOHEADER-Structure in Byte (uint32_t)
        // Offset 18: biWidth (4 bytes) width of the bitmap in pixels (int32_t)
        // Offset 22: biHeight (4 bytes) height of the bitmap in pixels (int32_t)
        // Offset 26: biPlanes (2 bytes) number of color planes, must be 1 (uint16_t)
        // Offset 28: biBitCount (2 bytes) number of bits per pixel (uint16_t) [24 for RGB]
        // Offset 30: biCompression (4 bytes) compression type (uint32_t) [0 for BI_RGB]
        // Offset 34: biSizeImage (4 bytes) size of the pixel data in bytes (uint32_t) [0 for BI_RGB]
        // Offset 38: biXPelsPerMeter (4 bytes) horizontal resolution in pixels per meter (uint32_t) [0]
        // Offset 42: biYPelsPerMeter (4 bytes) vertical resolution in pixels per meter (uint32_t) [0]
        // Offset 46: biClrUsed (4 bytes) number of colors in the color palette, 0 for default (uint32_t)
        // Offset 50: biClrImportant (4 bytes) number of important colors, 0 for all (uint32_t)

       unsigned char header[headerSize + infoHeaderSize];
        std::memset(header, 0, sizeof(header));
        // BMP file header (14 bytes)
        header[0] = 'B';
        header[1] = 'M';
        writeLE<uint32_t>(header, 2, fileSize);        // bfSize
        writeLE<uint32_t>(header, 6, 0);               // bfReserved1 and bfReserved2
        writeLE<uint32_t>(header, 10, pixelDataOffset);// bfOffBits

        // Info header (BITMAPINFOHEADER, 40 bytes)
        writeLE<uint32_t>(header, 14, infoHeaderSize);  // biSize
        writeLE<int32_t>(header, 18, width);            // biWidth
        writeLE<int32_t>(header, 22, height);           // biHeight
        writeLE<uint16_t>(header, 26, 1);               // biPlanes
        writeLE<uint16_t>(header, 28, 24);              // biBitCount (24 bits)
        writeLE<uint32_t>(header, 30, 0);               // biCompression (BI_RGB)
        writeLE<uint32_t>(header, 34, 0);               // biSizeImage (0 for BI_RGB)
        writeLE<int32_t>(header, 38, 0);                // biXPelsPerMeter
        writeLE<int32_t>(header, 42, 0);                // biYPelsPerMeter
        writeLE<uint32_t>(header, 46, 0);               // biClrUsed
        writeLE<uint32_t>(header, 50, 0);               // biClrImportant

        
        // Open file for writing
        FILE *file = fopen(filename, "wb");
        if (!file) {
            throw std::runtime_error("Could not open file for writing");
        }

        // Write the header
        fwrite(header, sizeof(unsigned char), sizeof(header), file);

        // Write pixel data
        for (int y = height - 1; y >= 0; --y) { // BMP format stores pixels bottom to top
            for (int x = 0; x < width; ++x) {
                unsigned char r = data[(y * width + x) * 3 + 0];
                unsigned char g = data[(y * width + x) * 3 + 1];
                unsigned char b = data[(y * width + x) * 3 + 2];
                fwrite(&b, sizeof(unsigned char), 1, file); // Blue
                fwrite(&g, sizeof(unsigned char), 1, file); // Green
                fwrite(&r, sizeof(unsigned char), 1, file); // Red
            }
        }

    };



    void render()
    {
    };


};