#include <bravo/bravo.hpp>

using namespace brv;

void cmd::build(const CmdContext *cctx) {
    build::compile(cctx);
    build::link(cctx);
}
