add_definitions(-DOMEGA_ENGINE_DLL)

file(GLOB ENGINE_HEADER inc/*.h)
file(GLOB ENGINE_SOURCE src/*.cxx)


# ====================== CUDA SUPPORT ======================
if (OMEGA_USE_CUDA)
    find_package(CUDAToolkit REQUIRED)

	file(GLOB ENGINE_CUDA_SOURCE "src/*.cu")
    add_library(engine SHARED ${ENGINE_HEADER} ${ENGINE_SOURCE} ${ENGINE_CUDA_SOURCE})

    set_target_properties(engine PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES native
    )

else()
	add_library(engine SHARED ${ENGINE_HEADER} ${ENGINE_SOURCE})
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