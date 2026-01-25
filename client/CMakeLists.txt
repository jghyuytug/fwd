cmake_minimum_required(VERSION 3.15)
project(DnfClient)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# DirectX 9 SDK 路径 (根据实际安装路径调整)
set(DIRECTX_SDK_DIR "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)")

# 包含目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    "${DIRECTX_SDK_DIR}/Include"
)

# 库目录
link_directories(
    "${DIRECTX_SDK_DIR}/Lib/x86"
)

# 源文件
set(SOURCES
    src/main.cpp
)

# 创建可执行文件
add_executable(DnfClient WIN32 ${SOURCES})

# 链接库
target_link_libraries(DnfClient
    d3d9.lib
    d3dx9.lib
    dinput8.lib
    dxguid.lib
    ws2_32.lib
    winmm.lib
)

# 设置为32位
set_target_properties(DnfClient PROPERTIES
    WIN32_EXECUTABLE TRUE
    VS_PLATFORM_NAME "Win32"
)

# Visual Studio 特定设置
if(MSVC)
    # 设置启动项目
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DnfClient)

    # 设置调试工作目录
    set_target_properties(DnfClient PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )

    # 编译选项
    target_compile_options(DnfClient PRIVATE
        /W3          # 警告级别3
        /MP          # 多处理器编译
        /Zi          # 生成调试信息
    )

    # 链接选项
    target_link_options(DnfClient PRIVATE
        /SUBSYSTEM:WINDOWS
        /MACHINE:X86
    )
endif()

# 复制DLL到输出目录 (如果需要)
# add_custom_command(TARGET DnfClient POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     "${DIRECTX_SDK_DIR}/Developer Runtime/x86/D3DX9_43.dll"
#     $<TARGET_FILE_DIR:DnfClient>
# )
