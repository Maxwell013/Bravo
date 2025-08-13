#include <bravo/bravo.hpp>

using namespace brv;

void brv::executeCommand(CmdContext *cctx) {
    cctx->cmd.call(cctx);
}

void brv::releaseContext(CmdContext *cctx) {
    for (ProjectContext *pctx : cctx->projects) {
        delete pctx->build;
        delete pctx->config;
        delete pctx;
    }
    delete cctx;
}

bool file::isdir(const fs::path &dir) {
    return (fs::exists(dir) && fs::is_directory(dir));
}

bool file::isfile(const fs::path &file) {
    return (fs::exists(file) && fs::is_regular_file(file));
}
