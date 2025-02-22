#ifndef PECT_FONTCHARDATA_H_
#define PECT_FONTCHARDATA_H_

#include <cstdint>
#include <memory>
#include <vector>

namespace PECT
{
    struct FontCharData
    {
        std::uint8_t Code;
        std::int32_t BearingX, BearingY, Advance;
        std::uint16_t Width, Height;
        std::shared_ptr<char[]> Data;

        inline FontCharData(std::uint8_t c, std::int32_t bx, std::int32_t by, std::int32_t a, std::uint16_t w, std::uint16_t h, std::shared_ptr<char[]> d) : Code(c), BearingX(bx), BearingY(by), Advance(a), Width(w), Height(h), Data(d) { }
        inline FontCharData(std::uint8_t c, std::int32_t bx, std::int32_t by, std::int32_t a, std::uint16_t w, std::uint16_t h) : Code(c), BearingX(bx), BearingY(by), Advance(a), Width(w), Height(h) { }

        inline bool HasTexture() const noexcept { return Width != 0 && Height != 0; }
    };

    struct FontData
    {
        std::uint16_t Ascender, Descender, LineSpacing;
        std::vector<FontCharData> Chars;
        
        inline FontData(std::uint16_t a, std::uint16_t d, std::uint16_t l, std::vector<FontCharData> c) : Ascender(a), Descender(d), LineSpacing(l), Chars(c) { }
        inline FontData(std::uint16_t a, std::uint16_t d, std::uint16_t l) : Ascender(a), Descender(d), LineSpacing(l) { }
    };
}

#endif