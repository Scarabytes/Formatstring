enable_testing()

#-------------------------------------------------------------------------------
# Compile time tests

function(add_compile_test NAME TARGET FILES)
    add_library(${TARGET} OBJECT EXCLUDE_FROM_ALL ${FILES})
    add_test(NAME ${NAME}
        COMMAND
            ${CMAKE_COMMAND} --build . --target ${TARGET} --config $<CONFIGURATION>
        WORKING_DIRECTORY
            ${CMAKE_BINARY_DIR}
    )
endfunction(add_compile_test)


add_compile_test(ToString_Static_Assert test_tostring_assert
        test/compile/TestToStringAssert.cpp)
target_link_libraries(test_tostring_assert formatstring)
set_tests_properties(ToString_Static_Assert PROPERTIES
    PASS_REGULAR_EXPRESSION
        "No conversion for the given type <T> to string was found")

#-------------------------------------------------------------------------------
# Runtime tests using catch

find_package(Catch2)
add_executable(test_runtime
        test/TestMain.cpp
        test/TestFormatException.cpp
        test/TestFormatstring.cpp
        test/TestQuickformat.cpp
        test/TestToString.cpp
        test/TestVariables.cpp
        test/stringify/TestBoolToString.cpp
        test/stringify/TestCollectionToString.cpp
        test/stringify/TestFloatToString.cpp
        test/stringify/TestFormatHelper.cpp
        test/stringify/TestIntToString.cpp
        test/stringify/TestPairToString.cpp
        test/stringify/TestStringToString.cpp
        test/stringify/TestTupleToString.cpp
)
target_link_libraries(test_runtime formatstring Catch2)
add_test(NAME Runtime_Test
        COMMAND test_runtime
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_executable(raw_test test/RawTest.cpp)
target_link_libraries(raw_test formatstring)
