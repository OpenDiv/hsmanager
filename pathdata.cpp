#include "pathdata.h"

fs::path pathData::audioPath = "hs_manager\\sound";
pathData::pathData()
{
    locateTfPath();
    setAudioPath("hs_manager\\sound");
}



bool pathData::addNewFilesIntoSoundFolder(QStringList newFileList) const
{
    bool allFilesCopied = true;

    for (const auto& it : newFileList)
    {
        try
        {
            fs::copy(it.toStdString(), getAudioPath().string()+"\\"+fs::path(it.toStdString()).filename().string());

        }
        catch (const fs::filesystem_error& e)
        {
            if (e.code() == std::errc::file_exists)
            {
                std::string selectedFileNameStr = it.toStdString().substr(0, it.toStdString().find_last_of("."));
                int counter = 1; // getting file name without extension, then adding number until error avoided
                // then copying selected file to audiofolder//*filename*
                fs::path selectedFilePath;
                do {
                    selectedFileNameStr = it.toStdString().substr(0, it.toStdString().find_last_of(".")) + " (" + std::to_string(counter) + ")" + fs::path(it.toStdString()).extension().string();
                    counter++;
                } while (fs::exists(getAudioPath().string()+"\\"+fs::path(selectedFileNameStr).filename().string()));
                    fs::copy(it.toStdString(), getAudioPath().string()+"\\"+fs::path(selectedFileNameStr).filename().string());
            }
        }
    }
    return allFilesCopied;
}


void pathData::openSoundFolder() const
{
        ShellExecuteW(NULL, L"open", getAudioPath().wstring().c_str(), NULL, NULL, SW_SHOWDEFAULT);
}

fs::path pathData::getTfPath() const
{
    return tfPath;
}

QString pathData::qGetTfPath() const
{
    return pathToQString(tfPath);
}

bool pathData::setTfPath(const fs::path& newPath)
{
    tfPath = newPath;
    return true;
}

bool pathData::locateTfPath()
{
    HKEY hKey;
    char steamPath[MAX_PATH];
    DWORD bufferSize = sizeof(steamPath);
    LONG result;

    result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Wow6432Node\\Valve\\Steam", 0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return false;
    }

    result = RegQueryValueExA(hKey, "InstallPath", NULL, NULL, (LPBYTE)steamPath, &bufferSize);

    if(result != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return false;
    }

    RegCloseKey(hKey);
    tfPath = std::string(steamPath) + "\\steamapps\\common\\Team Fortress 2";
    return true;
}

fs::path pathData::getAudioPath()
{
    return audioPath;
}

QString pathData::qGetAudioPath()
{
    return pathToQString(audioPath);
}

bool pathData::setAudioPath(const fs::path& newPath)
{
    audioPath = newPath;
    return true;
}

QString pathData::pathToQString(const fs::path& path)
{
    return QString::fromStdWString(path.wstring());
}

fs::path pathData::qStringToPath(const QString& qPath)
{
    return fs::path(qPath.toStdWString());
}



pathData::~pathData()
{
    // Деструктор, можно оставить пустым или добавить необходимые действия
}
