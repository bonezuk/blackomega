add_definitions(-DOMEGA_ENGINE_DLL)

file(GLOB ENGINE_HEADER inc/*.h)
file(GLOB ENGINE_SOURCE src/*.cxx)

qt_add_resources(ENGINE_UI_RESOURCES engine.qrc)

# ====================== CUDA SUPPORT ======================
if (OMEGA_USE_CUDA)
    find_package(CUDAToolkit REQUIRED)

	file(GLOB ENGINE_CUDA_SOURCE "src/*.cu")
    add_library(engine SHARED ${ENGINE_HEADER} ${ENGINE_SOURCE} ${ENGINE_CUDA_SOURCE} ${ENGINE_UI_RESOURCES})

    if (OMEGA_WIN32)
        set_target_properties(engine PROPERTIES
            CUDA_SEPARABLE_COMPILATION ON
            CUDA_ARCHITECTURES "75;86;87;89;120"
        )
    else ()
        set_target_properties(engine PROPERTIES
            CUDA_SEPARABLE_COMPILATION ON
            CUDA_ARCHITECTURES "61;75;86;87;89"
        )
    endif ()

    if (TIGER_DEBUG_BUILD)
        target_compile_options(engine PRIVATE
            $<$<COMPILE_LANGUAGE:CUDA>:-G -O0>
        )
    endif()
else()
	add_library(engine SHARED ${ENGINE_HEADER} ${ENGINE_SOURCE} ${ENGINE_UI_RESOURCES})
endif()
# ========================================================

# MSVC-specific options (fixed)
if(OMEGA_MSVC16)
    target_compile_options(engine PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:/MP>
    )
endif()

# Fixed linking - all keywords, one call
target_link_libraries(engine PRIVATE
    common
    xml2
    ${QTVER}::Core
    ${QTVER}::Gui
    ${QTVER}::Xml
    $<$<BOOL:${OMEGA_WIN32}>:winmm>
    $<$<BOOL:${OMEGA_USE_CUDA}>:CUDA::cudart>
)

if (${TIGER_LINUX_DISTRO})
    install(TARGETS engine DESTINATION lib)
endif()