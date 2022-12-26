// core/fileutil.cpp*
#include "fileutil.h"
#include <cstdlib>
#include <climits>
#ifndef PBRT_IS_WINDOWS
#include <libgen.h>
#endif

namespace pbrt {

static std::string searchDirectory;

#ifdef PBRT_IS_WINDOWS
bool IsAbsolutePath(const std::string &filename) {
    if (filename.empty()) return false;
    return (filename[0] == '\\' || filename[0] == '/' ||
            filename.find(':') != std::string::npos);
}

std::string AbsolutePath(const std::string &filename) {
    char full[_MAX_PATH];
    if (_fullpath(full, filename.c_str(), _MAX_PATH))
        return std::string(full);
    else
        return filename;
}

std::string ResolveFilename(const std::string &filename) {
    if (searchDirectory.empty() || filename.empty())
        return filename;
    else if (IsAbsolutePath(filename))
        return filename;

    char searchDirectoryEnd = searchDirectory[searchDirectory.size() - 1];
    if (searchDirectoryEnd == '\\' || searchDirectoryEnd == '/')
        return searchDirectory + filename;
    else
        return searchDirectory + "\\" + filename;
}

std::string DirectoryContaining(const std::string &filename) {
    // This code isn't tested but I believe it should work. Might need to add
    // some const_casts to make it compile though.
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char ext[_MAX_EXT];

    errno_t err = _splitpath_s(filename.c_str(), drive, _MAX_DRIVE, dir,
                               _MAX_DIR, nullptr, 0, ext, _MAX_EXT);
    if (err == 0) {
        char fullDir[_MAX_PATH];
        err = _makepath_s(fullDir, _MAX_PATH, drive, dir, nullptr, nullptr);
        if (err == 0) return std::string(fullDir);
    }
    return filename;
}

#else

bool IsAbsolutePath(const std::string &filename) {
    return (filename.size() > 0) && filename[0] == '/';
}

std::string AbsolutePath(const std::string &filename) {
    char full[PATH_MAX];
    if (realpath(filename.c_str(), full))
        return std::string(full);
    else
        return filename;
}

std::string ResolveFilename(const std::string &filename) {
    if (searchDirectory.empty() || filename.empty())
        return filename;
    else if (IsAbsolutePath(filename))
        return filename;
    else if (searchDirectory[searchDirectory.size() - 1] == '/')
        return searchDirectory + filename;
    else
        return searchDirectory + "/" + filename;
}

std::string DirectoryContaining(const std::string &filename) {
    char *t = strdup(filename.c_str());
    std::string result = dirname(t);
    free(t);
    return result;
}

#endif

void SetSearchDirectory(const std::string &dirname) {
    searchDirectory = dirname;
}

}  // namespace pbrt
