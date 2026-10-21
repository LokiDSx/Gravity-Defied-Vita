#pragma once

#include <filesystem>
#include <string>
#include <vector>

class ModsManager {
public:
    struct Mod {
        std::string name;
        std::string storePrefix;
        std::filesystem::path mrgPath;
    };

    static void init(const char* progName);
    static const std::vector<Mod>& getMods();
    static int getAppliedModIndex();
    static std::filesystem::path getMrgPath();
    static std::string getStorePrefix();
    static bool setModByIndex(int index);

private:
    static std::filesystem::path dataDir;
    static std::filesystem::path modsDir;
    static std::filesystem::path activeModFile;
    static std::vector<Mod> mods;
    static int appliedIndex;

    static std::filesystem::path platformDataDir(const char* progName);
    static std::string sanitize(const std::string& s);
    static void scan();
    static void loadActive();
    static void saveActive();
};
