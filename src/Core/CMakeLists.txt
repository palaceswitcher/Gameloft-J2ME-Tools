add_library (Core
	AssetFile.cpp
	AssetPack.cpp
	GfxAsset.cpp
	PngDecode.cpp
	AssetManager.cpp)

target_link_libraries(Core PRIVATE GameloftGfx)
target_link_libraries(Core PRIVATE imagequant)

install(TARGETS Core DESTINATION bin)
install(FILES AssetFile.hpp DESTINATION include)
install(FILES AssetPack.hpp DESTINATION include)
install(FILES GfxAsset.hpp DESTINATION include)
install(FILES PngDecode.hpp DESTINATION include)
install(FILES AssetManager.hpp DESTINATION include)
