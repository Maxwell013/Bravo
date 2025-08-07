#include <bravo/bravo.hpp>

using namespace brv;

void cmd::help(const CmdContext *cctx) {
    BRV_CONDITIONAL(cctx->verbose, "Verbose logging enabled!");
    BRV_TRACE(
        "\n\n    Bravo v",
        BRV_VERSION_MAJOR,
        ".",
        BRV_VERSION_MINOR,
        ".",
        BRV_VERSION_PATCH,
        "\n    A fast and lightweight C++ build system.\n\
        \n\
        Usage:\n\
            bravo <command> [option]\n\
        \n\
        Commands:\n\
            help                                Show this message\n\
            build                               Build the current project\n\
        \n\
        Options:\n\
            --verbose                           Enable verbose logging\n\
            --deps                              Force build all dependecies\n\
        \n\
        Example:\n\
            bravo help\n\
            bravo build\n\
            bravo build --verbose --deps\n\
        \n"
    );
}
