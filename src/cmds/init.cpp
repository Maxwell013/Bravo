#include <bravo/bravo.hpp>
#include <filesystem>

using namespace brv;

void cmd::init(const CmdContext *cctx) {
    fs::path root = fs::current_path();

    std::string project_name = BRV_DEFAULT_PROJECT_NAME;
    std::string build_name = BRV_DEFAULT_BUILD_NAME;
    if (!cctx->non_opt_args.empty()) {
        project_name = cctx->non_opt_args.front();
        build_name = "";
        for (const char &ch : project_name)
            build_name += std::tolower(ch);
    }

    BRV_CONDITIONAL(cctx->verbose, "Creating source directory!");
    fs::create_directory(root / BRV_DIR_SRC);
    BRV_CONDITIONAL(cctx->verbose, "Creating include directory!");
    fs::create_directories(root / BRV_DIR_INCLUDE / build_name);
    BRV_CONDITIONAL(cctx->verbose, "Creating test directories!");
    fs::create_directories(root / BRV_DIR_TEST / BRV_DIR_SRC);
    fs::create_directories(root / BRV_DIR_TEST / BRV_DIR_OBJ);
    fs::create_directories(root / BRV_DIR_TEST / BRV_DIR_BIN);

    fs::path path = root / BRV_FILE_NAME_CONFIG;
    if (file::isfile(path))
        return;

    BRV_CONDITIONAL(cctx->verbose, "Creating config file!");
    std::ostringstream json; // TODO : juliett

    json << "{" << std::endl;
    json << "  \"" << BRV_KEY_PROJECT_NAME << "\": \"" << project_name << "\"," << std::endl;
    json << "  \"" << BRV_KEY_PROJECT_TYPE << "\": \"" << BRV_DEFAULT_PROJECT_TYPE << "\"," << std::endl;
    json << "  \"" << BRV_KEY_ENTRY << "\": \"" << BRV_DEFAULT_ENTRY << "\"," << std::endl;
    json << "  \"" << BRV_KEY_BUILD_NAME << "\": \"" << build_name << "\"" << std::endl;
    json << "}" << std::endl;


    std::ofstream file(path);
    if (!file.is_open())
        BRV_THROW("Failed to create config file!");

    file << json.str();
    file.close();
}
