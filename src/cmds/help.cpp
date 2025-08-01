#include <bravo/bravo.hpp>

#include <iostream>

using namespace brv;

void cmd::help(const CmdContext *cctx) {
    std::cout << "Bravo " << cctx->cmd.skip_config << std::endl; // TODO : Lima ty shiiiiii
}
