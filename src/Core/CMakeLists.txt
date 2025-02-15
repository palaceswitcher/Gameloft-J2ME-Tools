add_library (Core
	File.cpp
	PackFile.cpp)

#target_link_libraries(UI PRIVATE Misc)

install(TARGETS Core DESTINATION bin)
install(FILES File.hpp DESTINATION include)
install(FILES PackFile.hpp DESTINATION include)
