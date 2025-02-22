#include "PECT/PageTexture.h"

namespace PECT
{
    PageTexture::PageTexture(const std::string& n, std::uint16_t x, std::uint16_t y, std::uint16_t w, std::uint16_t h, std::shared_ptr<char[]> d)
        : Name(n), IsFont(false), X(x), Y(y), Width(w), Height(h), Data(d)
    {
        if (!Bitmap.Create(Width, Height, 32))
        {
            throw std::string("impossible 1");
        }

        wxAlphaPixelData pixelData(Bitmap);

        if (!pixelData)
        {
            throw std::string("impossible 2");
        }

        wxAlphaPixelData::Iterator it(pixelData);
        std::size_t i = 0;

        for (std::size_t y = 0; y < Height; ++y)
        {
            wxAlphaPixelData::Iterator row = it;

            for (std::size_t x = 0; x < Width; ++x)
            {
                it.Red() = Data.get()[i++];
                it.Green() = Data.get()[i++];
                it.Blue() = Data.get()[i++];
                it.Alpha() = Data.get()[i++];
                ++it;
            }

            it = row;
            it.OffsetY(pixelData, 1);
        }
    }

    PageTexture::PageTexture(const std::string& n, std::uint16_t x, std::uint16_t y, std::uint16_t w, std::uint16_t h, std::shared_ptr<char[]> d, std::uint8_t c, std::int32_t bx, std::int32_t by, std::int32_t a)
        : Name(n), IsFont(true), X(x), Y(y), Width(w), Height(h), Data(d), Code(c), BearingX(bx), BearingY(by), Advance(a)
    {
        if (Width == 0 || Height == 0)
        {
            return;
        }

        if (!Bitmap.Create(Width, Height, 32))
        {
            throw std::string("impossible 1");
        }

        wxAlphaPixelData pixelData(Bitmap);

        if (!pixelData)
        {
            throw std::string("impossible 2");
        }

        wxAlphaPixelData::Iterator it(pixelData);
        std::size_t i = 0;

        for (std::size_t y = 0; y < Height; ++y)
        {
            wxAlphaPixelData::Iterator row = it;

            for (std::size_t x = 0; x < Width; ++x)
            {
                it.Red() = 0;
                it.Green() = 0;
                it.Blue() = 0;
                i += 3;
                it.Alpha() = Data.get()[i++];
                ++it;
            }

            it = row;
            it.OffsetY(pixelData, 1);
        }
    }

    PageTexture::~PageTexture()
    {
        
    }
}