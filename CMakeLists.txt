cmake_minimum_required(VERSION 3.0.0)
project(cpu_usage_tracker VERSION 0.1.0)

include(CTest)
enable_testing()

set(QUEUE_DIRS
    usage_tracker/queue.c
    usage_tracker/inc/queue.h)

set(TRACKER_DIRS
    usage_tracker/tracker.c
    usage_tracker/inc/tracker.h)

set(WDT_DIRS
    usage_tracker/wdt.c
    usage_tracker/inc/wdt.h)
set(LOGGER_DIRS
    usage_tracker/logger.c
    usage_tracker/inc/logger.h)

set(CMAKE_C_FLAGS_DEBUG "-pthread")
set(CMAKE_C_FLAGS_RELEASE "-pthread")

add_executable(cpu_usage_tracker 
    main.c
    ${QUEUE_DIRS}
    ${TRACKER_DIRS}
    ${WDT_DIRS}
    ${LOGGER_DIRS})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
