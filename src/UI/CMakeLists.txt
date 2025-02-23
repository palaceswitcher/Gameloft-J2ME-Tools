add_library (UI
	UI.cpp
	FileView.cpp
	GfxView.cpp
	FileDialog.cpp)

target_link_libraries(UI PRIVATE Core)
target_link_libraries(UI PRIVATE ImGui)

install(TARGETS UI DESTINATION bin)
install(FILES FileView.hpp DESTINATION include)
install(FILES GfxView.hpp DESTINATION include)
install(FILES FileDialog.hpp DESTINATION include)
