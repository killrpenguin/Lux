set(COMPILER_FLAGS
        -fexceptions
        -fvisibility=hidden
        -ffunction-sections
        -fdata-sections
        -fstack-protector-strong
	-fsafe-buffer-usage-suggestions
        $<$<CONFIG:Debug>:-fstack-usage>
        # Warnings
        -Wall -Wextra -Wpedantic -Wdeprecated -Wcast-align
	-Wunused -Weverything -Weffc++
        -Wformat=2 -Wformat-security -Wimplicit-fallthrough
        -Walloca -Wvla
        -Wcast-qual -Wconversion -Woverloaded-virtual
        -Wshadow -Wundef -Wuninitialized
        -Wold-style-cast -Wzero-as-null-pointer-constant
        -Wsuggest-override -Wnon-virtual-dtor
        -Wmismatched-tags -Winvalid-utf8
        -Wrange-loop-construct -Wmisleading-indentation
        -Wdisabled-optimization -Wdouble-promotion
	-Wlifetime-safety-suggestions -Wdangling
	-Wno-extra-semi -Wno-c++98-compat -Wno-documentation
	-Wno-padded -Wno-c++98-compat-pedantic -Wno-covered-switch-default
    )

# GCC-specific warnings (not supported by Clang)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(COMPILER_FLAGS ${COMPILER_FLAGS}
            -Wformat-overflow=2 -Wformat-truncation=2
            -Wtrampolines -Wno-psabi -Wnrvo
	    -Wuseless-cast -Wnull-dereference
            -Warray-bounds=2 -Wshift-overflow=2
            -Wstringop-overflow=4 -Warith-conversion
            -Wlogical-op -Wduplicated-cond -Wduplicated-branches
            -Wformat-signednes -Wcatch-value=2 -Wredundant-tags
            -Wdangling-reference -Wplacement-new=2
            -Walloc-zero -Wsign-promo -Wredundant-decls
        )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # The anonymous-union layout in object.inl is an intentional GCC extension
        # that clang flags under -Wpedantic; suppress just that one warning.
        set(COMPILER_FLAGS ${COMPILER_FLAGS} -Wno-nested-anon-types)
endif()
