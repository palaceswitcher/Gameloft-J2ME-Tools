#ifndef ASSETMANAGER_HPP_
#define ASSETMANAGER_HPP_
#include "AssetFile.hpp"
#include "AssetPack.hpp"
#include "GfxAsset.hpp"
#include <vector>
#include <string>
#include <memory>

/**
 * Loads an asset and assigns the appropriate type.
 * @param fData File data
 * @param fName File name
 * @param fPath File path
 */
std::unique_ptr<GenericAssetFile> loadAsset(std::vector<unsigned char> fData, std::string fName, std::string fPath);

#endif