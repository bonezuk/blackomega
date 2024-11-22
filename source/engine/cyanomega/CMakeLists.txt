add_definitions(-DOMEGA_ENGINE_CYANOMEGA_DLL)

set( CYANOMEGA_ENGINE_HEADER
	musepack/inc/cuetools/cd.h
	musepack/inc/cuetools/cdtext.h
	musepack/inc/cuetools/cuefile.h
	musepack/inc/mpc/datatypes.h
	musepack/inc/mpc/minimax.h
	musepack/inc/mpc/mpc_types.h
	musepack/inc/mpc/mpcdec.h
	musepack/inc/mpc/mpcdec_type.h
	musepack/inc/mpc/mpcmath.h
	musepack/inc/mpc/reader.h
	musepack/inc/mpc/streaminfo.h
	musepack/inc/replaygain/gain_analysis.h
	inc/CyanCodec.h
	inc/CyanOmegaDLL.h
	inc/MusePackDecoder.h
	inc/MusePackDecoderImpl.h
	inc/MusePackIF.h
)

set( CYANOMEGA_ENGINE_SOURCE
	musepack/src/cuefile/cd.c
	musepack/src/cuefile/cdtext.c
	musepack/src/cuefile/cue.h
	musepack/src/cuefile/cue_parse.c
	musepack/src/cuefile/cue_parse.h
	musepack/src/cuefile/cue_parse_prefix.h
	musepack/src/cuefile/cue_print.c
	musepack/src/cuefile/cue_scan.c
	musepack/src/cuefile/cuefile.c
	musepack/src/cuefile/time.c
	musepack/src/cuefile/time.h
	musepack/src/cuefile/toc.h
	musepack/src/cuefile/toc_parse.c
	musepack/src/cuefile/toc_parse.h
	musepack/src/cuefile/toc_parse_prefix.h
	musepack/src/cuefile/toc_print.c
	musepack/src/cuefile/toc_scan.c

	musepack/src/libmpcdec/decoder.h
	musepack/src/libmpcdec/huffman.c
	musepack/src/libmpcdec/huffman.h
	musepack/src/libmpcdec/internal.h
	musepack/src/libmpcdec/mpc_bits_reader.c
	musepack/src/libmpcdec/mpc_bits_reader.h
	musepack/src/libmpcdec/mpc_decoder.c
	musepack/src/libmpcdec/mpc_demux.c
	musepack/src/libmpcdec/mpc_reader.c
	musepack/src/libmpcdec/mpcdec_math.h
	musepack/src/libmpcdec/requant.c
	musepack/src/libmpcdec/requant.h
	musepack/src/libmpcdec/streaminfo.c
	musepack/src/libmpcdec/synth_filter.c
	
	musepack/src/replaygain/gain_analysis.c

	src/CyanCodec.cxx
	src/MusePackDecoder.cxx
	src/MusePackDecoderImpl.cxx
	src/MusePackIF.cxx
)

add_library(cyanomega SHARED ${CYANOMEGA_ENGINE_HEADER} ${CYANOMEGA_ENGINE_SOURCE})

target_include_directories(cyanomega PRIVATE "./musepack/inc/" )

if(MSVC)
	target_compile_options(cyanomega PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(cyanomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm)
else (OMEGA_WIN32)
	target_link_libraries(cyanomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS cyanomega DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
