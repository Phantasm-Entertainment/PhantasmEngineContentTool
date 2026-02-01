#ifndef PECT_FONTCHARDATA_H_
#define PECT_FONTCHARDATA_H_

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

namespace PECT
{
    struct FontCharData
    {
        std::uint8_t Code;
        std::int32_t BearingX, BearingY, Advance;
        std::uint16_t Width, Height;
        std::unique_ptr<unsigned char[]> Data;

        //inline FontCharData(std::uint8_t c, std::int32_t bx, std::int32_t by, std::int32_t a, std::uint16_t w, std::uint16_t h, std::unique_ptr<unsigned char[]> d) : Code(c), BearingX(bx), BearingY(by), Advance(a), Width(w), Height(h), Data(std::move(d)) { }
        inline FontCharData(std::uint8_t c, std::int32_t bx, std::int32_t by, std::int32_t a, std::uint16_t w, std::uint16_t h) : Code(c), BearingX(bx), BearingY(by), Advance(a), Width(w), Height(h) { }

        inline FontCharData(FontCharData&& other) noexcept
        : Code(other.Code), BearingX(other.BearingX), BearingY(other.BearingY), Advance(other.Advance),
        Width(other.Width), Height(other.Height), Data(std::move(other.Data)) { }

        inline FontCharData(const FontCharData& other) noexcept
        : Code(other.Code), BearingX(other.BearingX), BearingY(other.BearingY), Advance(other.Advance),
        Width(other.Width), Height(other.Height)
        {
            Data = std::make_unique<unsigned char[]>(Width * Height * 4);
            std::memcpy(Data.get(), other.Data.get(), Width * Height * 4);
        }

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