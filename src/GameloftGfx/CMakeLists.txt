add_library (GameloftGfx
	GameloftGfx.cpp)

#target_link_libraries(UI PRIVATE Misc)

install(TARGETS GameloftGfx DESTINATION bin)
install(FILES GameloftGfx.hpp DESTINATION include)
