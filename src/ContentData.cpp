#include "PECT/ContentData.h"

namespace PECT
{
    ImageData::ImageData(std::size_t w, std::size_t h, std::unique_ptr<unsigned char[]> d)
        : m_Width(w), m_Height(h), m_Data(std::move(d))
    {
        if (!m_Bitmap.Create(m_Width, m_Height, 32))
        {
            throw std::string("impossible 1");
        }

        wxAlphaPixelData pixelData(m_Bitmap);

        if (!pixelData)
        {
            throw std::string("impossible 2");
        }

        wxAlphaPixelData::Iterator it(pixelData);
        std::size_t i = 0;

        for (std::size_t y = 0; y < m_Height; ++y)
        {
            wxAlphaPixelData::Iterator row = it;

            for (std::size_t x = 0; x < m_Width; ++x)
            {
                it.Red() = m_Data.get()[i++];
                it.Green() = m_Data.get()[i++];
                it.Blue() = m_Data.get()[i++];
                it.Alpha() = m_Data.get()[i++];
                ++it;
            }

            it = row;
            it.OffsetY(pixelData, 1);
        }
    }
}