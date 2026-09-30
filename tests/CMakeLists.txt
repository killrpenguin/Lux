find_package(Catch2 3 REQUIRED)

add_executable(UnitTests TestsMain.cpp
	"ReplTests.cpp"
	"InputTests.cpp"
	"ValueTests.cpp"
	"ScannerTests.cpp"
	"RegisterVMTests.cpp")

target_link_libraries(UnitTests PRIVATE LL Catch2::Catch2WithMain)





