#include <bravo/bravo.hpp>

#include <iomanip>
#include <ios>
#include <ostream>
#include <sstream>
#include <utility>

using namespace brv;

void cmd::help(const CmdContext *cctx) {
    BRV_UNUSED(cctx);

    std::ostringstream str;
    str << std::endl << std::endl;
    str << "  Bravo " << BRV_VERSION_MAJOR;
    str << "." << BRV_VERSION_MINOR;
    str << "." << BRV_VERSION_PATCH;
    str << std::endl;
    str << "  A fast and lightweight C++ build system.";
    str << std::endl << std::endl;
    str << "    Usage:" << std::endl;
    str << "      bravo <command> [options]";

    str << std::endl << std::endl;
    str << "    Commands:" << std::endl;
    for (const std::pair<const std::string, std::string> &pair : CMD_USAGE_MAP)
        str << "        " << std::setw(30) << std::left << pair.first << pair.second << std::endl;

    str << std::endl << std::endl;
    str << "    Options:" << std::endl;
    for (const std::pair<const std::string, std::pair<char, std::string>> &pair : OPT_USAGE_MAP)
        str << "        -" << pair.second.first << ", --" << std::setw(24) << std::left << pair.first << pair.second.second << std::endl;

    str << std::endl << std::endl;
    str << "    Example:" << std::endl;
    str << "        bravo " << BRV_CMD_INIT_STR << std::endl;
    str << "        bravo " << BRV_CMD_BUILD_STR << std::endl;
    str << "        bravo " << BRV_CMD_RUN_STR;
    str << " --" << BRV_OPT_DEPS_STR_LONG;
    str << " --" << BRV_OPT_VERBOSE_STR_LONG << std::endl;
    str << "        bravo " << BRV_CMD_TEST_STR;
    str << " --" << BRV_OPT_NO_BUILD_STR_LONG;
    str << " --" << BRV_OPT_VERBOSE_STR_LONG << std::endl;

    BRV_TRACE(str.str());
}
