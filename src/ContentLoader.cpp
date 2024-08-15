#include "PECT/ContentLoader.h"

#include <wx/wx.h>

namespace PECT
{
    std::shared_ptr<std::uint8_t[]> ContentLoader::LoadPNG(const std::string& path, std::size_t* w, std::size_t* h)
    {
        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png)
        {
            throw std::string("libpng error");
        }

        png_infop info = png_create_info_struct(png);

        if (!info)
        {
            png_destroy_read_struct(&png, NULL, NULL);
            throw std::string("libpng error");
        }

        std::FILE* fp = NULL;

        if (setjmp(png_jmpbuf(png)))
        {
            if (fp)
            {
                std::fclose(fp);
            }
            
            png_destroy_read_struct(&png, &info, NULL);
            throw std::string("libpng error");
        }

        fp = std::fopen(path.c_str(), "rb");

        if (!fp)
        {
            png_destroy_read_struct(&png, &info, NULL);
            throw std::string("libpng error");
        }

        png_init_io(png, fp);
        png_read_info(png, info);
        std::uint32_t width = png_get_image_width(png, info);
        std::uint32_t height = png_get_image_height(png, info);
        std::uint8_t colour_type = png_get_color_type(png, info);
        std::uint8_t bit_depth = png_get_bit_depth(png, info);

        if (bit_depth < 8)
        {
            png_set_expand(png);
        }
        else if (bit_depth == 16)
        {
            png_set_strip_16(png);
        }

        if (colour_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        {
            png_set_expand_gray_1_2_4_to_8(png);
        }

        if (png_get_valid(png, info, PNG_INFO_tRNS))
        {
            png_set_tRNS_to_alpha(png);
        }

        if (colour_type == PNG_COLOR_TYPE_RGB || colour_type == PNG_COLOR_TYPE_GRAY || colour_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
        }

        if (colour_type == PNG_COLOR_TYPE_GRAY || colour_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        {
            png_set_gray_to_rgb(png);
        }

        png_read_update_info(png, info);
        png_bytep* row_pointers = (png_bytep*)std::malloc(sizeof(png_bytep) * height);

        for (std::uint32_t y = 0; y < height; ++y)
        {
            row_pointers[y] = (png_byte*)std::malloc(png_get_rowbytes(png, info));
        }

        png_read_image(png, row_pointers);
        png_destroy_read_struct(&png, &info, NULL);
        std::fclose(fp);
        fp = NULL;
        std::uint32_t pos = 0;
        std::shared_ptr<std::uint8_t[]> data = std::make_shared<std::uint8_t[]>(width * height * 4);

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
        *w = width;
        *h = height;
        return data;
    }

    std::shared_ptr<FontData> ContentLoader::LoadFont(const std::string& path, std::uint16_t size)
    {
        FT_Library library;

        if (FT_Init_FreeType(&library))
        {
            throw std::string("freetype error");
        }

        FT_Face face;
        FT_Error error = FT_New_Face(library, path.c_str(), 0, &face);

        if (error == FT_Err_Unknown_File_Format)
        {
            FT_Done_FreeType(library);
            throw std::string("invalid font file");
        }
        else if (error)
        {
            FT_Done_FreeType(library);
            throw std::string("couldn't open file");
        }

        if (FT_Set_Pixel_Sizes(face, size, size))
        {
            FT_Done_Face(face);
            FT_Done_FreeType(library);
            throw std::string("invalid font file");
        }

        std::uint16_t ascender = face->ascender >> 6;
        std::uint16_t descender = face->descender >> 6;
        std::uint16_t lineSpacing = face->height >> 6;
        std::vector<FontCharData> chars;

        for (std::uint8_t asciiCode = 32U; asciiCode <= 126U; ++asciiCode)
        {
            FT_UInt charIndex = FT_Get_Char_Index(face, asciiCode);

            if (charIndex == 0 ||
                FT_Load_Glyph(face, charIndex, FT_LOAD_DEFAULT) ||
                FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL) ||
                face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
            {
                continue;
            }

            if (face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
            {
                FontCharData fontChar(asciiCode, face->glyph->bitmap_left, face->glyph->bitmap_top, static_cast<std::int32_t>(face->glyph->advance.x >> 6), face->glyph->bitmap.width, face->glyph->bitmap.rows);

                if (fontChar.Width != 0 && fontChar.Height != 0)
                {
                    fontChar.Data = std::make_shared<std::uint8_t[]>(fontChar.Width * fontChar.Height * 4);
                    std::size_t pos = 0;
                    std::uint8_t* dataPtr = fontChar.Data.get();

                    for (std::int64_t y = 0; y < fontChar.Height; ++y)
                    {
                        for (std::int64_t x = 0; x < fontChar.Width; ++x)
                        {
                            dataPtr[pos++] = 0xFF;
                            dataPtr[pos++] = 0xFF;
                            dataPtr[pos++] = 0xFF;
                            dataPtr[pos++] = face->glyph->bitmap.buffer[y * fontChar.Width + x];
                        }
                    }
                }

                chars.push_back(std::move(fontChar));
            }
        }

        FT_Done_Face(face);
        FT_Done_FreeType(library);

        return std::make_shared<FontData>(ascender, descender, lineSpacing, std::move(chars));
    }
}