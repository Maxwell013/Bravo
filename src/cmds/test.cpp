#include <bravo/bravo.hpp>

using namespace brv;

void cmd::test(const CmdContext *cctx) {
    cmd::build(cctx); // TODO : skip linking if possible

    const BuildContext *bctx = cctx->active_project->build;

    BRV_ASSERT(!bctx->test_exe_files.empty(), "No test files found!");

    const std::vector<fs::path> *tests = &bctx->test_exe_files;

    if (!cctx->non_opt_args.empty()) {
        std::vector<fs::path> matching{};
        bool found;
        for (const std::string &arg : cctx->non_opt_args) {
            found = false;
            for (const fs::path &test : *tests)
                if (test.stem().string() == arg) {
                    matching.emplace_back(test);
                    found = true;
                    break;
                }
            BRV_ASSERT(found, "Could not find matching test to argument : '", arg, "'!");
        }
        tests = &matching;
    }

    for (const fs::path &test : *tests) {
        int exit_code = std::system((test.string()).c_str());
        lm::LogType type = exit_code == 0 ? lm::LogType::Debug : lm::LogType::Warning;
        LOGGER.log(type, "Test ", test.filename(), " exited with code : ", exit_code);
    }
}
