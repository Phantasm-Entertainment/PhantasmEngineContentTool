#include "PECT/ContentFile.h"

#include <brotli/encode.h>
#include <brotli/decode.h>

namespace PECT
{
    const unsigned char ContentFile::m_FileHeader[8] = { 80, 69, 67, 70, 2, 3, 1, 7 };
    const FileVersion ContentFile::m_FileVersion(1, 0, 0);
    const AtlasInt ContentFile::m_MaxPageSize = 4096;

    std::expected<AtlasPos, std::string> ContentFile::AddTexture(const std::string& name, ImageData& data)
    {
        if (IsNameTaken(name)) { return std::unexpected("content with that name already exists"); }

        if (data.GetWidth() > m_MaxPageSize || data.GetHeight() > m_MaxPageSize)
        {
            return std::unexpected("image too large");
        }

        if (m_Pages.empty())
        {
            auto& page = m_Pages.emplace_back(m_MaxPageSize, m_MaxPageSize);
            auto pos = page.AddTexture(name, data);

            if (!pos) { return std::unexpected("impossible"); }
            else { return *pos; }
        }

        for (auto& page : m_Pages)
        {
            auto pos = page.AddTexture(name, data);

            if (pos)
            {
                return *pos;
            }
        }

        auto& page = m_Pages.emplace_back(m_MaxPageSize, m_MaxPageSize);
        auto pos = page.AddTexture(name, data);
        if (!pos) { return std::unexpected("impossible"); }
        return *pos;
    }

    bool ContentFile::RemoveTexture(const std::string& name)
    {
        for (auto& page : m_Pages)
        {
            if (page.RemoveTexture(name))
            {
                return true;
            }
        }

        return false;
    }

    SaveResult ContentFile::Save(std::string path) noexcept
    {
        if (path == "")
        {
            if (m_SavePath.has_value())
            {
                path = *m_SavePath;
            }
            else
            {
                return SaveResult(true);
            }
        }
        else
        {
            m_SavePath = std::move(path);
        }

        std::basic_ofstream<unsigned char> of(m_SavePath.value(), std::ios::binary);

        if (of.fail())
        {
            return SaveResult("couldn't create file");
        }

        unsigned char buffer[32];

        // write header
        of.write(m_FileHeader, 8);

        // write file version
        WriteUInt32(m_FileVersion.GetMajor(), buffer);
        WriteUInt32(m_FileVersion.GetMinor(), buffer + 2);
        WriteUInt32(m_FileVersion.GetPatch(), buffer + 4);
        of.write(buffer, 6);

        

        return {};
    }

    // void ContentFile::AddFont(const std::string& name, const FontData& data)
    // {
    //     CheckName(name);
    //     m_FontEntries.emplace_back(name, data->Ascender, data->Descender, data->LineSpacing);
    //     std::uint32_t fontIndex = 0;

    //     for (; fontIndex < m_FontEntries.size(); ++fontIndex)
    //     {
    //         if (m_FontEntries[fontIndex].Name == name)
    //         {
    //             break;
    //         }
    //     }

    //     for (auto& c : data->Chars)
    //     {
    //         if (m_Pages.size() == 0)
    //         {
    //             auto& page = m_Pages.emplace_back(m_PageSize, m_PageSize);

    //             if (!page->AddFontTexture(name, c.Width, c.Height, c.Data, fontIndex, c.Code, c.BearingX, c.BearingY, c.Advance))
    //             {
    //                 throw std::string("impossible 1");
    //             }
    //         }
    //         else
    //         {
    //             bool added = false;

    //             for (auto page : m_Pages)
    //             {
    //                 if (page->AddFontTexture(name, c.Width, c.Height, c.Data, fontIndex, c.Code, c.BearingX, c.BearingY, c.Advance))
    //                 {
    //                     added = true;
    //                 }
    //             }

    //             if (!added)
    //             {
    //                 std::shared_ptr<AtlasPage> page = std::make_shared<AtlasPage>(m_PageSize, m_PageSize);
    //                 m_Pages.push_back(page);

    //                 if (!page->AddFontTexture(name, c.Width, c.Height, c.Data, fontIndex, c.Code, c.BearingX, c.BearingY, c.Advance))
    //                 {
    //                     throw std::string("impossible 2");
    //                 }
    //             }
    //         }
    //     }
    // }

    // bool ContentFile::RemoveFont(const std::string& name)
    // {
    //     auto it = m_FontEntries.begin();
    //     bool found = false;

    //     while (!found && it != m_FontEntries.end())
    //     {
    //         if ((*it).Name == name)
    //         {
    //             m_FontEntries.erase(it);
    //             found = true;
    //         }

    //         ++it;
    //     }

    //     if (!found)
    //     {
    //         return false;
    //     }

    //     for (auto& page : m_Pages)
    //     {
    //         auto it = page->m_Textures.begin();

    //         while (it != page->m_Textures.end())
    //         {
    //             if ((*it)->Name == name)
    //             {
    //                 page->m_Textures.erase(it);
    //             }
    //             else
    //             {
    //                 ++it;
    //             }
    //         }
    //     }

    //     return true;
    // }

    // void ContentFile::SaveToFile(const std::string& path)
    // {
    //     m_SavePath = path;
    //     Save();
    // }

    // void ContentFile::Save()
    // {
    //     if (!m_SavePath)
    //     {
    //         throw std::string("impossible - no save path");
    //     }

    //     unsigned char buf[255];
    //     std::basic_ofstream<unsigned char> of(m_SavePath.value(), std::ios::binary);

    //     if (of.bad())
    //     {
    //         throw std::string("of is bad");
    //     }

    //     if (!of.is_open())
    //     {
    //         throw std::string("of is not open");
    //     }

    //     of.write(m_FileHeader, 8);
    //     WriteUInt32(m_FileVersion, buf);
    //     of.write(buf, 4);
    //     WriteUInt16(m_Pages.size(), buf);
    //     of.write(buf, 2);
    //     std::uint32_t textureCount = 0;

    //     for (auto& page : m_Pages)
    //     {
    //         textureCount += page->GetPageTextures().size();
    //     }

    //     WriteUInt32(textureCount, buf);
    //     of.write(buf, 4);

    //     WriteUInt32(m_FontEntries.size(), buf);
    //     of.write(buf, 4);

    //     for (auto& fontEntry : m_FontEntries)
    //     {
    //         WriteUInt16(fontEntry.Ascender, buf);
    //         WriteUInt16(fontEntry.Descender, buf + 2);
    //         WriteUInt16(fontEntry.LineSpacing, buf + 4);
    //         buf[6] = fontEntry.Name.size();
    //         fontEntry.Name.c_str();
    //         of.write(buf, 7);
    //         of.write(reinterpret_cast<unsigned char*>(fontEntry.Name.data()), fontEntry.Name.size());
    //     }
        
    //     for (std::uint16_t pageNum = 0; pageNum < m_Pages.size(); ++pageNum)
    //     {
    //         for (auto& texture : m_Pages[pageNum]->GetPageTextures())
    //         {
    //             if (!texture->IsFont)
    //             {
    //                 buf[0] = 0; // this is a texture
    //                 of.write(buf, 1);

    //                 WriteUInt16(pageNum, buf);
    //                 WriteUInt16(texture->X, buf + 2);
    //                 WriteUInt16(texture->Y, buf + 4);
    //                 WriteUInt16(texture->Width, buf + 6);
    //                 WriteUInt16(texture->Height, buf + 8);
    //                 of.write(buf, 10);

    //                 buf[0] = static_cast<std::uint8_t>(texture->Name.length());
    //                 of.write(buf, 1);
    //                 of.write(reinterpret_cast<unsigned char*>(texture->Name.data()), texture->Name.length());
                    
    //                 std::size_t dataSize = texture->Width * texture->Height * 4;

    //                 if (dataSize > 1024)
    //                 {
    //                     std::uint8_t compressQuality = BROTLI_MAX_QUALITY / 2;
    //                     buf[0] = compressQuality;
    //                     of.write(buf, 1);
    //                     std::unique_ptr<unsigned char[]> compressOutput = std::make_unique<unsigned char[]>(dataSize);
    //                     std::size_t compressSize = dataSize;

    //                     if (BrotliEncoderCompress(compressQuality, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE,
    //                     dataSize, reinterpret_cast<uint8_t*>(texture->Data.get()), &compressSize, reinterpret_cast<uint8_t*>(compressOutput.get())) == BROTLI_FALSE)
    //                     {
    //                         throw std::string("compress failure");
    //                     }

    //                     WriteUInt32(static_cast<std::uint32_t>(compressSize), buf);
    //                     of.write(buf, 4);
    //                     of.write(compressOutput.get(), compressSize);
    //                 }
    //                 else
    //                 {
    //                     buf[0] = 0;
    //                     of.write(buf, 1);
    //                     of.write(texture->Data.get(), dataSize);
    //                 }
    //             }
    //             else
    //             {
    //                 buf[0] = 1; // this is a font character
    //                 of.write(buf, 1);

    //                 WriteUInt16(pageNum, buf);
    //                 WriteUInt16(texture->X, buf + 2);
    //                 WriteUInt16(texture->Y, buf + 4);
    //                 WriteUInt16(texture->Width, buf + 6);
    //                 WriteUInt16(texture->Height, buf + 8);
    //                 buf[10] = texture->Code;
    //                 WriteUInt32(texture->BearingX, buf + 11);
    //                 WriteUInt32(texture->BearingY, buf + 15);
    //                 WriteUInt32(texture->Advance, buf + 19);
    //                 of.write(buf, 23);

    //                 buf[0] = static_cast<std::uint8_t>(texture->Name.length());
    //                 of.write(reinterpret_cast<const char*>(buf), 1);
    //                 of.write(texture->Name.c_str(), texture->Name.length());

    //                 if (texture->Width != 0 && texture->Height != 0)
    //                 {
    //                     std::size_t dataSize = texture->Width * texture->Height * 4;

    //                     if (dataSize > 1024)
    //                     {
    //                         std::uint8_t compressQuality = BROTLI_MAX_QUALITY / 2;
    //                         buf[0] = compressQuality;
    //                         of.write(buf, 1);
    //                         std::unique_ptr<char[]> compressOutput = std::make_unique<char[]>(dataSize);
    //                         std::size_t compressSize = dataSize;

    //                         if (BrotliEncoderCompress(compressQuality, BROTLI_DEFAULT_WINDOW, BROTLI_DEFAULT_MODE,
    //                         dataSize, reinterpret_cast<uint8_t*>(texture->Data.get()), &compressSize, reinterpret_cast<uint8_t*>(compressOutput.get())) == BROTLI_FALSE)
    //                         {
    //                             throw std::string("compress failure");
    //                         }

    //                         WriteUInt32(static_cast<std::uint32_t>(compressSize), buf);
    //                         of.write(buf, 4);
    //                         of.write(compressOutput.get(), compressSize);
    //                     }
    //                     else
    //                     {
    //                         buf[0] = 0;
    //                         of.write(buf, 1);
    //                         of.write(texture->Data.get(), dataSize);
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }

    // void ContentFile::Load(const std::string& path)
    // {
    //     std::basic_ifstream<unsigned char> file(path, std::ios::binary);

    //     if (file.fail())
    //     {
    //         throw std::runtime_error("couldn't open file");
    //     }

    //     unsigned char buffer[8];
    //     file.read(buffer, sizeof(m_FileHeader));

    //     if (file.fail() || std::memcmp(buffer, m_FileHeader, sizeof(m_FileHeader)) != 0)
    //     {
    //         throw std::runtime_error("not a content file");
    //     }

    //     file.read(buffer, 4);

    //     if (file.fail())
    //     {
    //         throw std::runtime_error("can't read this content file");
    //     }

    //     std::uint32_t fileVersion;
    //     ReadUInt32(&fileVersion, buffer);

    //     if (fileVersion != m_FileVersion)
    //     {
    //         throw std::runtime_error("can't read this content file");
    //     }

    //     file.read(buffer, 2);
    // }

    // static void PECT_CloseFile(std::FILE* fp) noexcept(true)
    // {
    //     std::fclose(fp);
    // }

    // std::shared_ptr<ContentFile> ContentFile::LoadFromFile(const std::string& path)
    // {
    //     std::unique_ptr<std::FILE, decltype(&PECT_CloseFile)> fp(std::fopen(path.c_str(), "rb"), &PECT_CloseFile);

    //     if (!fp) { throw std::string("could not open file"); }

    //     char buffer[255];

    //     if (std::fread(&buffer, 1, 22, fp.get()) != 22) { throw std::string("not a valid content file 1"); }

    //     if (std::memcmp(&buffer, &m_FileHeader, 8) != 0) { throw std::string("not a valid content file 2"); }

    //     std::uint32_t version = ReadUInt32(&buffer[8]);
    //     std::uint16_t atlasPages = ReadUInt16(&buffer[12]);
    //     std::uint32_t textureListSize = ReadUInt32(&buffer[14]);
    //     std::uint32_t fontListSize = ReadUInt32(&buffer[18]);

    //     std::shared_ptr<ContentFile> contentFile = std::make_shared<ContentFile>();

    //     for (std::uint16_t i = 0; i < atlasPages; ++i)
    //     {
    //         contentFile->m_Pages.push_back(std::make_shared<AtlasPage>(contentFile->m_PageSize, contentFile->m_PageSize));
    //     }

    //     std::uint16_t ascender, descender, lineSpacing;
    //     std::uint8_t nameLen;

    //     for (std::uint32_t i = 0; i < fontListSize; ++i)
    //     {
    //         if (std::fread(buffer, 1, 7, fp.get()) != 7) { throw std::string("not a valid content file 3"); }

    //         ascender = ReadUInt16(buffer);
    //         descender = ReadUInt16(buffer + 2);
    //         lineSpacing = ReadUInt16(buffer + 4);
    //         nameLen = buffer[6];

    //         if (std::fread(buffer, 1, nameLen, fp.get()) != nameLen) { throw std::string("not a valid content file 4"); }

    //         contentFile->m_FontEntries.emplace_back(std::string(buffer, nameLen), ascender, descender, lineSpacing);
    //     }

    //     std::uint16_t pageNum, x, y, w, h;
    //     std::size_t imageDataSize;
    //     std::uint8_t compressType;
    //     std::int32_t bearingX, bearingY, advance;
    //     std::uint8_t code;

    //     for (std::uint32_t i = 0; i < textureListSize; ++i)
    //     {
    //         if (std::fread(buffer, 1, 1, fp.get()) != 1) { throw std::string("not a valid content file 5"); }

    //         if (buffer[0] == 0)
    //         {
    //             if (std::fread(&buffer, 1, 11, fp.get()) != 11) { throw std::string("not a valid content file 6"); }

    //             pageNum = ReadUInt16(buffer);
    //             x = ReadUInt16(buffer + 2);
    //             y = ReadUInt16(buffer + 4);
    //             w = ReadUInt16(buffer + 6);
    //             h = ReadUInt16(buffer + 8);
    //             imageDataSize = w * h * 4;
    //             nameLen = buffer[10];

    //             if (std::fread(&buffer, 1, nameLen, fp.get()) != nameLen) { throw std::string("not a valid content file 7"); }

    //             std::string name(buffer, nameLen);

    //             if (std::fread(&buffer, 1, 1, fp.get()) != 1) { throw std::string("not a valid content file 8"); }

    //             compressType = buffer[0];

    //             std::shared_ptr<char[]> imageData = std::make_shared<char[]>(imageDataSize);

    //             if (compressType == 0)
    //             {
    //                 if (std::fread(imageData.get(), 1, imageDataSize, fp.get()) != imageDataSize) { throw std::string("not a valid content file 9"); }
    //             }
    //             else
    //             {
    //                 if (std::fread(&buffer, 1, 4, fp.get()) != 4) { throw std::string("not a valid content file 10"); }

    //                 std::uint32_t compressLen = ReadUInt32(buffer);
    //                 std::unique_ptr<char[]> compressedBuff = std::make_unique<char[]>(compressLen);

    //                 if (std::fread(compressedBuff.get(), 1, compressLen, fp.get()) != compressLen) { throw std::string("not a valid content file 11"); }

    //                 if (BrotliDecoderDecompress(compressLen, reinterpret_cast<uint8_t*>(compressedBuff.get()), &imageDataSize, reinterpret_cast<uint8_t*>(imageData.get())) != BROTLI_DECODER_RESULT_SUCCESS)
    //                 {
    //                     throw std::string("decompress error");
    //                 }
    //             }

    //             contentFile->m_Pages[pageNum]->m_Textures.push_back(std::make_shared<PageTexture>(name, x, y, w, h, imageData));
    //         }
    //         else if (buffer[0] == 1)
    //         {
    //             if (std::fread(&buffer, 1, 24, fp.get()) != 24) { throw std::string("not a valid content file 12"); }

    //             pageNum = ReadUInt16(buffer);
    //             x = ReadUInt16(buffer + 2);
    //             y = ReadUInt16(buffer + 4);
    //             w = ReadUInt16(buffer + 6);
    //             h = ReadUInt16(buffer + 8);
    //             imageDataSize = w * h * 4;
    //             code = buffer[10];
    //             bearingX = ReadUInt32(buffer + 11);
    //             bearingY = ReadUInt32(buffer + 15);
    //             advance = ReadUInt32(buffer + 19);
    //             nameLen = buffer[23];

    //             if (std::fread(&buffer, 1, nameLen, fp.get()) != nameLen) { throw std::string("not a valid content file 13"); }

    //             std::string name(reinterpret_cast<const char*>(&buffer), nameLen);
    //             std::shared_ptr<char[]> imageData;

    //             if (w != 0 && h != 0)
    //             {
    //                 if (std::fread(&buffer, 1, 1, fp.get()) != 1) { throw std::string("not a valid content file 14"); }

    //                 compressType = buffer[0];

    //                 imageData = std::make_shared<char[]>(imageDataSize);

    //                 if (compressType == 0)
    //                 {
    //                     if (std::fread(imageData.get(), 1, imageDataSize, fp.get()) != imageDataSize) { throw std::string("not a valid content file 15"); }
    //                 }
    //                 else
    //                 {
    //                     if (std::fread(&buffer, 1, 4, fp.get()) != 4) { throw std::string("not a valid content file 16"); }

    //                     std::uint32_t compressLen = ReadUInt32(buffer);
    //                     std::unique_ptr<std::uint8_t[]> compressedBuff = std::make_unique<std::uint8_t[]>(compressLen);

    //                     if (std::fread(compressedBuff.get(), 1, compressLen, fp.get()) != compressLen) { throw std::string("not a valid content file 17"); }

    //                     if (BrotliDecoderDecompress(compressLen, compressedBuff.get(), &imageDataSize, reinterpret_cast<uint8_t*>(imageData.get())) != BROTLI_DECODER_RESULT_SUCCESS)
    //                     {
    //                         throw std::string("decompress error");
    //                     }
    //                 }
    //             }

    //             contentFile->m_Pages[pageNum]->m_Textures.push_back(std::make_shared<PageTexture>(name, x, y, w, h, imageData, code, bearingX, bearingY, advance));
    //         }
    //         else
    //         {
    //             throw std::string("not a valid content file 18");
    //         }

            
    //     }

    //     return contentFile;
    // }
}