#ifndef PECT_PAGETEXTURE_H_
#define PECT_PAGETEXTURE_H_

#include <cstdint>
#include <memory>
#include <vector>
#include <string>

#include <wx/bitmap.h>
#include <wx/rawbmp.h>

namespace PECT
{
    struct PageTexture
    {
        std::string Name;

        // if false, is a regular texture, else is a font character collection
        bool IsFont;
        std::uint8_t Code;
        std::int32_t BearingX, BearingY, Advance;

        std::uint16_t X, Y, Width, Height;
        std::shared_ptr<char[]> Data;
        wxBitmap Bitmap;

        // for a texture
        PageTexture(const std::string&, std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t, std::shared_ptr<char[]>);
        // for a font
        PageTexture(const std::string&, std::uint16_t, std::uint16_t, std::uint16_t, std::uint16_t, std::shared_ptr<char[]>, std::uint8_t, std::int32_t, std::int32_t, std::int32_t);
        ~PageTexture();
    };
}

#endif