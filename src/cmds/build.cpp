#include <bravo/bravo.hpp>

using namespace brv;

void cmd::build(const CmdContext *cctx) {
    if (cctx->no_build) return;
    build::compile(cctx);
    build::link(cctx);
}
