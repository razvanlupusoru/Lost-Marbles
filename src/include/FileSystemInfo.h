/*!
	@file
	@author		Albert Semenov
	@date		09/2009
	@module
*/
#ifndef __FILE_SYSTEM_INFO_H__
#define __FILE_SYSTEM_INFO_H__

#include <MyGUI.h>

#include <windows.h>
#include <io.h>

#include <string>
#include <vector>

namespace common {
struct FileInfo {
    FileInfo(const std::wstring& _name, bool _folder) : name(_name), folder(_folder) { }
    std::wstring name;
    bool folder;
};
typedef std::vector<FileInfo> VectorFileInfo;

inline bool isAbsolutePath(const wchar_t* path)
{
    if (isalpha(path[0]) && path[1] == ':')
        return true;
    return false;
}

inline std::wstring concatenatePath(const std::wstring& _base, const std::wstring& _name)
{
    if (_base.empty() || isAbsolutePath(_name.c_str()))
        return _name;
    else
        return _base + L'\\' + _name;
}

inline bool isReservedDir (const wchar_t *_fn)
{
    // if "." /*or ".."*/
    return (_fn [0] == '.' && (_fn [1] == 0 /*|| (_fn [1] == '.' && _fn [2] == 0)*/));
}

inline void getSystemFileList(VectorFileInfo& _result, const std::wstring& _folder, const std::wstring& _mask)
{
    bool ms_IgnoreHidden = true;

    long lHandle, res;
    struct _wfinddata_t tagData;

    // pattern can contain a directory name, separate it from mask
    size_t pos1 = _mask.rfind ('/');
    size_t pos2 = _mask.rfind ('\\');
    if (pos1 == _mask.npos || ((pos2 != _mask.npos) && (pos1 < pos2)))
        pos1 = pos2;
    std::wstring directory;
    if (pos1 != _mask.npos)
        directory = _mask.substr (0, pos1 + 1);

    std::wstring full_mask = concatenatePath(_folder, _mask);

    lHandle = _wfindfirst(full_mask.c_str(), &tagData);
    res = 0;
    while (lHandle != -1 && res != -1) {
        if (( !ms_IgnoreHidden || (tagData.attrib & _A_HIDDEN) == 0 ) &&
                (!isReservedDir (tagData.name))) {
            _result.push_back(FileInfo(directory + tagData.name, (tagData.attrib & _A_SUBDIR) != 0));
        }
        res = _wfindnext( lHandle, &tagData );
    }
    // Close if we found any files
    if(lHandle != -1)
        _findclose(lHandle);
}

inline std::wstring getSystemCurrentFolder()
{
    wchar_t buff[MAX_PATH+1];
    ::GetCurrentDirectoryW(MAX_PATH, buff);
    return buff;
}

typedef std::vector<std::wstring> VectorWString;
inline void scanFolder(VectorWString& _result, const std::wstring& _folder, bool _recursive, const std::wstring& _mask, bool _fullpath)
{
    std::wstring folder = _folder;
    if (!folder.empty()) folder += L"/";

    VectorFileInfo result;
    getSystemFileList(result, folder, _mask);

    for (VectorFileInfo::const_iterator item=result.begin(); item!=result.end(); ++item) {
        if (item->folder) continue;

        if (_fullpath)
            _result.push_back(folder + item->name);
        else
            _result.push_back(item->name);
    }

    if (_recursive) {
        getSystemFileList(result, folder, L"*");

        for (VectorFileInfo::const_iterator item=result.begin(); item!=result.end(); ++item) {
            if (!item->folder
                    || item->name == L".."
                    || item->name == L".") continue;
            scanFolder(_result, folder + item->name, _recursive, _mask, _fullpath);
        }

    }
}

}

#endif // __FILE_SYSTEM_INFO_H__
