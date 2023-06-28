# Find HPCToolkit libraries/headers

find_path(HPCTOOLKIT_ROOT
  NAMES include/hpctoolkit.h
)

find_library(HPCTOOLKIT_LIBRARIES
  NAMES hpctoolkit libhpctoolkit
  HINTS ${HPCTOOLKIT_ROOT}/lib/hpctoolkit
)

find_path(HPCTOOLKIT_INCLUDE_DIRS
  NAMES hpctoolkit.h
  HINTS ${HPCTOOLKIT_ROOT}/include
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(HPCTOOLKIT
  DEFAULT_MSG
  HPCTOOLKIT_LIBRARIES
  HPCTOOLKIT_INCLUDE_DIRS
)

mark_as_advanced(
  HPCTOOLKIT_INCLUDE_DIRS
  HPCTOOLKIT_LIBRARIES
)
