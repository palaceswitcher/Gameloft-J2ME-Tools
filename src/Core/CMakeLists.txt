add_library (Core
	PackedFile.cpp)

#target_link_libraries(UI PRIVATE Misc)

install(TARGETS Core DESTINATION bin)
install(FILES FileHandler.hpp DESTINATION include)
