CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

FUNCTION(SET_COMPILER_FLAGS)

  # Attempt some platform/compiler introspection
  STRING(COMPARE EQUAL "Linux" ${CMAKE_SYSTEM_NAME} LINUX)
  STRING(COMPARE EQUAL "Windows" ${CMAKE_SYSTEM_NAME} WINDOWS)
  STRING(COMPARE EQUAL "Darwin" ${CMAKE_SYSTEM_NAME} OS_X)

  IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    SET(CLANG TRUE)
  ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    SET(CLANG TRUE)
  ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(GCC TRUE)
  ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    SET(INTEL TRUE)
  ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    SET(MSVC TRUE)
  ENDIF()


  IF(CLANG)

    # suppress missing override warnings which are automatically enabled in recent
    # versions of Clang. These produce a slew of warnings from GiNaC >= 1.7
    # also suppress unknown attribute warnings which produce errors for GiNaC_deprecated
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-inconsistent-missing-override -Wno-unknown-attributes -Wno-undefined-var-template" PARENT_SCOPE)
    #  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" PARENT_SCOPE)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-inconsistent-missing-override -Wno-unknown-attributes -Wno-undefined-var-template" PARENT_SCOPE)
    #  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" PARENT_SCOPE)

  ELSEIF(GCC)

    # suppress unknown attribute warnings which produce errors for GiNaC_deprecated
    # suppress warnings for deprecated binder1st, binder2nd etc. declarations from SPLINTER's bundled Eigen
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-ignored-attributes -Wno-deprecated-declarations" PARENT_SCOPE)
    #  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" PARENT_SCOPE)
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wno-ignored-attributes -Wno-deprecated-declarations" PARENT_SCOPE)
    #  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" PARENT_SCOPE)

  ELSEIF(INTEL)

  ENDIF()

ENDFUNCTION(SET_COMPILER_FLAGS)
