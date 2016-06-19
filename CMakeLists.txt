cmake_minimum_required(VERSION 3.0)

# where is yara?
set(YaraPath ./yara-3.4.0)
include_directories(${YaraPath}/libyara/include)
link_directories(${YaraPath}/libyara/.libs)

# where is boost?
include_directories(/opt/boost)
link_directories(/opt/boost/stage/lib)

# where is Qt5?
set(CMAKE_PREFIX_PATH "/data/util/qt5/5.7/gcc_64")

# enable c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(AUTOUIC TRUE)
set(AUTOMOC TRUE)
set(QT_QMAKE_EXECUTABLE ${QT_ROOT_DIR}/bin/qmake)
find_package(Qt5Widgets)

include_directories(.)

set(Sources
  src/main.cpp
  src/asio_events.cpp
  src/main_controller.cpp
  src/settings.cpp
  src/ruleset_manager.cpp
  src/ruleset.cpp
  src/ruleset_view.cpp
  src/scanner.cpp
  src/scanner_rule.cpp
  src/main_window.cpp
  src/target_panel.cpp
  src/match_panel.cpp
  src/rule_window.cpp
  src/compile_window.cpp
  src/about_window.cpp
  src/gfx_renderer.cpp
)

QT5_WRAP_CPP(Sources
  src/asio_events.h
  src/main_window.h
  src/target_panel.h
  src/match_panel.h
  src/rule_window.h
  src/compile_window.h
  src/about_window.h
)

QT5_WRAP_UI(Sources
  src/ui/main_window.ui
  src/ui/target_panel.ui
  src/ui/match_panel.ui
  src/ui/rule_window.ui
  src/ui/compile_window.ui
  src/ui/about_window.ui
)

QT5_ADD_RESOURCES(Sources
  res/resources.qrc
)

set(Libraries
  ${Qt5Widgets_LIBRARIES}
  boost_system
  boost_thread
  yara
)

add_executable(yaragui ${Sources})
target_link_libraries(yaragui ${Libraries})
