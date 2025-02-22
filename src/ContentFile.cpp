#include "PECT/ContentFile.h"

#include <brotli/encode.h>
#include <brotli/decode.h>

namespace PECT
{
    const char ContentFile::m_FileHeader[8] = { 80, 69, 67, 70, 2, 3, 1, 7 };
    std::uint32_t ContentFile::m_FileVersion = 1;

    std::uint32_t ContentFile::ReadUInt32(unsigned char* buffer)
    {
        return std::uint32_t((buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]));
    }

    std::uint16_t ContentFile::ReadUInt16(unsigned char* buffer)
    {
        return std::uint16_t((buffer[0] << 8) | (buffer[1]));
    }

    void ContentFile::WriteUInt32(std::uint32_t val, unsigned char* buf)
    {
        buf[0] = (val >> 24) & 0xFF;
        buf[1] = (val >> 16) & 0xFF;
        buf[2] = (val >> 8) & 0xFF;
        buf[3] = val & 0xFF;
    }

    void ContentFile::WriteUInt16(std::uint16_t val, unsigned char* buf)
    {
        buf[0] = (val >> 8) & 0xFF;
        buf[1] = val & 0xFF;
    }

    ContentFile::ContentFile() : m_PageSize(4096)
    {

    }

    ContentFile::~ContentFile()
    {

    }

    void ContentFile::CheckName(const std::string& n)
    {
        for (auto& page : m_Pages)
        {
            for (auto& texture : page->GetPageTextures())
            {
                if (texture->Name == n)
                {
                    throw std::string("name taken");
                }
            }
        }

        for (auto& entry : m_FontEntries)
        {
            if (entry.Name == n)
            {
                throw std::string("name taken");
            }
        }
    }

    void ContentFile::AddTexture(const std::string& name, std::uint16_t w, std::uint16_t h, std::shared_ptr<char[]> d)
    {
        CheckName(name);

        if (w > m_PageSize || h > m_PageSize)
        {
            throw std::string("image too large");
        }

        if (m_Pages.size() == 0)
        {
            std::shared_ptr<AtlasPage> page = std::make_shared<AtlasPage>(m_PageSize, m_PageSize);
            m_Pages.push_back(page);

            if (!page->AddTexture(name, w, h, d))
            {
                throw std::string("impossible 1");
            }

            return;
        }
        else
        {
            for (auto page : m_Pages)
            {
                if (page->AddTexture(name, w, h, d))
                {
                    return;
                }
            }

            std::shared_ptr<AtlasPage> page = std::make_shared<AtlasPage>(m_PageSize, m_PageSize);
            m_Pages.push_back(page);

            if (!page->AddTexture(name, w, h, d))
            {
                throw std::string("impossible 2");
            }
        }
    }

    bool ContentFile::RemoveTexture(const std::string& name)
    {
        for (auto& page : m_Pages)
        {
            auto it = page->m_Textures.begin();

            while (it != page->m_Textures.end())
            {
                if ((*it)->Name == name)
                {
                    page->m_Textures.erase(it);
                    return true;
                }

                ++it;
            }
        }

        return false;
    }

    void ContentFile::AddFont(const std::string& name, std::shared_ptr<FontData> data)
    {
        CheckName(name);
        m_FontEntries.emplace_back(name, data->Ascender, data->Descender, data->LineSpacing);
        std::uint32_t fontIndex = 0;

        for (; fontIndex < m_FontEntries.size(); ++fontIndex)
        {
            if (m_FontEntries[fontIndex].Name == name)
            {
                break;
            }
        }

        for (auto& c : data->Chars)
        {
            if (m_Pages.size() == 0)
            {
                std::shared_ptr<AtlasPage> page = std::make_shared<AtlasPage>(m_PageSize, m_PageSize);
                m_Pages.push_back(page);

                if (!page->AddFontTexture(name, c.Width, c.Height, c.Data, fontIndex, c.Code, c.BearingX, c.BearingY, c.Advance))
                {
                    throw std::string("impossible 1");
                }
            }
            else
            {
                bool added = false;

                for (auto page : m_Pages)
                {
                    if (page->AddFontTexture(name, c.Width, c.Height, c.Data, fontIndex, c.Code, c.BearingX, c.BearingY, c.Advance))
                    {
                        added = true;
                    }
                }

                if (!added)
                {
                    std::shared_ptr<AtlasPage> page = std::make_shared<AtlasPage>(m_PageSize, m_PageSize);
                    m_Pages.push_back(page);

                    if (!page->AddFontTexture(name, c.Width, c.Height, c.Data, fontIndex, c.Code, c.BearingX, c.BearingY, c.Advance))
                    {
                        throw std::string("impossible 2");
                    }
                }
            }
        }
    }

    bool ContentFile::RemoveFont(const std::string& name)
    {
        auto it = m_FontEntries.begin();
        bool found = false;

        while (!found && it != m_FontEntries.end())
        {
            if ((*it).Name == name)
            {
                m_FontEntries.erase(it);
                found = true;
            }

            ++it;
        }

        if (!found)
        {
            return false;
        }

        for (auto& page : m_Pages)
        {
            auto it = page->m_Textures.begin();

            while (it != page->m_Textures.end())
            {
                if ((*it)->Name == name)
                {
                    page->m_Textures.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        return true;
    }

    void ContentFile::SaveToFile(const std::string& path)
    {
        char buf[255];
        std::ofstream of(path, std::ios::binary);

        if (of.bad())
        {
            throw std::string("of is bad");
        }

        if (!of.is_open())
        {
            throw std::string("of is not open");
        }

        of.write(m_FileHeader, 8);
        WriteUInt32(m_FileVersion, reinterpret_cast<unsigned char*>(buf));
        of.write(buf, 4);
        WriteUInt16(m_Pages.size(), reinterpret_cast<unsigned char*>(buf));
        of.write(buf, 2);
        std::uint32_t textureCount = 0;

        for (auto& page : m_Pages)
        {
            textureCount += page->GetPageTextures().size();
        }

        WriteUInt32(textureCount, reinterpret_cast<unsigned char*>(buf));
        of.write(buf, 4);

        WriteUInt32(m_FontEntries.size(), reinterpret_cast<unsigned char*>(buf));
        of.write(buf, 4);

        for (auto& fontEntry : m_FontEntries)
        {
            WriteUInt16(fontEntry.Ascender, reinterpret_cast<unsigned char*>(buf));
            WriteUInt16(fontEntry.Descender, reinterpret_cast<unsigned char*>(buf + 2));
            WriteUInt16(fontEntry.LineSpacing, reinterpret_cast<unsigned char*>(buf + 4));
            buf[6] = fontEntry.Name.size();
            of.write(buf, 7);
            of.write(fontEntry.Name.c_str(), fontEntry.Name.size());
        }
        
        for (std::uint16_t pageNum = 0; pageNum < m_Pages.size(); ++pageNum)
        {
            for (auto& texture : m_Pages[pageNum]->GetPageTextures())
            {
                if (!texture->IsFont)
                {
                    buf[0] = 0; // this is a texture
                    of.write(buf, 1);

                    WriteUInt16(pageNum, reinterpret_cast<unsigned char*>(buf));
                    WriteUInt16(texture->X, reinterpret_cast<unsigned char*>(buf + 2));
                    WriteUInt16(texture->Y, reinterpret_cast<unsigned char*>(buf + 4));
                    WriteUInt16(texture->Width, reinterpret_cast<unsigned char*>(buf + 6));
                    WriteUInt16(texture->Height, reinterpret_cast<unsigned char*>(buf + 8));
                    of.write(buf, 10);

                    buf[0] = static_cast<std::uint8_t>(texture->Name.length());
                    of.write(buf, 1);
                    of.write(texture->Name.c_str(), texture->Name.length());
                    
                    std::size_t dataSize = texture->Width * texture->Height * 4;

                    if (dataSize > 1024)
                    {
                        std::uint8_t compressQuality = BROTLI_MAX_QUALITY / 2;
                        buf[0] = compressQuality;
                        of.write(buf, 1);
                        std::unique_ptr<char[]> compressOutput = std::make_unique<char[]>(dataSize);
                        std::size_t compressSize = dataSize;

                        if (BrotliEncoderCompress(compressQuality, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE,
                        dataSize, reinterpret_cast<uint8_t*>(texture->Data.get()), &compressSize, reinterpret_cast<uint8_t*>(compressOutput.get())) == BROTLI_FALSE)
                        {
                            throw std::string("compress failure");
                        }

                        WriteUInt32(static_cast<std::uint32_t>(compressSize), reinterpret_cast<unsigned char*>(buf));
                        of.write(buf, 4);
                        of.write(compressOutput.get(), compressSize);
                    }
                    else
                    {
                        buf[0] = 0;
                        of.write(buf, 1);
                        of.write(texture->Data.get(), dataSize);
                    }
                }
                else
                {
                    buf[0] = 1; // this is a font character
                    of.write(buf, 1);

                    WriteUInt16(pageNum, reinterpret_cast<unsigned char*>(buf));
                    WriteUInt16(texture->X, reinterpret_cast<unsigned char*>(buf + 2));
                    WriteUInt16(texture->Y, reinterpret_cast<unsigned char*>(buf + 4));
                    WriteUInt16(texture->Width, reinterpret_cast<unsigned char*>(buf + 6));
                    WriteUInt16(texture->Height, reinterpret_cast<unsigned char*>(buf + 8));
                    buf[10] = texture->Code;
                    WriteUInt32(texture->BearingX, reinterpret_cast<unsigned char*>(buf + 11));
                    WriteUInt32(texture->BearingY, reinterpret_cast<unsigned char*>(buf + 15));
                    WriteUInt32(texture->Advance, reinterpret_cast<unsigned char*>(buf + 19));
                    of.write(buf, 23);

                    buf[0] = static_cast<std::uint8_t>(texture->Name.length());
                    of.write(reinterpret_cast<const char*>(buf), 1);
                    of.write(texture->Name.c_str(), texture->Name.length());

                    if (texture->Width != 0 && texture->Height != 0)
                    {
                        std::size_t dataSize = texture->Width * texture->Height * 4;

                        if (dataSize > 1024)
                        {
                            std::uint8_t compressQuality = BROTLI_MAX_QUALITY / 2;
                            buf[0] = compressQuality;
                            of.write(buf, 1);
                            std::unique_ptr<char[]> compressOutput = std::make_unique<char[]>(dataSize);
                            std::size_t compressSize = dataSize;

                            if (BrotliEncoderCompress(compressQuality, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE,
                            dataSize, reinterpret_cast<uint8_t*>(texture->Data.get()), &compressSize, reinterpret_cast<uint8_t*>(compressOutput.get())) == BROTLI_FALSE)
                            {
                                throw std::string("compress failure");
                            }

                            WriteUInt32(static_cast<std::uint32_t>(compressSize), reinterpret_cast<unsigned char*>(buf));
                            of.write(buf, 4);
                            of.write(compressOutput.get(), compressSize);
                        }
                        else
                        {
                            buf[0] = 0;
                            of.write(buf, 1);
                            of.write(texture->Data.get(), dataSize);
                        }
                    }
                }
            }
        }
    }

    static void PECT_CloseFile(std::FILE* fp) noexcept(true)
    {
        std::fclose(fp);
    }

    std::shared_ptr<ContentFile> ContentFile::LoadFromFile(const std::string& path)
    {
        std::unique_ptr<std::FILE, decltype(&PECT_CloseFile)> fp(std::fopen(path.c_str(), "rb"), &PECT_CloseFile);

        if (!fp) { throw std::string("could not open file"); }

        char buffer[255];

        if (std::fread(&buffer, 1, 22, fp.get()) != 22) { throw std::string("not a valid content file 1"); }

        if (std::memcmp(&buffer, &m_FileHeader, 8) != 0) { throw std::string("not a valid content file 2"); }

        std::uint32_t version = ReadUInt32(reinterpret_cast<unsigned char*>(&buffer[8]));
        std::uint16_t atlasPages = ReadUInt16(reinterpret_cast<unsigned char*>(&buffer[12]));
        std::uint32_t textureListSize = ReadUInt32(reinterpret_cast<unsigned char*>(&buffer[14]));
        std::uint32_t fontListSize = ReadUInt32(reinterpret_cast<unsigned char*>(&buffer[18]));

        std::shared_ptr<ContentFile> contentFile = std::make_shared<ContentFile>();

        for (std::uint16_t i = 0; i < atlasPages; ++i)
        {
            contentFile->m_Pages.push_back(std::make_shared<AtlasPage>(contentFile->m_PageSize, contentFile->m_PageSize));
        }

        std::uint16_t ascender, descender, lineSpacing;
        std::uint8_t nameLen;

        for (std::uint32_t i = 0; i < fontListSize; ++i)
        {
            if (std::fread(buffer, 1, 7, fp.get()) != 7) { throw std::string("not a valid content file 3"); }

            ascender = ReadUInt16(reinterpret_cast<unsigned char*>(buffer));
            descender = ReadUInt16(reinterpret_cast<unsigned char*>(buffer + 2));
            lineSpacing = ReadUInt16(reinterpret_cast<unsigned char*>(buffer + 4));
            nameLen = buffer[6];

            if (std::fread(buffer, 1, nameLen, fp.get()) != nameLen) { throw std::string("not a valid content file 4"); }

            contentFile->m_FontEntries.emplace_back(std::string(buffer, nameLen), ascender, descender, lineSpacing);
        }

        std::uint16_t pageNum, x, y, w, h;
        std::size_t imageDataSize;
        std::uint8_t compressType;
        std::int32_t bearingX, bearingY, advance;
        std::uint8_t code;

        for (std::uint32_t i = 0; i < textureListSize; ++i)
        {
            if (std::fread(buffer, 1, 1, fp.get()) != 1) { throw std::string("not a valid content file 5"); }

            if (buffer[0] == 0)
            {
                if (std::fread(&buffer, 1, 11, fp.get()) != 11) { throw std::string("not a valid content file 6"); }

                pageNum = ReadUInt16(reinterpret_cast<unsigned char*>(buffer));
                x = ReadUInt16(reinterpret_cast<unsigned char*>(buffer + 2));
                y = ReadUInt16(reinterpret_cast<unsigned char*>(buffer + 4));
                w = ReadUInt16(reinterpret_cast<unsigned char*>(buffer + 6));
                h = ReadUInt16(reinterpret_cast<unsigned char*>(buffer + 8));
                imageDataSize = w * h * 4;
                nameLen = buffer[10];

                if (std::fread(&buffer, 1, nameLen, fp.get()) != nameLen) { throw std::string("not a valid content file 7"); }

                std::string name(buffer, nameLen);

                if (std::fread(&buffer, 1, 1, fp.get()) != 1) { throw std::string("not a valid content file 8"); }

                compressType = buffer[0];

                std::shared_ptr<char[]> imageData = std::make_shared<char[]>(imageDataSize);

                if (compressType == 0)
                {
                    if (std::fread(imageData.get(), 1, imageDataSize, fp.get()) != imageDataSize) { throw std::string("not a valid content file 9"); }
                }
                else
                {
                    if (std::fread(&buffer, 1, 4, fp.get()) != 4) { throw std::string("not a valid content file 10"); }

                    std::uint32_t compressLen = ReadUInt32(reinterpret_cast<unsigned char*>(buffer));
                    std::unique_ptr<char[]> compressedBuff = std::make_unique<char[]>(compressLen);

                    if (std::fread(compressedBuff.get(), 1, compressLen, fp.get()) != compressLen) { throw std::string("not a valid content file 11"); }

                    if (BrotliDecoderDecompress(compressLen, reinterpret_cast<uint8_t*>(compressedBuff.get()), &imageDataSize, reinterpret_cast<uint8_t*>(imageData.get())) != BROTLI_DECODER_RESULT_SUCCESS)
                    {
                        throw std::string("decompress error");
                    }
                }

                contentFile->m_Pages[pageNum]->m_Textures.push_back(std::make_shared<PageTexture>(name, x, y, w, h, imageData));
            }
            else if (buffer[0] == 1)
            {
                if (std::fread(&buffer, 1, 24, fp.get()) != 24) { throw std::string("not a valid content file 12"); }

                pageNum = ReadUInt16(reinterpret_cast<unsigned char*>(buffer));
                x = ReadUInt16(reinterpret_cast<unsigned char*>(buffer + 2));
                y = ReadUInt16(reinterpret_cast<unsigned char*>(buffer + 4));
                w = ReadUInt16(reinterpret_cast<unsigned char*>(buffer + 6));
                h = ReadUInt16(reinterpret_cast<unsigned char*>(buffer + 8));
                imageDataSize = w * h * 4;
                code = buffer[10];
                bearingX = ReadUInt32(reinterpret_cast<unsigned char*>(buffer + 11));
                bearingY = ReadUInt32(reinterpret_cast<unsigned char*>(buffer + 15));
                advance = ReadUInt32(reinterpret_cast<unsigned char*>(buffer + 19));
                nameLen = buffer[23];

                if (std::fread(&buffer, 1, nameLen, fp.get()) != nameLen) { throw std::string("not a valid content file 13"); }

                std::string name(reinterpret_cast<const char*>(&buffer), nameLen);
                std::shared_ptr<char[]> imageData;

                if (w != 0 && h != 0)
                {
                    if (std::fread(&buffer, 1, 1, fp.get()) != 1) { throw std::string("not a valid content file 14"); }

                    compressType = buffer[0];

                    imageData = std::make_shared<char[]>(imageDataSize);

                    if (compressType == 0)
                    {
                        if (std::fread(imageData.get(), 1, imageDataSize, fp.get()) != imageDataSize) { throw std::string("not a valid content file 15"); }
                    }
                    else
                    {
                        if (std::fread(&buffer, 1, 4, fp.get()) != 4) { throw std::string("not a valid content file 16"); }

                        std::uint32_t compressLen = ReadUInt32(reinterpret_cast<unsigned char*>(buffer));
                        std::unique_ptr<std::uint8_t[]> compressedBuff = std::make_unique<std::uint8_t[]>(compressLen);

                        if (std::fread(compressedBuff.get(), 1, compressLen, fp.get()) != compressLen) { throw std::string("not a valid content file 17"); }

                        if (BrotliDecoderDecompress(compressLen, compressedBuff.get(), &imageDataSize, reinterpret_cast<uint8_t*>(imageData.get())) != BROTLI_DECODER_RESULT_SUCCESS)
                        {
                            throw std::string("decompress error");
                        }
                    }
                }

                contentFile->m_Pages[pageNum]->m_Textures.push_back(std::make_shared<PageTexture>(name, x, y, w, h, imageData, code, bearingX, bearingY, advance));
            }
            else
            {
                throw std::string("not a valid content file 18");
            }

            
        }

        return contentFile;
    }
}