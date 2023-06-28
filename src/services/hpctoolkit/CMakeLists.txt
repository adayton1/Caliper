set(CALIPER_HPCTOOLKIT_SOURCES
  Nvtx.cpp)

add_library(caliper-hpctoolkit OBJECT ${CALIPER_HPCTOOLKIT_SOURCES})
target_include_directories(caliper-hpctoolkit PRIVATE ${CALI_HPCTOOLKIT_INCLUDE_DIRS})

add_service_objlib("caliper-hpctoolkit")

add_caliper_service("hpctoolkit CALIPER_HAVE_HPCTOOLKIT")
