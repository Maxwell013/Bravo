#include <bravo/bravo.hpp>
#include <filesystem>

using namespace brv;

void cmd::clean(const CmdContext *cctx) {
    fs::path root = fs::current_path();

    BRV_CONDITIONAL(cctx->verbose, "Removing binary directory!");
    fs::remove_all(root / BRV_DIR_BIN);
    BRV_CONDITIONAL(cctx->verbose, "Removing object directory!");
    fs::remove_all(root / BRV_DIR_OBJ);
}
