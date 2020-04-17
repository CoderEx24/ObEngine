#include <System/Path.hpp>
#include <Utils/StringUtils.hpp>

namespace obe::System
{
    std::vector<MountablePath> Path::MountedPaths = std::vector<MountablePath>();

    Path::Path()
    {
        this->m_path = "";
    }

    Path::Path(const Path& path)
    {
        this->m_path = path.toString();
    }

    Path::Path(const std::string& path)
    {
        m_path = std::move(path);
    }

    Path Path::add(const std::string& path) const
    {
        return Path(
            m_path + ((!m_path.empty() && m_path.back() != '/') ? "/" : "") + path);
    }

    std::string Path::last() const
    {
        std::vector<std::string> splitPath = Utils::String::split(m_path, "/");
        return splitPath.back();
    }

    Path Path::getPath(const unsigned int index)
    {
        if (index < MountedPaths.size())
            return Path(MountedPaths[index].basePath).add(m_path);
        throw aube::ErrorHandler::Raise("ObEngine.System.Path.UnknownPathAtIndex",
            { { "index", std::to_string(index) }, { "path", m_path } });
    }

    std::string Path::find(PathType pathType) const
    {
        for (MountablePath& mountedPath : MountedPaths)
        {
            if ((pathType == PathType::All || pathType == PathType::File)
                && Utils::File::fileExists(mountedPath.basePath
                    + ((!mountedPath.basePath.empty()) ? "/" : "") + this->m_path))
            {
                return mountedPath.basePath + ((!mountedPath.basePath.empty()) ? "/" : "")
                    + this->m_path;
            }
            else if ((pathType == PathType::All || pathType == PathType::Directory)
                && Utils::File::directoryExists(mountedPath.basePath
                    + ((!mountedPath.basePath.empty()) ? "/" : "") + this->m_path))
            {
                return mountedPath.basePath + ((!mountedPath.basePath.empty()) ? "/" : "")
                    + this->m_path;
            }
        }
        return "";
    }

    std::string Path::toString() const
    {
        return m_path;
    }

    void Path::Mount(MountablePath path)
    {
        MountedPaths.push_back(path);
        orderMountedPaths();
    }

    std::vector<MountablePath>& Path::Paths()
    {
        return MountedPaths;
    }

    std::vector<std::string> Path::StringPaths()
    {
        std::vector<std::string> mountedPaths;
        mountedPaths.reserve(MountedPaths.size());
        for (auto& mountedPath : MountedPaths)
        {
            mountedPaths.push_back(mountedPath.basePath);
        }
        return mountedPaths;
    }

    void Path::orderMountedPaths()
    {
        std::sort(MountedPaths.begin(), MountedPaths.end(),
            [](const MountablePath& first, const MountablePath& second) {
                return first.priority > second.priority;
            });
    }
} // namespace obe::System

obe::System::Path operator"" _path(const char* str, std::size_t len)
{
    return obe::System::Path(std::string(str, len));
}

std::string operator""_fs(const char* str, std::size_t len)
{
    return obe::System::Path(std::string(str, len)).find();
}
