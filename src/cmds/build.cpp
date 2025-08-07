#include <bravo/bravo.hpp>

using namespace brv;

void cmd::build(const CmdContext *cctx) {
    BRV_CONDITIONAL(cctx->verbose, "Verbose logging enabled!");
    BRV_CONDITIONAL(cctx->verbose && cctx->rebuild, "Dependecy rebuild enabled!");
    build::compile(cctx);
    build::link(cctx);
}
