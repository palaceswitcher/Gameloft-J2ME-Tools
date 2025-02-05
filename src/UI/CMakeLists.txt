add_library (UI
	FileMenu.cpp
	FileDialog.cpp)

target_link_libraries(UI PRIVATE Core)
target_link_libraries(UI PRIVATE ImGui)

install(TARGETS UI DESTINATION bin)
install(FILES FileDialog.hpp DESTINATION include)
