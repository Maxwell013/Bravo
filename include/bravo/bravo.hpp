#pragma once

#include <climits>
#include <cstddef>
#include <map>
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

#define BRV_VERSION_MAJOR               0
#define BRV_VERSION_MINOR               2
#define BRV_VERSION_PATCH               0

// CLI DEFINES

#define BRV_CMD_HELP_STR                "help"
#define BRV_CMD_BUILD_STR               "build"
#define BRV_CMD_RUN_STR                 "run"
#define BRV_CMD_CLEAN_STR               "clean"
#define BRV_CMD_INIT_STR                "init"
#define BRV_CMD_TEST_STR                "test"

#define BRV_CMD_HELP_USAGE              "Show this message"
#define BRV_CMD_BUILD_USAGE             "Compile and link the current project"
#define BRV_CMD_RUN_USAGE               "Compile, link and run the current project"
#define BRV_CMD_CLEAN_USAGE             "Remove binary and object file directories"
#define BRV_CMD_INIT_USAGE              "Create new project in current directory"
#define BRV_CMD_TEST_USAGE              "Compile, link and run tests"

#define BRV_CMD_HELP_SKIP_CONFIG        true
#define BRV_CMD_BUILD_SKIP_CONFIG       false
#define BRV_CMD_RUN_SKIP_CONFIG         false
#define BRV_CMD_CLEAN_SKIP_CONFIG       true
#define BRV_CMD_INIT_SKIP_CONFIG        true
#define BRV_CMD_TEST_SKIP_CONFIG        false

#define BRV_CMD_HELP_NON_OPT_ARGC_MAX   0
#define BRV_CMD_BUILD_NON_OPT_ARGC_MAX  0
#define BRV_CMD_RUN_NON_OPT_ARGC_MAX    0
#define BRV_CMD_CLEAN_NON_OPT_ARGC_MAX  0
#define BRV_CMD_INIT_NON_OPT_ARGC_MAX   1
#define BRV_CMD_TEST_NON_OPT_ARGC_MAX   USHRT_MAX

#define BRV_OPT_VERBOSE_STR_LONG        "verbose"
#define BRV_OPT_DEPS_STR_LONG           "deps"
#define BRV_OPT_NO_BUILD_STR_LONG       "no-build"

#define BRV_OPT_VERBOSE_STR_SHRT        'v'
#define BRV_OPT_DEPS_STR_SHRT           'd'
#define BRV_OPT_NO_BUILD_STR_SHRT       'n'

#define BRV_OPT_VERBOSE_USAGE           "Enable verbose logging"
#define BRV_OPT_DEPS_USAGE              "Force build all dependencies recursively"
#define BRV_OPT_NO_BUILD_USAGE          "Skip auto re-build"

#define BRV_OPT_VERBOSE_ID              0
#define BRV_OPT_DEPS_ID                 1
#define BRV_OPT_NO_BUILD_ID             2

// INTERNAL DEFINES

#define BRV_FILE_NAME_CONFIG            "bravo.json"
#define BRV_FILE_EXT_CPP                ".cpp"
#define BRV_FILE_EXT_OBJ                ".o"
#define BRV_FILE_EXT_ARCHIVE            ".a"
#define BRV_FILE_EXT_EXE                ""

#define BRV_DIR_SRC                     "src"
#define BRV_DIR_OBJ                     "obj"
#define BRV_DIR_BIN                     "bin"
#define BRV_DIR_INCLUDE                 "include"
#define BRV_DIR_TEST                    "tests"

// PARSING DEFINES

#define BRV_KEY_PROJECT_NAME            "project_name"
#define BRV_KEY_PROJECT_TYPE            "project_type"
#define BRV_KEY_ENTRY                   "entry"
#define BRV_KEY_DEPS                    "deps"
#define BRV_KEY_BUILD_NAME              "build_name"
#define BRV_KEY_RUN_ARGS                "run_args"

#define BRV_PROJECT_TYPE_EXEC           "exec"
#define BRV_PROJECT_TYPE_STATIC         "static"

#define BRV_VALIDATION_PROJECT_NAME     "Name validation"
#define BRV_VALIDATION_PROJECT_TYPE     "Type validation"
#define BRV_VALIDATION_ENTRY            "Entry validation"
#define BRV_VALIDATION_DEPS             "Deps validation"

// DEFAULT DEFINES

#define BRV_DEFAULT_PROJECT_NAME        "MyProject"
#define BRV_DEFAULT_PROJECT_TYPE        BRV_PROJECT_TYPE_EXEC
#define BRV_DEFAULT_ENTRY               "main.cpp"
#define BRV_DEFAULT_BUILD_NAME          "myproject"

// LOGGING DEFINES

#define BRV_TRACE(...) brv::LOGGER.log(lm::LogType::Trace, __VA_ARGS__)
#define BRV_DEBUG(...) brv::LOGGER.log(lm::LogType::Debug, __VA_ARGS__)
#define BRV_INFO(...) brv::LOGGER.log(lm::LogType::Info, __VA_ARGS__)
#define BRV_WARNING(...) brv::LOGGER.log(lm::LogType::Warning, __VA_ARGS__)
#define BRV_ERROR(...) brv::LOGGER.log(lm::LogType::Error, __VA_ARGS__)
#define BRV_FATAL(...) brv::LOGGER.log(lm::LogType::Fatal, __VA_ARGS__)

#define BRV_CONDITIONAL(condition, ...) brv::LOGGER.log(condition, lm::LogType::Info, __VA_ARGS__)
#define BRV_THROW(...) brv::LOGGER.assert(false, lm::LogType::Error, __VA_ARGS__)
#define BRV_ASSERT(assertion, ...) brv::LOGGER.assert(assertion, lm::LogType::Error, __VA_ARGS__)
#define BRV_UNUSED(unused) do { (void)unused; } while (false)


namespace fs = std::filesystem;

namespace brv {
    // TYPE ALIASES

    struct CmdContext;
    typedef std::function<void(const CmdContext *)> BravoCmd;
    typedef std::function<std::string(const std::vector<fs::path> &, std::vector<fs::path> &, const fs::path &)> LinkProcess;

    // STRUCTS

    // Config file tokens
    struct ConfigContext {
        fs::path root;

        std::string project_name;
        std::string project_type;
        std::string build_name;
        std::optional<std::string> entry;
        std::optional<std::string> run_args;
        std::vector<fs::path> deps;
    };
    // Build data
    struct BuildContext {
        fs::path end_dst;
        fs::path bin_dir;
        fs::path obj_dir;
        fs::path src_dir;
        fs::path test_dir;
        fs::path include_dir;
        std::vector<fs::path> src_files;
        std::vector<fs::path> obj_files;
        std::vector<fs::path> test_src_files;
        std::vector<fs::path> test_obj_files;
        std::vector<fs::path> test_exe_files;
        std::vector<fs::path> include_dirs;
    };
    // Project config and build context
    struct ProjectContext {
        ConfigContext *config;
        BuildContext *build;
    };
    // Cmd struct for function pointer and command constants
    struct Cmd {
        BravoCmd call;
        bool skip_config;
        unsigned int non_opt_argc_max;
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
        bool no_build = false;
        std::vector<std::string> non_opt_args;
        std::unordered_map<fs::path, std::vector<fs::path>> dep_graph;
        std::vector<ProjectContext *> projects;
        std::vector<ProjectContext *> build_protocol;
        ProjectContext *active_project;
    };

    // MAIN INTERFACE

    // Process cli arguments
    CmdContext *processCliArgs(int argc, char **argv);
    // Parse project config file
    ConfigContext *processConfigFile(const CmdContext *cctx, const fs::path &root);
    // Recursively parse dependecies
    BuildContext *processDeps(const ConfigContext *cfg, CmdContext *cctx);
    // Resolve dependecy graph and build protocol
    void resolveProtocol(CmdContext *cctx);
    // Execute the parsed command
    void executeCommand(CmdContext *cctx);
    // Release context ressources
    void releaseContext(CmdContext *cctx);

    // COMMANDS

    namespace cmd {
        // Prints out the usage guidlines
        void help(const CmdContext *cctx);
        // Compiles and links the project
        void build(const CmdContext *cctx);
        // Compiles, links and runs the final executable
        void run(const CmdContext *cctx);
        // Removes binary and object directories
        void clean(const CmdContext *cctx);
        // Creates source directory, include directory and config file
        void init(const CmdContext *cctx);
        // Compiles, links and runs test executables
        void test(const CmdContext *cctx);
    } // namespace cmd

    // INTERNAL FUNCTIONS

    namespace cli {
        std::string fuzzyMatch(const std::string &input, const std::vector<std::string> &valid);
        void parseArg(const std::string &input, const std::string &cmd, CmdContext *cctx);
        void setOpt(CmdContext *cctx, const std::string &cmd, unsigned int opt_id);
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
        void validate(const ConfigContext *cfg, const CmdContext *cctx);
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
        void worker(unsigned int id, bool verbose, const std::vector<std::string> &cmds, std::atomic<size_t> &index);
        std::string linkExec(const std::vector<fs::path> &objs, std::vector<fs::path> &archs, const fs::path &dst);
        std::string linkStatic(const std::vector<fs::path> &objs, std::vector<fs::path> &archs, const fs::path &dst);
        std::string makeCompileCommand(const std::string &common, const fs::path &src, const fs::path &dst);
        bool rebuild(const fs::path &src, const fs::path &obj);
        int threadCount(const std::vector<std::string> &cmds);
    } // namespace build

    namespace file {
        bool isdir(const fs::path &dir);
        bool isfile(const fs::path &file);
        void recurse(const fs::path &dir, std::vector<fs::path> &files, const std::string &target_ext);
        void swap(const std::vector<fs::path> &srcs, std::vector<fs::path> &dsts, const fs::path &src, const fs::path &dst, const std::string &ext);
    } // namespace file

    // CLI CONSTANTS

    inline const std::vector<std::string> CMD_VECTOR = {
        BRV_CMD_HELP_STR,
        BRV_CMD_BUILD_STR,
        BRV_CMD_RUN_STR,
        BRV_CMD_CLEAN_STR,
        BRV_CMD_INIT_STR,
        BRV_CMD_TEST_STR,
    };
    inline const std::unordered_map<std::string, std::string> CMD_USAGE_MAP = {
        {BRV_CMD_HELP_STR, BRV_CMD_HELP_USAGE},
        {BRV_CMD_BUILD_STR, BRV_CMD_BUILD_USAGE},
        {BRV_CMD_RUN_STR, BRV_CMD_RUN_USAGE},
        {BRV_CMD_CLEAN_STR, BRV_CMD_CLEAN_USAGE},
        {BRV_CMD_INIT_STR, BRV_CMD_INIT_USAGE},
        {BRV_CMD_TEST_STR, BRV_CMD_TEST_USAGE},
    };
    inline const std::unordered_map<std::string, Cmd> CMD_MAP = {
        {BRV_CMD_HELP_STR, {
            cmd::help,
            BRV_CMD_HELP_SKIP_CONFIG,
            BRV_CMD_HELP_NON_OPT_ARGC_MAX,
        }},
        {BRV_CMD_BUILD_STR, {
            cmd::build,
            BRV_CMD_BUILD_SKIP_CONFIG,
            BRV_CMD_BUILD_NON_OPT_ARGC_MAX,
        }},
        {BRV_CMD_RUN_STR, {
            cmd::run,
            BRV_CMD_RUN_SKIP_CONFIG,
            BRV_CMD_RUN_NON_OPT_ARGC_MAX,
        }},
        {BRV_CMD_CLEAN_STR, {
            cmd::clean,
            BRV_CMD_CLEAN_SKIP_CONFIG,
            BRV_CMD_CLEAN_NON_OPT_ARGC_MAX,
        }},
        {BRV_CMD_INIT_STR, {
            cmd::init,
            BRV_CMD_INIT_SKIP_CONFIG,
            BRV_CMD_INIT_NON_OPT_ARGC_MAX,
        }},
        {BRV_CMD_TEST_STR, {
            cmd::test,
            BRV_CMD_TEST_SKIP_CONFIG,
            BRV_CMD_TEST_NON_OPT_ARGC_MAX,
        }},
    };
    inline const std::unordered_map<std::string, std::set<unsigned int>> VALID_OPT_IDS = {
        {BRV_CMD_HELP_STR, {BRV_OPT_VERBOSE_ID}},
        {BRV_CMD_BUILD_STR, {BRV_OPT_VERBOSE_ID, BRV_OPT_DEPS_ID}},
        {BRV_CMD_RUN_STR, {BRV_OPT_VERBOSE_ID, BRV_OPT_DEPS_ID, BRV_OPT_NO_BUILD_ID}},
        {BRV_CMD_CLEAN_STR, {BRV_OPT_VERBOSE_ID}},
        {BRV_CMD_INIT_STR, {BRV_OPT_VERBOSE_ID}},
        {BRV_CMD_TEST_STR, {BRV_OPT_VERBOSE_ID, BRV_OPT_DEPS_ID, BRV_OPT_NO_BUILD_ID}},
    };
    inline const std::vector<std::string> OPT_LONG_VECTOR {
        BRV_OPT_VERBOSE_STR_LONG,
        BRV_OPT_DEPS_STR_LONG,
        BRV_OPT_NO_BUILD_STR_LONG,
    };
    inline const std::set<char> OPT_SHORT_SET {
        BRV_OPT_VERBOSE_STR_SHRT,
        BRV_OPT_DEPS_STR_SHRT,
        BRV_OPT_NO_BUILD_STR_SHRT,
    };
    inline const std::unordered_map<std::string, unsigned int> OPT_LONG_MAP {
        {BRV_OPT_VERBOSE_STR_LONG, BRV_OPT_VERBOSE_ID},
        {BRV_OPT_DEPS_STR_LONG, BRV_OPT_DEPS_ID},
        {BRV_OPT_NO_BUILD_STR_LONG, BRV_OPT_NO_BUILD_ID},
    };
    inline const std::unordered_map<char, unsigned int> OPT_SHORT_MAP {
        {BRV_OPT_VERBOSE_STR_SHRT, BRV_OPT_VERBOSE_ID},
        {BRV_OPT_DEPS_STR_SHRT, BRV_OPT_DEPS_ID},
        {BRV_OPT_NO_BUILD_STR_SHRT, BRV_OPT_NO_BUILD_ID},
    };

    inline const std::map<std::string, std::pair<char, std::string>> OPT_USAGE_MAP = {
        {BRV_OPT_VERBOSE_STR_LONG, {
            BRV_OPT_VERBOSE_STR_SHRT,
            BRV_OPT_VERBOSE_USAGE
        }},
        {BRV_OPT_DEPS_STR_LONG, {
            BRV_OPT_DEPS_STR_SHRT,
            BRV_OPT_DEPS_USAGE
        }},
        {BRV_OPT_NO_BUILD_STR_LONG, {
            BRV_OPT_NO_BUILD_STR_SHRT,
            BRV_OPT_NO_BUILD_USAGE
        }},
    };

    // PARSING CONSTANTS

    inline const std::set<std::string> VALID_PROJECT_TYPES = {
        BRV_PROJECT_TYPE_EXEC,
        BRV_PROJECT_TYPE_STATIC,
    };
    inline const std::vector<Validation> VALIDATION_MAP = {
        {config::validateProjectName, BRV_VALIDATION_PROJECT_NAME},
        {config::validateProjectType, BRV_VALIDATION_PROJECT_TYPE},
        {config::validateEntry, BRV_VALIDATION_ENTRY},
        {config::validateDeps, BRV_VALIDATION_DEPS},
    };

    // BUILDING CONSTANTS

    inline const std::unordered_map<std::string, LinkProcess> LINK_PROCESS_MAP = {
        {BRV_PROJECT_TYPE_EXEC, build::linkExec},
        {BRV_PROJECT_TYPE_STATIC, build::linkStatic},
    };

    // LOGGING CONSTANTS

    inline const lm::Logger& LOGGER = *new lm::Logger("BRAVO", lm::flags::DEFAULTS);
} // namespace brv
