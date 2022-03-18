/**
 * @file src/selector/selector.cpp
 * @author RenoirTan
 * @brief Source file for Fidgety's "selectors"
 * @version 0.1
 * @date 2022-03-15
 * 
 * @copyright Copyright (c) 2022
 */

#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/regex.hpp>
#include <spdlog/spdlog.h>
#include <fidgety/appdata.hpp>
#include <fidgety/selector.hpp>

using namespace Fidgety;
namespace BoostFs = boost::filesystem;

void ProcessedPartLocations::clear(void) {
    this->decoder.clear();
    this->encoder.clear();
    this->validator.clear();
    this->validatorContextCreator.clear();
}

Selector::Selector(const Appdata &appdata) : mAppdata(appdata) { }

Selector::Selector(Appdata &&appdata) : mAppdata(std::move(appdata)) { }

bool Selector::isValid(void) const {
// Check if LoadablePartsFileNames is non-zero (not empty)
#define CHK_LPFN_NZ(part)                                   \
    if (mAppdata.loadablePartsFileNames.part.size() == 0) { \
        return false;                                       \
    }                                                       \

    CHK_LPFN_NZ(decoder);
    CHK_LPFN_NZ(encoder);
    CHK_LPFN_NZ(validator);
    CHK_LPFN_NZ(validatorContextCreator);
#undef CHK_LPFN_NZ

    return true;
}

SelectorStatus Selector::processHints(void) {
   ProcessedPartLocations ppl;

    for (const auto &spath : mAppdata.searchPaths) {
        BoostFs::path dir(spath);
        if (BoostFs::exists(dir) && BoostFs::is_directory(dir)) {
            BoostFs::directory_iterator it_end;
            for (BoostFs::directory_iterator node(dir); node != it_end; ++node) {
                BoostFs::path path(node->path());
                if (BoostFs::is_regular_file(path)) {
#define _CHK_LPFN(part) { \
    const char *cPath = path.c_str(); \
    auto it = std::find( \
        mAppdata.loadablePartsFileNames.part.cbegin(), \
        mAppdata.loadablePartsFileNames.part.cend(), \
        cPath \
    ); \
    if (it != mAppdata.loadablePartsFileNames.part.cend()) { \
        ppl.part.emplace_back(cPath); \
    } \
} \

                    _CHK_LPFN(decoder);
                    _CHK_LPFN(encoder);
                    _CHK_LPFN(validator);
                    _CHK_LPFN(validatorContextCreator);

#undef _CHK_LPFN
                }
            }
        }
    }

    mLocations = std::move(ppl);
    return SelectorStatus::Ok;
}

std::string Selector::getDecoderLocation(void) {
    if (mLocations.decoder.size() == 0) {
        return "";
    } else {
        return mLocations.decoder.at(0);
    }
}

std::string Selector::getEncoderLocation(void) {
    if (mLocations.encoder.size() == 0) {
        return "";
    } else {
        return mLocations.encoder.at(0);
    }
}

std::string Selector::getValidatorLocation(void) {
    if (mLocations.validator.size() == 0) {
        return "";
    } else {
        return mLocations.validator.at(0);
    }
}

std::string Selector::getValidatorContextCreatorLocation(void) {
    if (mLocations.validatorContextCreator.size() == 0) {
        return "";
    } else {
        return mLocations.validatorContextCreator.at(0);
    }
}
