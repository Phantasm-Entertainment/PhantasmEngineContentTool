#ifndef PECT_CONTENTDATA_H_
#define PECT_CONTENTDATA_H_

#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>
#include <unordered_map>

#include <wx/bitmap.h>
#include <wx/rawbmp.h>

namespace PECT
{
    class ImageData
    {
        friend class ContentLoader;
    private:
        std::size_t m_Width, m_Height;
        std::unique_ptr<unsigned char[]> m_Data;
        wxBitmap m_Bitmap;
    public:
        ImageData(std::size_t, std::size_t, std::unique_ptr<unsigned char[]>) noexcept;

        std::size_t GetWidth() const noexcept { return m_Width; }
        std::size_t GetHeight() const noexcept { return m_Height; }
        unsigned char* GetData() const noexcept { return m_Data.get(); }
        const wxBitmap& GetBitmap() const noexcept { return m_Bitmap; }
    };

    class FontCharData
    {
        friend class ContentLoader;
    private:
        std::uint8_t m_Code;
        std::int32_t m_BearingX, m_BearingY, m_Advance;
        std::uint16_t m_Width, m_Height;
        std::unique_ptr<unsigned char[]> m_Data;
    public:
        FontCharData(std::uint8_t c, std::int32_t bx, std::int32_t by, std::int32_t a, std::uint16_t w, std::uint16_t h) : m_Code(c), m_BearingX(bx), m_BearingY(by), m_Advance(a), m_Width(w), m_Height(h) { }

        FontCharData(FontCharData&& other) noexcept
        : m_Code(other.m_Code), m_BearingX(other.m_BearingX), m_BearingY(other.m_BearingY), m_Advance(other.m_Advance),
        m_Width(other.m_Width), m_Height(other.m_Height), m_Data(std::move(other.m_Data)) { }

        FontCharData(const FontCharData& other) noexcept
        : m_Code(other.m_Code), m_BearingX(other.m_BearingX), m_BearingY(other.m_BearingY), m_Advance(other.m_Advance),
        m_Width(other.m_Width), m_Height(other.m_Height)
        {
            if (other.m_Data)
            {
                m_Data = std::make_unique<unsigned char[]>(m_Width * m_Height * 4);
                std::memcpy(m_Data.get(), other.m_Data.get(), m_Width * m_Height * 4);
            }
        }

        bool HasTexture() const noexcept { return m_Width != 0 && m_Height != 0; }
    };

    class FontData
    {
        friend class ContentLoader;
    private:
        std::uint16_t m_Ascender, m_Descender, m_LineSpacing;
        std::vector<FontCharData> m_Chars;
    public:
        FontData(std::uint16_t a, std::uint16_t d, std::uint16_t l, std::vector<FontCharData> c) : m_Ascender(a), m_Descender(d), m_LineSpacing(l), m_Chars(std::move(c)) { }
    };
}

#endif