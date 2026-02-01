#include "PECT/ContentLoader.h"

namespace PECT
{
    void ptr_close_file(std::FILE* fp) { std::fclose(fp); }

    class WrPngReadStruct // wrapper for png_structp
    {
    private:
        png_structp m_Handle;
    public:
        WrPngReadStruct() noexcept : m_Handle(png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) {}
        ~WrPngReadStruct() noexcept { if (m_Handle != NULL) { png_destroy_read_struct(&m_Handle, NULL, NULL); } }
        const png_structp operator()() const noexcept { return m_Handle; }
        operator bool() const noexcept { return m_Handle != NULL; }
    };

    class WrPngInfoStruct // wrapper for png_infop
    {
    private:
        png_structp m_Png;
        png_infop m_Handle;
    public:
        WrPngInfoStruct(png_structp png) noexcept : m_Png(png), m_Handle(png_create_info_struct(m_Png)) {}
        ~WrPngInfoStruct() noexcept { if (m_Handle != NULL) { png_destroy_info_struct(m_Png, &m_Handle); } }
        const png_infop operator()() const noexcept { return m_Handle; }
        operator bool() const noexcept { return m_Handle != NULL; }
    };

    std::expected<ImageData, std::string> ContentLoader::LoadPNG(const std::string& path)
    {
        WrPngReadStruct png;

        if (!png) { return std::unexpected("libpng error"); }

        WrPngInfoStruct info(png());

        if (!info) { return std::unexpected("libpng error"); }

        if (setjmp(png_jmpbuf(png()))) { return std::unexpected("libpng error"); }

        std::unique_ptr<std::FILE, decltype(&ptr_close_file)> fp(std::fopen(path.c_str(), "rb"), &ptr_close_file);

        if (!fp) { return std::unexpected("libpng error"); }

        png_init_io(png(), fp.get());
        png_read_info(png(), info());
        std::uint32_t width = png_get_image_width(png(), info());
        std::uint32_t height = png_get_image_height(png(), info());
        std::uint8_t colour_type = png_get_color_type(png(), info());
        std::uint8_t bit_depth = png_get_bit_depth(png(), info());

        if (bit_depth < 8)
        {
            png_set_expand(png());
        }
        else if (bit_depth == 16)
        {
            png_set_strip_16(png());
        }

        if (colour_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        {
            png_set_expand_gray_1_2_4_to_8(png());
        }

        if (png_get_valid(png(), info(), PNG_INFO_tRNS))
        {
            png_set_tRNS_to_alpha(png());
        }

        if (colour_type == PNG_COLOR_TYPE_RGB || colour_type == PNG_COLOR_TYPE_GRAY || colour_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_filler(png(), 0xFF, PNG_FILLER_AFTER);
        }

        if (colour_type == PNG_COLOR_TYPE_GRAY || colour_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        {
            png_set_gray_to_rgb(png());
        }

        png_read_update_info(png(), info());
        png_bytep* row_pointers = (png_bytep*)std::malloc(sizeof(png_bytep) * height);

        for (std::uint32_t y = 0; y < height; ++y)
        {
            row_pointers[y] = (png_byte*)std::malloc(png_get_rowbytes(png(), info()));
        }

        png_read_image(png(), row_pointers);
        std::uint32_t pos = 0;
        std::unique_ptr<unsigned char[]> data = std::make_unique<unsigned char[]>(width * height * 4);

        for (std::int32_t y = 0; y < height; ++y)
        {
            for (std::int32_t x = 0; x < width * 4; ++x)
            {
                data.get()[pos++] = row_pointers[y][x];
            }
        }

        for (std::uint32_t y = 0; y < height; ++y)
        {
            std::free(row_pointers[y]);
        }

        std::free(row_pointers);
        return ImageData(width, height, std::move(data));
    }

    class WrFtLibrary // wrapper for FT_Library
    {
    private:
        FT_Library m_Handle;
        FT_Error m_Error;
    public:
        WrFtLibrary() noexcept { m_Error = FT_Init_FreeType(&m_Handle); }
        ~WrFtLibrary() noexcept { if (m_Error == 0) { FT_Done_FreeType(m_Handle); } }
        const FT_Library operator()() const noexcept { return m_Handle; }
        const FT_Error Error() const noexcept { return m_Error; }
    };

    class WrFtFace // wrapper for FT_Face
    {
    private:
        FT_Face m_Face;
        FT_Error m_Error;
    public:
        WrFtFace(FT_Library lib, const char* path) noexcept { m_Error = FT_New_Face(lib, path, 0, &m_Face); }
        ~WrFtFace() noexcept { if (m_Error == 0) { FT_Done_Face(m_Face); } }
        const FT_Face operator()() const noexcept { return m_Face; }
        const FT_Error Error() const noexcept { return m_Error; }
    };

    std::expected<FontData, std::string> ContentLoader::LoadFont(const std::string& path, std::uint16_t size)
    {
        WrFtLibrary library;

        if (library.Error() != 0) { return std::unexpected("freetype error"); }

        WrFtFace face(library(), path.c_str());

        if (face.Error() == FT_Err_Unknown_File_Format) { return std::unexpected("invalid font file"); }
        else if (face.Error() != 0) { return std::unexpected("couldn't open file"); }

        if (FT_Set_Pixel_Sizes(face(), size, size))
        {
            return std::unexpected("invalid font file");
        }

        std::uint16_t ascender = face()->ascender >> 6;
        std::uint16_t descender = face()->descender >> 6;
        std::uint16_t lineSpacing = face()->height >> 6;
        std::vector<FontCharData> chars;

        for (std::uint8_t asciiCode = 32U; asciiCode <= 126U; ++asciiCode)
        {
            FT_UInt charIndex = FT_Get_Char_Index(face(), asciiCode);

            if (charIndex == 0 ||
                FT_Load_Glyph(face(), charIndex, FT_LOAD_DEFAULT) ||
                FT_Render_Glyph(face()->glyph, FT_RENDER_MODE_NORMAL) ||
                face()->glyph->format != FT_GLYPH_FORMAT_BITMAP)
            {
                continue;
            }

            if (face()->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
            {
                FontCharData fontChar(asciiCode, face()->glyph->bitmap_left, face()->glyph->bitmap_top, static_cast<std::int32_t>(face()->glyph->advance.x >> 6), face()->glyph->bitmap.width, face()->glyph->bitmap.rows);

                if (fontChar.m_Width != 0 && fontChar.m_Height != 0)
                {
                    fontChar.m_Data = std::make_unique<unsigned char[]>(fontChar.m_Width * fontChar.m_Height * 4);
                    std::size_t pos = 0;
                    unsigned char* dataPtr = fontChar.m_Data.get();

                    for (std::int64_t y = 0; y < fontChar.m_Height; ++y)
                    {
                        for (std::int64_t x = 0; x < fontChar.m_Width; ++x)
                        {
                            dataPtr[pos++] = 0xFF;
                            dataPtr[pos++] = 0xFF;
                            dataPtr[pos++] = 0xFF;
                            dataPtr[pos++] = face()->glyph->bitmap.buffer[y * fontChar.m_Width + x];
                        }
                    }
                }

                chars.push_back(std::move(fontChar));
            }
        }

        return FontData(ascender, descender, lineSpacing, std::move(chars));
    }
}