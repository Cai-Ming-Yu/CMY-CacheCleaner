#include <iostream>
#include <filesystem>

#include <yaml-cpp/yaml.h>
#include <CuStringMatcher.h>
#include <CuLogger.h>

using namespace std;
using namespace CU;
namespace fs = filesystem;

#define CLOGE(...) Logger::Error(__VA_ARGS__)
#define CLOGW(...) Logger::Warn(__VA_ARGS__)
#define CLOGI(...) Logger::Info(__VA_ARGS__)
#define CLOGD(...) Logger::Debug(__VA_ARGS__)
#define CLOGV(...) Logger::Verbose(__VA_ARGS__)

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Invalid usage" << endl;
        return -1;
    }

    string configFile = argv[1];

    string logPath = fs::canonical(fs::absolute(fs::path(argv[1])).parent_path()).string() + "/runtimeLog.txt";
    Logger::Create(Logger::LogLevel::INFO, logPath);
    CLOGI((string("Created log file: ") + logPath).c_str());

    try
    {
        YAML::Node config = YAML::LoadFile(configFile);
        CLOGI((string("Read config file: ") + configFile).c_str());

        bool cleanAppCache = config["cleanAppCache"].as<bool>();
        if (cleanAppCache)
        {
            CLOGI("config cleanAppCache: true");
        }
        else
        {
            CLOGI("config cleanAppCache: false");
        }

        string appMode = config["appMode"].as<string>();
        CLOGI((string("config appMode: ") + appMode).c_str());

        string time = config["time"].as<string>();
        CLOGI((string("config time: ") + time).c_str());

        bool cleanDotFile = config["cleanDotFile"].as<bool>();
        if (cleanDotFile)
        {
            CLOGI("config cleanDotFile: true");
        }
        else
        {
            CLOGI("config cleanDotFile: false");
        }

        bool cleanSdcard = config["cleanSdcard"].as<bool>();
        if (cleanSdcard)
        {
            CLOGI("config cleanSdcard: true");
        }
        else
        {
            CLOGI("config cleanSdcard: false");
        }

        CLOGI("config filenameWhitelist:");
        vector<string> filenameWhitelist;
        for (const auto &filename : config["filenameWhitelist"])
        {
            filenameWhitelist.push_back(filename.as<string>());
            CLOGI(filename.as<string>().c_str());
        }

        CLOGI("config filenameBlacklist:");
        vector<string> filenameBlacklist;
        for (const auto &filename : config["filenameBlacklist"])
        {
            filenameBlacklist.push_back(filename.as<string>());
            CLOGI(filename.as<string>().c_str());
        }

        CLOGI("config appWhitelist:");
        vector<string> appWhitelist;
        for (const auto &app : config["appWhitelist"])
        {
            appWhitelist.push_back(app.as<string>());
            CLOGI(app.as<string>().c_str());
        }

        CLOGI("config appBlacklist:");
        vector<string> appBlacklist;
        for (const auto &app : config["appBlacklist"])
        {
            appBlacklist.push_back(app.as<string>());
            CLOGI(app.as<string>().c_str());
        }

        CLOGI("config fileWhitelist:");
        vector<string> fileWhitelist;
        for (const auto &file : config["fileWhitelist"])
        {
            fileWhitelist.push_back(file.as<string>());
            CLOGI(file.as<string>().c_str());
        }

        CLOGI("config fileBlacklist:");
        vector<string> fileBlacklist;
        for (const auto &file : config["fileBlacklist"])
        {
            fileBlacklist.push_back(file.as<string>());
            CLOGI(file.as<string>().c_str());
        }

        CLOGI("config appFileBlacklist:");
        map<string, vector<string>> appFileBlacklist;
        const YAML::Node &appFileBlacklistNode = config["appFileBlacklist"];
        if (appFileBlacklistNode.IsDefined() && appFileBlacklistNode.IsMap())
        {
            for (const auto &app : appFileBlacklistNode)
            {
                string packageName = app.first.as<string>();
                CLOGI((packageName + string(":")).c_str());
                const YAML::Node &pathsNode = app.second;
                if (pathsNode.IsSequence())
                {
                    for (const auto &path : pathsNode)
                    {
                        appFileBlacklist[packageName].push_back(path.as<string>());
                        CLOGI(path.as<string>().c_str());
                    }
                }
                else
                {
                    appFileBlacklist[packageName].push_back(pathsNode.as<string>());
                    CLOGI(pathsNode.as<string>().c_str());
                }
            }
        }

        CLOGI("Read config finished");
    }
    catch (const YAML::Exception &e)
    {
        stringstream ss;
        ss << "Error while parsing YAML: " << e.what();
        CLOGE(ss.str().c_str());
        return -1;
    }

    return 0;
}