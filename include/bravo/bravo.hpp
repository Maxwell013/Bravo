#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <filesystem>
#include <atomic>
#include <set>

#include <juliett/juliett.hpp>
#include <lima/lima.hpp>

// META DEFINES
#define BRV_VERSION_MAJOR 0
#define BRV_VERSION_MINOR 1
#define BRV_VERSION_PATCH 2

// CLI DEFINES
// ---
// Command strings
#define BRV_CMD_HELP_STR            "help"
#define BRV_CMD_BUILD_STR           "build"
// Command constants
#define BRV_CMD_HELP_SKIP_CONFIG    true
#define BRV_CMD_BUILD_SKIP_CONFIG   false
// Option strings
#define BRV_OPT_VERBOSE_STR         "--verbose"
#define BRV_OPT_DEPS_STR            "--deps"
// Option ids
#define BRV_OPT_VERBOSE_ID          0
#define BRV_OPT_DEPS_ID             1

// INTERNAL DEFINES
// ---
// File names
#define BRV_FILE_NAME_CONFIG        "bravo.json"
// File extentions
#define BRV_FILE_EXT_CPP            ".cpp"
#define BRV_FILE_EXT_OBJ            ".o"
#define BRV_FILE_EXT_ARCHIVE        ".a"
#define BRV_FILE_EXT_EXE            ""
// Directory names
#define BRV_DIR_SRC                 "src"
#define BRV_DIR_OBJ                 "obj"
#define BRV_DIR_BIN                 "bin"
#define BRV_DIR_INCLUDE             "include"

// PARSING DEFINES
// ---
// Keys
#define BRV_KEY_PROJECT_NAME        "project_name"
#define BRV_KEY_PROJECT_TYPE        "project_type"
#define BRV_KEY_ENTRY               "entry"
#define BRV_KEY_DEPS                "deps"
// Literals
#define BRV_PROJECT_TYPE_EXEC       "exec"
#define BRV_PROJECT_TYPE_STATIC     "static"
// Validation
#define BRV_VALIDATION_PROJECT_NAME "Name validation"
#define BRV_VALIDATION_PROJECT_TYPE "Type validation"
#define BRV_VALIDATION_ENTRY        "Entry validation"
#define BRV_VALIDATION_DEPS         "Deps validation"

// LOGGING DEFINES
// ---
#define BRV_TRACE(...) brv::LOGGER.log(lm::LogType::Trace, __VA_ARGS__)
#define BRV_DEBUG(...) brv::LOGGER.log(lm::LogType::Debug, __VA_ARGS__)
#define BRV_INFO(...) brv::LOGGER.log(lm::LogType::Info, __VA_ARGS__)
#define BRV_WARNING(...) brv::LOGGER.log(lm::LogType::Warning, __VA_ARGS__)
#define BRV_ERROR(...) brv::LOGGER.log(lm::LogType::Error, __VA_ARGS__)
#define BRV_FATAL(...) brv::LOGGER.log(lm::LogType::Fatal, __VA_ARGS__)
#define BRV_CONDITIONAL(condition, ...) brv::LOGGER.log(condition, lm::LogType::Info, __VA_ARGS__)
#define BRV_THROW(...) brv::LOGGER.assert(false, lm::LogType::Fatal, __VA_ARGS__)
#define BRV_ASSERT(assertion, ...) brv::LOGGER.assert(assertion, lm::LogType::Fatal, __VA_ARGS__)


namespace fs = std::filesystem;

namespace brv {
    // STRUCTS
    // ---
    // Parsed config file
    struct ConfigContext {
        fs::path root;
        std::string project_name;
        std::string project_type;
        std::optional<std::string> entry;
        std::vector<fs::path> deps;
    };
    // Parsed build data
    struct BuildContext {
        fs::path bin_dir;
        fs::path obj_dir;
        fs::path src_dir;
        fs::path include_dir;
        std::vector<fs::path> src_files;
        std::vector<fs::path> obj_files;
        std::vector<fs::path> include_dirs;
    };
    // Project config and build context
    struct ProjectContext {
        ConfigContext *config;
        BuildContext *build;
    };
    // Cmd struct for function pointer and command constants
    struct CmdContext;
    struct Cmd {
        std::function<void(const CmdContext *cctx)> call;
        bool skip_config;
    };
    // Validation struct for function pointer and name
    struct Validation {
        std::function<void(const ConfigContext *cfg)> call;
        std::string name;
    };
    // Cli data and global context
    struct CmdContext {
        Cmd cmd;
        bool verbose = false;
        bool rebuild = false;
        std::unordered_map<fs::path, std::vector<fs::path>> graph;
        std::vector<ProjectContext *> projects;
        std::vector<ProjectContext *> build_protocol;
    };


    // MAIN INTERFACE
    // ---
    // Process cli arguments -- uses cli namespace
    CmdContext *processCliArgs(int argc, char **argv);
    // Parse project config file -- uses config namespace
    ConfigContext *processConfigFile(const CmdContext *cctx, const fs::path &root);
    // Recursively parse dependecies -- uses deps namespace
    BuildContext *processDeps(const ConfigContext *cfg, CmdContext *cctx);
    // Resolve dependecy graph and build protocol -- uses graph namespace
    void resolveProtocol(CmdContext *cctx);
    // Release context ressources
    void releaseContext(CmdContext *cctx);

    // COMMANDS
    // ---
    namespace cmd {
        // Help : prints out the usage guidlines
        void help(const CmdContext *cctx);
        // Build : compiles and links the project
        void build(const CmdContext *cctx);
    } // namespace cmd

    // INTERNAL FUNCTIONS
    // ---
    namespace cli {
        std::string fuzzyMatch(const std::string &input, const std::vector<std::string> &valid);
        void setOpt(CmdContext *cctx, const std::string &opt);
    } // namespace cli

    namespace graph {
        void build(std::set<fs::path> &visited, std::set<fs::path> &visiting, CmdContext *cctx, ProjectContext *pctx);
    } // namespace graph
    namespace config {
        void load(ConfigContext *cfg);
        jltt::JValue *read(const fs::path &root);
        std::string getString(jltt::JValue *json, const jltt::JString &key);
        std::optional<std::string> getOptString(jltt::JValue *json, const jltt::JString &key);
        std::vector<fs::path> getPathVec(jltt::JValue *json, const jltt::JString &key);
        void validate(const ConfigContext *cfg);
        void validateProjectName(const ConfigContext *cfg);
        void validateProjectType(const ConfigContext *cfg);
        void validateEntry(const ConfigContext *cfg);
        void validateDeps(const ConfigContext *cfg);
    } // namespace config

    namespace deps {
        void scanProject(BuildContext *bctx, const ConfigContext *cfg);
        void scanDeps(const ConfigContext *cfg, CmdContext *cctx);
    } // namespace deps

    namespace build {
        void compile(const CmdContext *cctx);
        void link(const CmdContext *cctx);
        void worker(const int id, const bool verbose, const std::vector<std::string> &cmds, std::atomic<size_t> &index);
        std::string linkExec(const ProjectContext *pctx, const std::vector<fs::path> &archs);
        std::string linkStatic(const ProjectContext *pctx, std::vector<fs::path> &archs);
        bool rebuild(const fs::path &src, const fs::path &obj);
        int threadCount(const std::vector<std::string> &cmds);
    } // namespace build

    namespace file {
        bool isdir(const fs::path &dir);
        bool isfile(const fs::path &file);
    } // namespace file

    // CLI CONSTANTS
    // ---
    // Valid strings for fuzzy finding commands
    inline const std::vector<std::string> VALID_CMD_STRS = {
        BRV_CMD_HELP_STR,
        BRV_CMD_BUILD_STR,
    };
    // Command map for function calls
    inline std::unordered_map<std::string, Cmd> CMD_MAP = {
        {BRV_CMD_HELP_STR, {cmd::help, BRV_CMD_HELP_SKIP_CONFIG}},
        {BRV_CMD_BUILD_STR, {cmd::build, BRV_CMD_BUILD_SKIP_CONFIG}}
    };
    // Valid strings for fuzzy finding options
    inline std::unordered_map<std::string, std::vector<std::string>> VALID_OPT_STRS = {
        {BRV_CMD_HELP_STR, {BRV_OPT_VERBOSE_STR}},
        {BRV_CMD_BUILD_STR, {BRV_OPT_VERBOSE_STR, BRV_OPT_DEPS_STR}}
    };
    // Options map for fast lookup
    inline std::unordered_map<std::string, int> OPT_MAP {
        {BRV_OPT_VERBOSE_STR, BRV_OPT_VERBOSE_ID},
        {BRV_OPT_DEPS_STR, BRV_OPT_DEPS_ID}
    };

    // PARSING CONSTANTS
    // ---
    // Literal : project type
    inline const std::set<std::string> VALID_PROJECT_TYPES = {
        BRV_PROJECT_TYPE_EXEC,
        BRV_PROJECT_TYPE_STATIC,
    };
    // Validation map for function calls
    inline const std::vector<Validation> VALIDATION_MAP = {
        {config::validateProjectName, BRV_VALIDATION_PROJECT_NAME},
        {config::validateProjectType, BRV_VALIDATION_PROJECT_TYPE},
        {config::validateEntry, BRV_VALIDATION_ENTRY},
        {config::validateDeps, BRV_VALIDATION_DEPS},
    };

    // Logs
    inline const lm::Logger& LOGGER = *new lm::Logger("BRAVO", lm::flags::DEFAULTS);
} // namespace brv
