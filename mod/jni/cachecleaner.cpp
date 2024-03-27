#include <chrono>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string_view>
#include <thread>
#include <unordered_map>

#include <CuLogger.h>
#include <CuStringMatcher.h>
#include <yaml-cpp/yaml.h>

#define eq ==

using namespace std;
using namespace literals;
using namespace CU;
namespace fs = filesystem;

template <typename T> bool all_eq(T a, T b) { return (a eq b); }

template <typename T, typename... Args> bool all_eq(T a, T b, Args... args) {
  return (a eq b) and all_eq(a, args...);
}

template <typename T> bool all_not_eq(T a, T b) { return (a not_eq b); }

template <typename T, typename... Args>
bool all_not_eq(T a, T b, Args... args) {
  return (a not_eq b) and all_not_eq(a, args...);
}

template <typename T> bool all_or_eq(T a, T b) { return (a eq b); }

template <typename T, typename... Args> bool all_or_eq(T a, T b, Args... args) {
  return (a eq b) or all_or_eq(a, args...);
}

template <typename T> bool all_or_not_eq(T a, T b) { return (a not_eq b); }

template <typename T, typename... Args>
bool all_or_not_eq(T a, T b, Args... args) {
  return (a not_eq b) or all_or_not_eq(a, args...);
}

#define CLOGE(...) Logger::Error(__VA_ARGS__)
#define CLOGW(...) Logger::Warn(__VA_ARGS__)
#define CLOGI(...) Logger::Info(__VA_ARGS__)
#define CLOGD(...) Logger::Debug(__VA_ARGS__)
#define CLOGV(...) Logger::Verbose(__VA_ARGS__)

string exec(string_view cmd) {
  if (FILE *fp = popen(cmd.data(), "r")) {
    ostringstream stm;
    char line[128];
    while (fgets(line, 128, fp)) {
      stm << line;
    }
    pclose(fp);
    return stm.str();
  }
  return ""s;
}

void rmDir(string_view dir) {
  if (fs::exists(dir) && fs::is_directory(dir)) {
    for (const auto &path : fs::directory_iterator(dir)) {
      fs::remove_all(path.path());
    }
  }
}

void cleanApp(string_view app, bool multiUser) {
  if (multiUser) {
    for (const auto &entry : fs::directory_iterator("/storage/emulated"sv)) {
      if (entry.is_directory()) {
        rmDir(entry.path().string() + "/"s + string(app) + "/cache"s);
      }
    }
    for (const auto &entry : fs::directory_iterator("/data/user"sv)) {
      if (entry.is_directory()) {
        rmDir(entry.path().string() + "/"s + string(app) + "/cache"s);
        rmDir(entry.path().string() + "/"s + string(app) + "/code_cache"s);
      }
    }
  } else {
    rmDir("/sdcard/Android/data/"s + string(app) + "/cache"s);
    rmDir("/data/data/"s + string(app) + "/cache"s);
    rmDir("/data/data/"s + string(app) + "/code_cache"s);
  }
  CLOGI(("Cleaned app cache: "s + string(app)).c_str());
}

signed main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "Invalid usage"sv << endl;
    return -1;
  }

  string configFile = argv[1];

  string logPath =
      fs::canonical(fs::absolute(fs::path(argv[1])).parent_path()).string() +
      "/runtimeLog.txt"s;
  Logger::Create(Logger::LogLevel::INFO, logPath);
  CLOGI(("Created log file: "s + logPath).c_str());

  bool cleanAppCache, multiUser, cleanDotFile, cleanSdcard;
  optional<string> appMode, time, appWhitelist, appBlacklist;
  optional<vector<string>> filenameWhitelist, filenameBlacklist, fileWhitelist,
      fileBlacklist;
  optional<map<string, vector<string>>> appFileBlacklist;

  map<char, int> timeUnitInSeconds = {
      {'s', 1}, {'m', 60}, {'h', 3600}, {'d', 86400}};

  unordered_map<string_view, int> appModes = {
      {"user"sv, 1}, {"system"sv, 2}, {"all"sv, 3}};

  for (;;) {
    cleanAppCache = multiUser = cleanDotFile = cleanSdcard = false;
    for (auto &opt : {&appMode, &time, &appWhitelist, &appBlacklist}) {
      if (*opt) {
        opt->reset();
      }
    }
    for (auto &opt : {&filenameWhitelist, &filenameBlacklist, &fileWhitelist,
                      &fileBlacklist}) {
      if (*opt) {
        opt->reset();
      }
    }
    for (auto &opt : {&appFileBlacklist}) {
      if (*opt) {
        opt->reset();
      }
    }
    *appMode = *time = *appWhitelist = *appBlacklist = "";
    *filenameWhitelist = *filenameBlacklist = *fileWhitelist =
        *fileBlacklist = {};
    *appFileBlacklist = {};

    try {
      YAML::Node config = YAML::LoadFile(configFile);
      CLOGI(("Read config file: "s + configFile).c_str());

      cleanAppCache = config["cleanAppCache"].as<bool>();
      if (cleanAppCache) {
        CLOGI("config cleanAppCache: true");
      } else {
        CLOGI("config cleanAppCache: false");
      }

      *appMode = config["appMode"].as<string>();
      CLOGI(("config appMode: "s + *appMode).c_str());

      cleanAppCache = config["multiUser"].as<bool>();
      if (cleanAppCache) {
        CLOGI("config multiUser: true");
      } else {
        CLOGI("config multiUser: false");
      }

      *time = config["time"].as<string>();
      CLOGI(("config time: "s + *time).c_str());

      cleanDotFile = config["cleanDotFile"].as<bool>();
      if (cleanDotFile) {
        CLOGI("config cleanDotFile: true");
      } else {
        CLOGI("config cleanDotFile: false");
      }

      cleanSdcard = config["cleanSdcard"].as<bool>();
      if (cleanSdcard) {
        CLOGI("config cleanSdcard: true");
      } else {
        CLOGI("config cleanSdcard: false");
      }

      CLOGI("config filenameWhitelist:");
      for (const auto &filename : config["filenameWhitelist"]) {
        (*filenameWhitelist).push_back(filename.as<string>());
        CLOGI(filename.as<string>().c_str());
      }

      CLOGI("config filenameBlacklist:");
      for (const auto &filename : config["filenameBlacklist"]) {
        (*filenameBlacklist).push_back(filename.as<string>());
        CLOGI(filename.as<string>().c_str());
      }

      CLOGI("config appWhitelist:");
      for (const auto &app : config["appWhitelist"]) {
        *appWhitelist += "アプリ"s + app.as<string>() + "アプリ"s;
        CLOGI(app.as<string>().c_str());
      }

      CLOGI("config appBlacklist:");
      for (const auto &app : config["appBlacklist"]) {
        *appBlacklist += "アプリ"s + app.as<string>() + "アプリ"s;
        CLOGI(app.as<string>().c_str());
      }

      CLOGI("config fileWhitelist:");
      for (const auto &file : config["fileWhitelist"]) {
        (*fileWhitelist).push_back(file.as<string>());
        CLOGI(file.as<string>().c_str());
      }

      CLOGI("config fileBlacklist:");
      for (const auto &file : config["fileBlacklist"]) {
        (*fileBlacklist).push_back(file.as<string>());
        CLOGI(file.as<string>().c_str());
      }

      CLOGI("config appFileBlacklist:");
      const YAML::Node &appFileBlacklistNode = config["appFileBlacklist"];
      if (appFileBlacklistNode.IsDefined() && appFileBlacklistNode.IsMap()) {
        for (const auto &app : appFileBlacklistNode) {
          string packageName = app.first.as<string>();
          CLOGI((packageName + ":"s).c_str());
          const YAML::Node &pathsNode = app.second;
          if (pathsNode.IsSequence()) {
            for (const auto &path : pathsNode) {
              (*appFileBlacklist)[packageName].push_back(path.as<string>());
              CLOGI(path.as<string>().c_str());
            }
          } else {
            (*appFileBlacklist)[packageName].push_back(pathsNode.as<string>());
            CLOGI(pathsNode.as<string>().c_str());
          }
        }
      }
      CLOGI("Read config finished");
    } catch (const YAML::Exception &e) {
      stringstream ss;
      ss << "Error while parsing YAML: "sv << e.what();
      CLOGE(ss.str().c_str());
      return -1;
    }

    try {

      if (cleanAppCache) {
        string apps;
        switch (appModes[*appMode]) {
        case 1:

          CLOGI("Run cleanAppCache in user mode");
          apps = exec("pm list packages -3 | sed 's/package://'"sv);
          break;

        case 2:

          CLOGI("Run cleanAppCache in system mode");
          apps = exec(
              "pm list packages -s | sed 's/package://' | grep -v '^android$'"sv);
          break;

        case 3:

          CLOGI("Run cleanAppCache in all mode");
          apps = exec(
              "pm list packages | sed 's/package://' | grep -v '^android$'"sv);
          break;

        default:
          CLOGW("Unknown appMode, cleanAppCache will not run");
          apps = ""s;
          break;
        }
        if (all_not_eq(apps, ""s, "\n"s)) {
          istringstream iss(apps);
          string packageName;
          while (getline(iss, packageName, '\n')) {
            StringMatcher matcher("*アプリ"s + packageName + "アプリ*"s);
            if (matcher.match(*appWhitelist)) {
              CLOGI(("Skip cleanup app: "s + packageName).c_str());
            } else if (matcher.match(*appBlacklist)) {
              CLOGI(("Force cleanup app: "s + packageName).c_str());
              cleanApp(packageName, multiUser);
            } else {
              if (all_or_eq(exec("pidof "s + packageName + " 2>/dev/null"s),
                            ""s, "\n"s)) {
                cleanApp(packageName, multiUser);
              }
            }
            this_thread::sleep_for(chrono::milliseconds(1));
          }
        }
      }

      CLOGI(("Work finished, rest "s + *time).c_str());
      this_thread::sleep_for(chrono::seconds(stoi(time->c_str()) *
                                             timeUnitInSeconds[time->back()]));
    } catch (const exception &e) {
      stringstream ss;
      ss << "Error while running: "sv << e.what();
      CLOGE(ss.str().c_str());
      return -1;
    }
  }

  return 0;
}