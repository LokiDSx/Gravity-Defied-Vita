#include "ModsManager.h"

#include <algorithm>
#include <fstream>
#include <iostream>

#ifdef WIN32
#include <libgen.h>
#include <cstring>
#else
#include <unistd.h>
#include <pwd.h>
#endif

std::filesystem::path ModsManager::dataDir;
std::filesystem::path ModsManager::modsDir;
std::filesystem::path ModsManager::activeModFile;
std::vector<ModsManager::Mod> ModsManager::mods;
int ModsManager::appliedIndex = 0;

std::filesystem::path ModsManager::platformDataDir(const char* progName)
{
#ifdef VITA_PORT
    return std::filesystem::path("ux0:data") / "GravityDefied";
#elif defined(WIN32)
    const char* base = dirname(strdup(progName));
    return std::filesystem::path(base);
#else
    const char* homeDir = getenv("HOME");
    if (!homeDir)
        homeDir = getpwuid(getuid())->pw_dir;

    if (!homeDir)
        throw std::system_error(errno, std::system_category(), "Error getting home directory");

    return std::filesystem::path(homeDir) / ".GravityDefied";
#endif
}

std::string ModsManager::sanitize(const std::string& s)
{
    std::string result;
    result.reserve(s.size());

    for (char c : s) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_') {
            result += c;
        } else {
            result += '_';
        }
    }

    return result.empty() ? "mod" : result;
}

void ModsManager::init(const char* progName)
{
    dataDir = platformDataDir(progName);
    modsDir = dataDir / "Mods";
    activeModFile = dataDir / "activeMod";

    std::error_code ec;
    std::filesystem::create_directories(modsDir, ec);

    std::filesystem::path hint = modsDir / "PUT TRACKS HERE.txt";
    if (!std::filesystem::exists(hint, ec)) {
        std::ofstream out(hint);
        out << "Put custom tracks into this folder.\n"
               "One mod per folder, each folder must contain levels.mrg.\n"
               "Example: Mods/MyPack/levels.mrg\n"
               "A levels.mrg placed directly in Mods also works.\n";
    }

    scan();
    loadActive();
}

void ModsManager::scan()
{
    mods.clear();
    mods.push_back({ "Original", "", {} });

    std::error_code ec;
    if (!std::filesystem::exists(modsDir, ec)) {
        return;
    }

    std::vector<Mod> found;
    for (const auto& entry : std::filesystem::directory_iterator(modsDir, ec)) {
        if (entry.is_directory()) {
            std::filesystem::path mrg = entry.path() / "levels.mrg";
            if (std::filesystem::exists(mrg, ec)) {
                std::string name = entry.path().filename().string();
                found.push_back({ name, "mod_" + sanitize(name) + "_", mrg });
            }
        } else if (entry.is_regular_file() && entry.path().extension() == ".mrg") {
            std::string name = entry.path().stem().string();
            found.push_back({ name, "mod_" + sanitize(name) + "_", entry.path() });
        }
    }

    std::sort(found.begin(), found.end(), [](const Mod& a, const Mod& b) { return a.name < b.name; });
    mods.insert(mods.end(), found.begin(), found.end());
}

void ModsManager::loadActive()
{
    appliedIndex = 0;
    std::ifstream in(activeModFile);
    if (!in.is_open()) {
        return;
    }

    std::string name;
    std::getline(in, name);
    for (std::size_t i = 0; i < mods.size(); ++i) {
        if (mods[i].name == name) {
            appliedIndex = static_cast<int>(i);
            return;
        }
    }
}

void ModsManager::saveActive()
{
    std::ofstream out(activeModFile, std::ios::trunc);
    if (out.is_open()) {
        out << mods[appliedIndex].name;
    }
}

const std::vector<ModsManager::Mod>& ModsManager::getMods()
{
    return mods;
}

int ModsManager::getAppliedModIndex()
{
    return appliedIndex;
}

std::filesystem::path ModsManager::getMrgPath()
{
    return mods[appliedIndex].mrgPath;
}

std::string ModsManager::getStorePrefix()
{
    return mods[appliedIndex].storePrefix;
}

bool ModsManager::setModByIndex(int index)
{
    if (index < 0 || index >= static_cast<int>(mods.size()) || index == appliedIndex) {
        return false;
    }

    appliedIndex = index;
    saveActive();
    return true;
}
