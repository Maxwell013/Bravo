#include <bravo/bravo.hpp>

int main(int argc, char** argv) {
    brv::CmdContext *cctx = new brv::CmdContext();
    brv::ProjectContext *pctx = new brv::ProjectContext();

    // Parse and validate command and options
    cctx = brv::processCliArgs(argc, argv);

    cctx->projects.emplace_back(pctx);

    // Load and validate the json config file
    pctx->config = brv::processConfigFile(cctx, fs::current_path());

    // Scan project and dependencies
    pctx->build = brv::processDeps(pctx->config, cctx);

    // Resolve dependency graph
    brv::resolveProtocol(cctx);

    // Execute the command
    brv::executeCommand(cctx);

    // delete context (cleanup)
    brv::releaseContext(cctx);

    return EXIT_SUCCESS;
}
