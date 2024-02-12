#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>

#include <yaml-cpp/yaml.h>
#include <CuStringMatcher.h>
#include <CuLogger.h>
#include <unordered_map>

using namespace std;
using namespace literals;
using namespace CU;
namespace fs = filesystem;

#define CLOGE(...) Logger::Error(__VA_ARGS__)
#define CLOGW(...) Logger::Warn(__VA_ARGS__)
#define CLOGI(...) Logger::Info(__VA_ARGS__)
#define CLOGD(...) Logger::Debug(__VA_ARGS__)
#define CLOGV(...) Logger::Verbose(__VA_ARGS__)

string exec(const char *cmd)
{
    if (FILE *fp = popen(cmd, "r"))
    {
        ostringstream stm;
        char line[128];
        while (fgets(line, 128, fp))
        {
            stm << line;
        }
        pclose(fp);
        return stm.str();
    }
    return "";
}

void rmDir(const string &dir)
{
    if (fs::exists(dir) && fs::is_directory(dir))
    {
        for (const auto &path : fs::directory_iterator(dir))
        {
            fs::remove_all(path.path());
        }
    }
}

void cleanApp(const string &app)
{
    rmDir("/sdcard/Android/data/"s + app + "/cache"s);
    rmDir("/data/data/"s + app + "/cache"s);
    rmDir("/data/data/"s + app + "/code_cache"s);
    CLOGI(("Cleaned app cache: "s + app).c_str());
}

signed main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Invalid usage" << endl;
        return -1;
    }

    string configFile = argv[1];

    string logPath = fs::canonical(fs::absolute(fs::path(argv[1])).parent_path()).string() + "/runtimeLog.txt";
    Logger::Create(Logger::LogLevel::INFO, logPath);
    CLOGI(("Created log file: "s + logPath).c_str());

    bool cleanAppCache, cleanDotFile, cleanSdcard;
    string appMode, time, appWhitelist, appBlacklist;
    vector<string> filenameWhitelist, filenameBlacklist, fileWhitelist, fileBlacklist;
    map<string, vector<string>> appFileBlacklist;

    map<char, int> timeUnitInSeconds = {
        {'s', 1},
        {'m', 60},
        {'h', 3600},
        {'d', 86400}};

    unordered_map<string, int> appModes = {
        {"user", 1},
        {"system", 2},
        {"all", 3}};

    for (;;)
    {
        cleanAppCache = cleanDotFile = cleanSdcard = false;
        appMode = time = appWhitelist = appBlacklist = "";
        filenameWhitelist = filenameBlacklist = fileWhitelist = fileBlacklist = {};
        appFileBlacklist = {};

        try
        {
            YAML::Node config = YAML::LoadFile(configFile);
            CLOGI(("Read config file: "s + configFile).c_str());

            cleanAppCache = config["cleanAppCache"].as<bool>();
            if (cleanAppCache)
            {
                CLOGI("config cleanAppCache: true");
            }
            else
            {
                CLOGI("config cleanAppCache: false");
            }

            appMode = config["appMode"].as<string>();
            CLOGI(("config appMode: "s + appMode).c_str());

            time = config["time"].as<string>();
            CLOGI(("config time: "s + time).c_str());

            cleanDotFile = config["cleanDotFile"].as<bool>();
            if (cleanDotFile)
            {
                CLOGI("config cleanDotFile: true");
            }
            else
            {
                CLOGI("config cleanDotFile: false");
            }

            cleanSdcard = config["cleanSdcard"].as<bool>();
            if (cleanSdcard)
            {
                CLOGI("config cleanSdcard: true");
            }
            else
            {
                CLOGI("config cleanSdcard: false");
            }

            CLOGI("config filenameWhitelist:");
            for (const auto &filename : config["filenameWhitelist"])
            {
                filenameWhitelist.push_back(filename.as<string>());
                CLOGI(filename.as<string>().c_str());
            }

            CLOGI("config filenameBlacklist:");
            for (const auto &filename : config["filenameBlacklist"])
            {
                filenameBlacklist.push_back(filename.as<string>());
                CLOGI(filename.as<string>().c_str());
            }

            CLOGI("config appWhitelist:");
            for (const auto &app : config["appWhitelist"])
            {
                appWhitelist += "アプリ"s + app.as<string>() + "アプリ"s;
                CLOGI(app.as<string>().c_str());
            }

            CLOGI("config appBlacklist:");
            for (const auto &app : config["appBlacklist"])
            {
                appBlacklist += "アプリ"s + app.as<string>() + "アプリ"s;
                CLOGI(app.as<string>().c_str());
            }

            CLOGI("config fileWhitelist:");
            for (const auto &file : config["fileWhitelist"])
            {
                fileWhitelist.push_back(file.as<string>());
                CLOGI(file.as<string>().c_str());
            }

            CLOGI("config fileBlacklist:");
            for (const auto &file : config["fileBlacklist"])
            {
                fileBlacklist.push_back(file.as<string>());
                CLOGI(file.as<string>().c_str());
            }

            CLOGI("config appFileBlacklist:");
            const YAML::Node &appFileBlacklistNode = config["appFileBlacklist"];
            if (appFileBlacklistNode.IsDefined() && appFileBlacklistNode.IsMap())
            {
                for (const auto &app : appFileBlacklistNode)
                {
                    string packageName = app.first.as<string>();
                    CLOGI((packageName + ":"s).c_str());
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

        if (cleanAppCache)
        {
            string apps;
            switch (appModes[appMode])
            {
            case 1:

                CLOGI("Run cleanAppCache in user mode");
                apps = exec("pm list packages -3 | sed 's/package://'");
                break;

            case 2:

                CLOGI("Run cleanAppCache in system mode");
                apps = exec("pm list packages -s | sed 's/package://' | grep -v '^android$'");
                break;

            case 3:

                CLOGI("Run cleanAppCache in all mode");
                apps = exec("pm list packages | sed 's/package://' | grep -v '^android$'");
                break;

            default:
                CLOGW("Unknown appMode, cleanAppCache will not run");
                apps = "";
                break;
            }
            if (apps != "" && apps != "\n")
            {
                istringstream iss(apps);
                string packageName;
                while (getline(iss, packageName, '\n'))
                {
                    StringMatcher matcher("*アプリ"s + packageName + "アプリ*"s);
                    if (matcher.match(appWhitelist))
                    {
                        CLOGI(("Skip cleanup app: "s + packageName).c_str());
                    }
                    else if (matcher.match(appBlacklist))
                    {
                        CLOGI(("Force cleanup app: "s + packageName).c_str());
                        cleanApp(packageName);
                    }
                    else
                    {
                        if (exec(("pidof"s + packageName).c_str()) == "")
                        {
                            cleanApp(packageName);
                        }
                    }
                    this_thread::sleep_for(chrono::milliseconds(1));
                }
            }
        }

        CLOGI(("Work finished, rest "s + time).c_str());
        this_thread::sleep_for(chrono::seconds(stoi(time.c_str()) * timeUnitInSeconds[time.back()]));
    }

    return 0;
}