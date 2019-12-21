cmake_minimum_required(VERSION 3.6.3)
project(PocoNotes)

# ================================== 配置conan ============================================================
find_program(CONAN_EXECUTABLE conan)
if (CONAN_EXECUTABLE)
    # -s os=?
    list(APPEND CONAN_SETTINGS -s os=${CMAKE_SYSTEM_NAME})

    # -s compiler=?
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
        set(CONAN_COMPLIER "gcc")
    elseif(MSVC)
        set(CONAN_COMPLIER "Visual Studio")
    else()
        message(FATAL_ERROR "Unsupport complier.")
    endif()
    list(APPEND CONAN_SETTINGS -s compiler=${CONAN_COMPLIER})

    # -s compiler.version=?
    if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
        list(APPEND CONAN_SETTINGS -s compiler.version=4.9)
    elseif(MSVC)
        if(${MSVC_VERSION} EQUAL 1600)
            list(APPEND CONAN_SETTINGS -s compiler.version=10)
        elseif(${MSVC_VERSION} EQUAL 1700)
            list(APPEND CONAN_SETTINGS -s compiler.version=11)
        elseif(${MSVC_VERSION} EQUAL 1800)
            list(APPEND CONAN_SETTINGS -s compiler.version=12)
        elseif(${MSVC_VERSION} EQUAL 1900)
            list(APPEND CONAN_SETTINGS -s compiler.version=14)
        elseif(${MSVC_VERSION} GREATER 1900)
            list(APPEND CONAN_SETTINGS -s compiler.version=15)
        endif()
    endif()

    # -s compiler.libcxx=?
    list(APPEND CONAN_SETTINGS -s compiler.libcxx=libstdc++11)

    # -s arch=?
    if (NOT CONAN_ARCH)
        message("Processor is: ${CMAKE_SYSTEM_PROCESSOR}")

        string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYSPROC)
        set(X86_ALIAS_LIST x86 i386 i686 x86_64 amd64)
        list(FIND X86_ALIAS_LIST "${SYSPROC}" X86_MATCH_ITER)
        message("X86_MATCH_ITER: ${X86_MATCH_ITER}")
        if("${SYSPROC}" STREQUAL "" OR X86_MATCH_ITER GREATER "-1")
            message(STATUS "Detected x86 target processor")
            set(CONAN_ARCH "x86")
            if("${CMAKE_SIZEOF_VOID_P}" MATCHES 8)
                message(STATUS "Detected x86_64 target processor")
                set(CONAN_ARCH "x86_64")
            endif()
		else()
            message(FATAL_ERROR "Unsupport target system.")
        endif()
    endif()
    list(APPEND CONAN_SETTINGS -s arch=${CONAN_ARCH})

    # conan包安装配置参数
    list(APPEND RELEASE_ARGS install -g cmake_multi -s build_type=Release ${CONAN_SETTINGS})
    list(APPEND DEBUG_ARGS install -g cmake_multi -s build_type=Debug ${CONAN_SETTINGS})
    message("Use this conan settings(release): ${RELEASE_ARGS}")
    message("Use this conan settings(debug): ${DEBUG_ARGS}")

    # 安装conan包，并生成conanbuildinfo_multi.cmake
    execute_process(COMMAND ${CONAN_EXECUTABLE} ${RELEASE_ARGS} ${CMAKE_SOURCE_DIR}
                    RESULT_VARIABLE rlt
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    if (rlt)
        message(FATAL_ERROR "Conan install release failed: ${rlt}")
    endif()

    execute_process(COMMAND ${CONAN_EXECUTABLE} ${DEBUG_ARGS} ${CMAKE_SOURCE_DIR}
                    RESULT_VARIABLE rlt
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    if (rlt)
        message(FATAL_ERROR "Conan install debug failed: ${rlt}")
    endif()

    # 包含conan配置，注意要加上 TARGETS 参数，否则运行程序时找不到链接库
    include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
    conan_basic_setup()
    conan_basic_setup(TARGETS)
endif()

# ================================== 配置基本编译参数 =====================================================
# 设置输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(WIN32)
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "/Zi ${CMAKE_CXX_FLAGS_RELEASE}")
else(WIN32)
    set(CMAKE_CXX_FLAGS "-std=c++11 -g ${CMAKE_CXX_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endif(WIN32)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-static-libgcc -static-libstdc++ -pthread ${CMAKE_CXX_FLAGS}")
endif()


add_subdirectory(${CMAKE_SOURCE_DIR}/source)
