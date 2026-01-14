cmake_minimum_required(VERSION 3.16)

project(DnfLogin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用Qt自动工具
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt6包
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# 启用废弃API警告
add_compile_definitions(QT_DEPRECATED_WARNINGS)

# 源文件
set(PROJECT_SOURCES
    main.cpp
    widget.cpp
    widget.h
    widget.ui
    clickablelabel.cpp
    clickablelabel.h
    regeditdialog.cpp
    regeditdialog.h
    regeditdialog.ui
    forgetdialog.cpp
    forgetdialog.h
    forgetdialog.ui
    aes_cbc.cpp
    aes_cbc.h
    httpaesrequest.cpp
    httpaesrequest.h
    util.cpp
    util.h
    repasswddialog.cpp
    repasswddialog.h
    repasswddialog.ui
    checkqlineedit.cpp
    checkqlineedit.h
)

# Windows资源文件
# 注释掉避免windres路径问题
# if(WIN32)
#     list(APPEND PROJECT_SOURCES DnfLogin.rc)
# endif()

# 创建可执行文件
if(WIN32)
    add_executable(DnfLogin WIN32 ${PROJECT_SOURCES})
else()
    add_executable(DnfLogin ${PROJECT_SOURCES})
endif()

# 添加包含目录（让UIC生成的头文件能找到自定义组件）
target_include_directories(DnfLogin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 链接Qt库
target_link_libraries(DnfLogin PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# Windows特定设置
if(WIN32)
    # 链接Windows库
    target_link_libraries(DnfLogin PRIVATE
        ws2_32
        iphlpapi
    )

    # 设置Windows子系统
    set_target_properties(DnfLogin PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# 安装规则
install(TARGETS DnfLogin
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Qt部署
if(WIN32)
    # 使用windeployqt自动复制Qt依赖
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET DnfLogin POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:DnfLogin>
            COMMENT "Running windeployqt..."
        )
    endif()
endif()
