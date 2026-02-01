#ifndef PECT_FILEVERSION_H_
#define PECT_FILEVERSION_H_

#include <cstdint>

namespace PECT
{
    class FileVersion
    {
    private:
        std::uint16_t m_Major, m_Minor, m_Patch;
    public:
        FileVersion(std::uint16_t ma, std::uint16_t mi, std::uint16_t pa) noexcept :
        m_Major(ma), m_Minor(mi), m_Patch(pa) { }

        FileVersion(FileVersion&& other) noexcept :
        m_Major(other.m_Major), m_Minor(other.m_Minor), m_Patch(other.m_Patch) { }

        FileVersion(const FileVersion& other) noexcept :
        m_Major(other.m_Major), m_Minor(other.m_Minor), m_Patch(other.m_Patch) { }

        std::uint16_t GetMajor() const noexcept { return m_Major; }
        std::uint16_t GetMinor() const noexcept { return m_Minor; }
        std::uint16_t GetPatch() const noexcept { return m_Patch; }
    };
}

#endif