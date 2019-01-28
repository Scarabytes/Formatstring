cmake_minimum_required(VERSION 3.5)
project(formatstring LANGUAGES CXX VERSION 1.2)

if (MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra)
endif()

#-------------------------------------------------------------------------------

add_library(formatstring "" include/formatstring/stringify/ChronoToString.h src/formatstring/stringify/ChronoToString.cpp)
target_sources(formatstring
    PRIVATE
        include/formatstring/detail/ToStringHandler.h
        include/formatstring/detail/Variable.h
        include/formatstring/err/FormatException.h
        include/formatstring/stringify/BoolToString.h
        include/formatstring/stringify/CollectionToString.h
        include/formatstring/stringify/FloatToString.h
        include/formatstring/stringify/FormatHelper.h
        include/formatstring/stringify/IntToString.h
        include/formatstring/stringify/PairToString.h
        include/formatstring/stringify/StringToString.h
        include/formatstring/stringify/TupleToString.h
        include/formatstring/util/Assert.h
        include/formatstring/util/Metafunctions.h
        include/formatstring/util/PointerUtil.h
        include/formatstring/Formatstring.h
        include/formatstring/QuickFormat.h
        include/formatstring/ToString.h
        include/formatstring/Wrapper.h

        src/formatstring/err/FormatException.cpp
        src/formatstring/stringify/BoolToString.cpp
        src/formatstring/stringify/FloatToString.cpp
        src/formatstring/stringify/FormatHelper.cpp
        src/formatstring/stringify/Grisu2.cpp
        src/formatstring/stringify/IntToString.cpp
        src/formatstring/stringify/StringToString.cpp
        src/formatstring/Formatstring.cpp
)
target_include_directories(formatstring PUBLIC include PRIVATE src)
target_compile_features(formatstring PUBLIC cxx_std_11)

#-------------------------------------------------------------------------------

include(Tests.cmake)
