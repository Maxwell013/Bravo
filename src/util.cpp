#include <bravo/bravo.hpp>
#include <vector>

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

void file::recurse(const fs::path &dir, std::vector<fs::path> &files, const std::string &target_ext) {
    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;

        fs::path ext = entry.path().extension();
        if (ext == target_ext)
            files.emplace_back(fs::absolute(entry.path()));
    }
}

void file::swap(const std::vector<fs::path> &srcs, std::vector<fs::path> &dsts, const fs::path &src, const fs::path &dst, const std::string &ext) {
    for (const fs::path &file : srcs) {
        fs::path end = fs::relative(file, src).replace_extension(ext);
        dsts.emplace_back(dst / end);
    }
}
