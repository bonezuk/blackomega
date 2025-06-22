ios {
	DEFINES += OMEGA_IOS
	QMAKE_CFLAGS   += -Wno-shorten-64-to-32
	QMAKE_CXXFLAGS += -Wno-shorten-64-to-32
}

QMAKE_CFLAGS   += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-unused-parameter

android {
	DEFINES += OMEGA_ANDROID
}

TEMPLATE = app

QT += quick qml xml core5compat

TARGET = BlackOmegaIOS

# --- common ---

HEADERS  = ./common/inc/AbstractProgressInterface.h
HEADERS += ./common/inc/Allocation.h
HEADERS += ./common/inc/Array.h
HEADERS += ./common/inc/BIOBufferedStream.h
HEADERS += ./common/inc/BIOMemory.h
HEADERS += ./common/inc/BIOMemoryStream.h
HEADERS += ./common/inc/BIOStream.h
HEADERS += ./common/inc/BOParse.h
HEADERS += ./common/inc/BOQueueTree.h
HEADERS += ./common/inc/BOTree.h
HEADERS += ./common/inc/BSemaphoreQueue.h
HEADERS += ./common/inc/BString.h
HEADERS += ./common/inc/BStringCollection.h
HEADERS += ./common/inc/CRC32.h
HEADERS += ./common/inc/CommonDLL.h
HEADERS += ./common/inc/CommonDirectoriesForFiles.h
HEADERS += ./common/inc/CommonFunctions.h
HEADERS += ./common/inc/CommonTypes.h
HEADERS += ./common/inc/DLong.h
HEADERS += ./common/inc/DebugOutput.h
HEADERS += ./common/inc/DictionaryXMLMap.h
HEADERS += ./common/inc/DiskIF.h
HEADERS += ./common/inc/DiskLayerIF.h
HEADERS += ./common/inc/DiskOps.h
HEADERS += ./common/inc/Exception.h
HEADERS += ./common/inc/Factory.h
HEADERS += ./common/inc/JaroWinklerDistance.h
HEADERS += ./common/inc/LargeNumber.h
HEADERS += ./common/inc/Log.h
HEADERS += ./common/inc/MD5Digest.h
HEADERS += ./common/inc/Memory.h
HEADERS += ./common/inc/Mutex.h
HEADERS += ./common/inc/ProcessThread.h
HEADERS += ./common/inc/ProductVersionInfo.h
HEADERS += ./common/inc/QBIOStreamDevice.h
HEADERS += ./common/inc/Random.h
HEADERS += ./common/inc/SBService.h
HEADERS += ./common/inc/SHA1Digest.h
HEADERS += ./common/inc/Semaphore.h
HEADERS += ./common/inc/ServiceEvent.h
HEADERS += ./common/inc/TTime.h
HEADERS += ./common/inc/TimeStamp.h
HEADERS += ./common/inc/UString.h

SOURCES  = ./common/src/AbstractProgressInterface.cxx
SOURCES += ./common/src/Allocation.cxx
SOURCES += ./common/src/BIOBufferedStream.cxx
SOURCES += ./common/src/BIOMemory.cxx
SOURCES += ./common/src/BIOMemoryStream.cxx
SOURCES += ./common/src/BIOStream.cxx
SOURCES += ./common/src/BOParse.cxx
SOURCES += ./common/src/BString.cxx
SOURCES += ./common/src/BStringCollection.cxx
SOURCES += ./common/src/CRC32.cxx
SOURCES += ./common/src/CommonDirectoriesForFiles.cxx
SOURCES += ./common/src/CommonFunctions.cxx
SOURCES += ./common/src/DLong.cxx
SOURCES += ./common/src/DebugOutput.cxx
SOURCES += ./common/src/DictionaryXMLMap.cxx
SOURCES += ./common/src/DiskIF.cxx
SOURCES += ./common/src/DiskLayerIF.cxx
SOURCES += ./common/src/DiskOps.cxx
SOURCES += ./common/src/Exception.cxx
SOURCES += ./common/src/JaroWinklerDistance.cxx
SOURCES += ./common/src/LargeNumber.cxx
SOURCES += ./common/src/Log.cxx
SOURCES += ./common/src/MD5Digest.cxx
SOURCES += ./common/src/Memory.cxx
SOURCES += ./common/src/Mutex.cxx
SOURCES += ./common/src/ProcessThread.cxx
SOURCES += ./common/src/ProductVersionInfo.cxx
SOURCES += ./common/src/QBIOStreamDevice.cxx
SOURCES += ./common/src/Random.cxx
SOURCES += ./common/src/SBService.cxx
SOURCES += ./common/src/SHA1Digest.cxx
SOURCES += ./common/src/Semaphore.cxx
SOURCES += ./common/src/ServiceEvent.cxx
SOURCES += ./common/src/TTime.cxx
SOURCES += ./common/src/TimeStamp.cxx
SOURCES += ./common/src/UString.cxx

# --- engine ---

HEADERS += ./engine/inc/AData.h
HEADERS += ./engine/inc/BFileStream.h
HEADERS += ./engine/inc/BTime.h
HEADERS += ./engine/inc/BitStream.h
HEADERS += ./engine/inc/Codec.h
HEADERS += ./engine/inc/Compare.h
HEADERS += ./engine/inc/EngineDLL.h
HEADERS += ./engine/inc/File.h
HEADERS += ./engine/inc/FIRFilter.h
HEADERS += ./engine/inc/FIRFilter200HzLowPass.h
HEADERS += ./engine/inc/FormatType.h
HEADERS += ./engine/inc/FormatTypeFromFloat.h
HEADERS += ./engine/inc/IOFile.h
HEADERS += ./engine/inc/InterleavedCodec.h
HEADERS += ./engine/inc/KaiserFilter.h
HEADERS += ./engine/inc/OmegaDCT.h
HEADERS += ./engine/inc/OmegaDCT64.h
HEADERS += ./engine/inc/RData.h
HEADERS += ./engine/inc/Recycler.h
HEADERS += ./engine/inc/Resample.h
HEADERS += ./engine/inc/Sequence.h
HEADERS += ./engine/inc/SequenceMemory.h
HEADERS += ./engine/inc/WavChannelMask.h

SOURCES += ./engine/src/AData.cxx
SOURCES += ./engine/src/BFileStream.cxx
SOURCES += ./engine/src/BTime.cxx
SOURCES += ./engine/src/BitStream.cxx
SOURCES += ./engine/src/Codec.cxx
SOURCES += ./engine/src/Compare.cxx
SOURCES += ./engine/src/File.cxx
SOURCES += ./engine/src/FIRFilter.cxx
SOURCES += ./engine/src/FIRFilter200HzLowPass.cxx
SOURCES += ./engine/src/FormatType.cxx
SOURCES += ./engine/src/FormatTypeFromFloat.cxx
SOURCES += ./engine/src/IOFile.cxx
SOURCES += ./engine/src/InterleavedCodec.cxx
SOURCES += ./engine/src/KaiserFilter.cxx
SOURCES += ./engine/src/OmegaDCT.cxx
SOURCES += ./engine/src/OmegaDCT64.cxx
SOURCES += ./engine/src/RData.cxx
SOURCES += ./engine/src/Resample.cxx
SOURCES += ./engine/src/Sequence.cxx
SOURCES += ./engine/src/SequenceMemory.cxx
SOURCES += ./engine/src/WavChannelMask.cxx

# --- engine/blackomega ---

HEADERS += ./engine/blackomega/inc/AntiAlias.h
HEADERS += ./engine/blackomega/inc/BSequence.h
HEADERS += ./engine/blackomega/inc/BandIndex.h
HEADERS += ./engine/blackomega/inc/BlackOmegaDLL.h
HEADERS += ./engine/blackomega/inc/Dequantize.h
HEADERS += ./engine/blackomega/inc/DequantizeStereo.h
HEADERS += ./engine/blackomega/inc/Hybrid.h
HEADERS += ./engine/blackomega/inc/MPCodec.h
HEADERS += ./engine/blackomega/inc/MPHeader.h
HEADERS += ./engine/blackomega/inc/Reorder.h
HEADERS += ./engine/blackomega/inc/ScaleFactor.h
HEADERS += ./engine/blackomega/inc/SideInfo.h
HEADERS += ./engine/blackomega/inc/Stereo.h
HEADERS += ./engine/blackomega/inc/Subband.h

SOURCES += ./engine/blackomega/src/AntiAlias.cxx
SOURCES += ./engine/blackomega/src/BSequence.cxx
SOURCES += ./engine/blackomega/src/BandIndex.cxx
SOURCES += ./engine/blackomega/src/Dequantize.cxx
SOURCES += ./engine/blackomega/src/DequantizeStereo.cxx
SOURCES += ./engine/blackomega/src/Hybrid.cxx
SOURCES += ./engine/blackomega/src/MPCodec.cxx
SOURCES += ./engine/blackomega/src/MPHeader.cxx
SOURCES += ./engine/blackomega/src/Reorder.cxx
SOURCES += ./engine/blackomega/src/ScaleFactor.cxx
SOURCES += ./engine/blackomega/src/SideInfo.cxx
SOURCES += ./engine/blackomega/src/Stereo.cxx
SOURCES += ./engine/blackomega/src/Subband.cxx

# --- engine/blueomega ---

HEADERS += ./engine/blueomega/inc/BlueOmegaDLL.h
HEADERS += ./engine/blueomega/inc/WaveEngine.h
HEADERS += ./engine/blueomega/inc/WaveInformation.h

SOURCES += ./engine/blueomega/src/WaveEngine.cxx
SOURCES += ./engine/blueomega/src/WaveInformation.cxx

# --- engine/cyanomega ---

INCLUDEPATH += ./engine/cyanomega/musepack/inc

HEADERS += ./engine/cyanomega/musepack/inc/cuetools/cd.h
HEADERS += ./engine/cyanomega/musepack/inc/cuetools/cdtext.h
HEADERS += ./engine/cyanomega/musepack/inc/cuetools/cuefile.h

HEADERS += ./engine/cyanomega/musepack/src/cuefile/cue.h
HEADERS += ./engine/cyanomega/musepack/src/cuefile/cue_parse.h
HEADERS += ./engine/cyanomega/musepack/src/cuefile/cue_parse_prefix.h
HEADERS += ./engine/cyanomega/musepack/src/cuefile/time.h
HEADERS += ./engine/cyanomega/musepack/src/cuefile/toc.h
HEADERS += ./engine/cyanomega/musepack/src/cuefile/toc_parse.h
HEADERS += ./engine/cyanomega/musepack/src/cuefile/toc_parse_prefix.h

SOURCES += ./engine/cyanomega/musepack/src/cuefile/cd.c
SOURCES += ./engine/cyanomega/musepack/src/cuefile/cdtext.c
SOURCES += ./engine/cyanomega/musepack/src/cuefile/cue_parse.c
SOURCES += ./engine/cyanomega/musepack/src/cuefile/cue_print.c
SOURCES += ./engine/cyanomega/musepack/src/cuefile/cue_scan.c
SOURCES += ./engine/cyanomega/musepack/src/cuefile/cuefile.c
SOURCES += ./engine/cyanomega/musepack/src/cuefile/time.c
SOURCES += ./engine/cyanomega/musepack/src/cuefile/toc_parse.c
SOURCES += ./engine/cyanomega/musepack/src/cuefile/toc_print.c
SOURCES += ./engine/cyanomega/musepack/src/cuefile/toc_scan.c

HEADERS += ./engine/cyanomega/musepack/inc/replaygain/gain_analysis.h

SOURCES += ./engine/cyanomega/musepack/src/replaygain/gain_analysis.c

HEADERS += ./engine/cyanomega/musepack/inc/mpc/datatypes.h
HEADERS += ./engine/cyanomega/musepack/inc/mpc/minimax.h
HEADERS += ./engine/cyanomega/musepack/inc/mpc/mpc_types.h
HEADERS += ./engine/cyanomega/musepack/inc/mpc/mpcdec.h
HEADERS += ./engine/cyanomega/musepack/inc/mpc/mpcmath.h
HEADERS += ./engine/cyanomega/musepack/inc/mpc/reader.h
HEADERS += ./engine/cyanomega/musepack/inc/mpc/streaminfo.h

HEADERS += ./engine/cyanomega/musepack/src/libmpcdec/decoder.h
HEADERS += ./engine/cyanomega/musepack/src/libmpcdec/huffman.h
HEADERS += ./engine/cyanomega/musepack/src/libmpcdec/internal.h
HEADERS += ./engine/cyanomega/musepack/src/libmpcdec/mpc_bits_reader.h
HEADERS += ./engine/cyanomega/musepack/src/libmpcdec/mpcdec_math.h
HEADERS += ./engine/cyanomega/musepack/src/libmpcdec/requant.h

SOURCES += ./engine/cyanomega/musepack/src/libmpcdec/huffman.c
SOURCES += ./engine/cyanomega/musepack/src/libmpcdec/mpc_bits_reader.c
SOURCES += ./engine/cyanomega/musepack/src/libmpcdec/mpc_decoder.c
SOURCES += ./engine/cyanomega/musepack/src/libmpcdec/mpc_demux.c
SOURCES += ./engine/cyanomega/musepack/src/libmpcdec/mpc_reader.c
SOURCES += ./engine/cyanomega/musepack/src/libmpcdec/requant.c
SOURCES += ./engine/cyanomega/musepack/src/libmpcdec/streaminfo.c
SOURCES += ./engine/cyanomega/musepack/src/libmpcdec/synth_filter.c

HEADERS += ./engine/cyanomega/inc/CyanCodec.h
HEADERS += ./engine/cyanomega/inc/CyanOmegaDLL.h
HEADERS += ./engine/cyanomega/inc/MusePackDecoder.h
HEADERS += ./engine/cyanomega/inc/MusePackDecoderImpl.h
HEADERS += ./engine/cyanomega/inc/MusePackIF.h

SOURCES += ./engine/cyanomega/src/CyanCodec.cxx
SOURCES += ./engine/cyanomega/src/MusePackDecoder.cxx
SOURCES += ./engine/cyanomega/src/MusePackDecoderImpl.cxx
SOURCES += ./engine/cyanomega/src/MusePackIF.cxx

# --- engine/greenomega ---

HEADERS += ./engine/greenomega/inc/FLACFrame.h
HEADERS += ./engine/greenomega/inc/FLACFramework.h
HEADERS += ./engine/greenomega/inc/FLACHeader.h
HEADERS += ./engine/greenomega/inc/FLACMetaBase.h
HEADERS += ./engine/greenomega/inc/FLACMetaComment.h
HEADERS += ./engine/greenomega/inc/FLACMetaCuesheet.h
HEADERS += ./engine/greenomega/inc/FLACMetaPicture.h
HEADERS += ./engine/greenomega/inc/FLACMetaSeekTable.h
HEADERS += ./engine/greenomega/inc/FLACMetaStreamInfo.h
HEADERS += ./engine/greenomega/inc/FLACSubframe.h
HEADERS += ./engine/greenomega/inc/GreenCodec.h
HEADERS += ./engine/greenomega/inc/GreenOmegaDLL.h

SOURCES += ./engine/greenomega/src/FLACFrame.cxx
SOURCES += ./engine/greenomega/src/FLACFramework.cxx
SOURCES += ./engine/greenomega/src/FLACHeader.cxx
SOURCES += ./engine/greenomega/src/FLACMetaBase.cxx
SOURCES += ./engine/greenomega/src/FLACMetaComment.cxx
SOURCES += ./engine/greenomega/src/FLACMetaCuesheet.cxx
SOURCES += ./engine/greenomega/src/FLACMetaPicture.cxx
SOURCES += ./engine/greenomega/src/FLACMetaSeekTable.cxx
SOURCES += ./engine/greenomega/src/FLACMetaStreamInfo.cxx
SOURCES += ./engine/greenomega/src/FLACSubframe.cxx
SOURCES += ./engine/greenomega/src/GreenCodec.cxx

# --- engine/redomega ---

HEADERS += ./engine/redomega/inc/ALACAdaptiveDecode.h
HEADERS += ./engine/redomega/inc/ALACCafContainer.h
HEADERS += ./engine/redomega/inc/ALACContainer.h
HEADERS += ./engine/redomega/inc/ALACDecoder.h
HEADERS += ./engine/redomega/inc/ALACDescription.h
HEADERS += ./engine/redomega/inc/ALACFrameIndex.h
HEADERS += ./engine/redomega/inc/ALACMatrix.h
HEADERS += ./engine/redomega/inc/ALACSequence.h
HEADERS += ./engine/redomega/inc/ALACSpecificConfig.h
HEADERS += ./engine/redomega/inc/CAFFile.h
HEADERS += ./engine/redomega/inc/RedCodec.h
HEADERS += ./engine/redomega/inc/RedOmegaDLL.h

SOURCES += ./engine/redomega/src/ALACAdaptiveDecode.cxx
SOURCES += ./engine/redomega/src/ALACCafContainer.cxx
SOURCES += ./engine/redomega/src/ALACContainer.cxx
SOURCES += ./engine/redomega/src/ALACDecoder.cxx
SOURCES += ./engine/redomega/src/ALACDescription.cxx
SOURCES += ./engine/redomega/src/ALACFrameIndex.cxx
SOURCES += ./engine/redomega/src/ALACMatrix.cxx
SOURCES += ./engine/redomega/src/ALACSequence.cxx
SOURCES += ./engine/redomega/src/ALACSpecificConfig.cxx
SOURCES += ./engine/redomega/src/CAFFile.cxx
SOURCES += ./engine/redomega/src/RedCodec.cxx

# --- engine/silveromega ---

HEADERS += ./engine/silveromega/inc/SilverCodec.h
HEADERS += ./engine/silveromega/inc/SilverOmegaDLL.h
HEADERS += ./engine/silveromega/inc/VSilverCodebook.h
HEADERS += ./engine/silveromega/inc/VSilverCodecComments.h
HEADERS += ./engine/silveromega/inc/VSilverCodecData.h
HEADERS += ./engine/silveromega/inc/VSilverCodecInformation.h
HEADERS += ./engine/silveromega/inc/VSilverContainer.h
HEADERS += ./engine/silveromega/inc/VSilverFloorBase.h
HEADERS += ./engine/silveromega/inc/VSilverFloorData0.h
HEADERS += ./engine/silveromega/inc/VSilverFloorData1.h
HEADERS += ./engine/silveromega/inc/VSilverFunctions.h
HEADERS += ./engine/silveromega/inc/VSilverInfoObject.h
HEADERS += ./engine/silveromega/inc/VSilverMapData.h
HEADERS += ./engine/silveromega/inc/VSilverModeData.h
HEADERS += ./engine/silveromega/inc/VSilverOgg.h
HEADERS += ./engine/silveromega/inc/VSilverResidueBase.h
HEADERS += ./engine/silveromega/inc/VSilverSeeker.h
HEADERS += ./engine/silveromega/inc/VSilverWindow.h

SOURCES += ./engine/silveromega/src/SilverCodec.cxx
SOURCES += ./engine/silveromega/src/VSilverCodebook.cxx
SOURCES += ./engine/silveromega/src/VSilverCodecComments.cxx
SOURCES += ./engine/silveromega/src/VSilverCodecData.cxx
SOURCES += ./engine/silveromega/src/VSilverCodecInformation.cxx
SOURCES += ./engine/silveromega/src/VSilverContainer.cxx
SOURCES += ./engine/silveromega/src/VSilverFloorBase.cxx
SOURCES += ./engine/silveromega/src/VSilverFloorData0.cxx
SOURCES += ./engine/silveromega/src/VSilverFloorData1.cxx
SOURCES += ./engine/silveromega/src/VSilverFrameSynthesis.cxx
SOURCES += ./engine/silveromega/src/VSilverFunctions.cxx
SOURCES += ./engine/silveromega/src/VSilverInfoObject.cxx
SOURCES += ./engine/silveromega/src/VSilverMapData.cxx
SOURCES += ./engine/silveromega/src/VSilverModeData.cxx
SOURCES += ./engine/silveromega/src/VSilverOgg.cxx
SOURCES += ./engine/silveromega/src/VSilverResidueBase.cxx
SOURCES += ./engine/silveromega/src/VSilverSeeker.cxx
SOURCES += ./engine/silveromega/src/VSilverWindow.cxx

# --- engine/violetomega ---

HEADERS += ./engine/violetomega/inc/IFFChunk.h
HEADERS += ./engine/violetomega/inc/IFFCommonChunk.h
HEADERS += ./engine/violetomega/inc/IFFFile.h
HEADERS += ./engine/violetomega/inc/IFFFormChunk.h
HEADERS += ./engine/violetomega/inc/IFFID3Chunk.h
HEADERS += ./engine/violetomega/inc/IFFSoundChunk.h
HEADERS += ./engine/violetomega/inc/IFFTextChunk.h
HEADERS += ./engine/violetomega/inc/VioletCodec.h
HEADERS += ./engine/violetomega/inc/VioletOmegaDLL.h

SOURCES += ./engine/violetomega/src/IFFChunk.cxx
SOURCES += ./engine/violetomega/src/IFFCommonChunk.cxx
SOURCES += ./engine/violetomega/src/IFFFile.cxx
SOURCES += ./engine/violetomega/src/IFFFormChunk.cxx
SOURCES += ./engine/violetomega/src/IFFID3Chunk.cxx
SOURCES += ./engine/violetomega/src/IFFSoundChunk.cxx
SOURCES += ./engine/violetomega/src/IFFTextChunk.cxx
SOURCES += ./engine/violetomega/src/VioletCodec.cxx

# --- engine/whiteomega ---

HEADERS += ./engine/whiteomega/inc/AACDecode.h
HEADERS += ./engine/whiteomega/inc/AACRoot.h
HEADERS += ./engine/whiteomega/inc/ALACAtomContainer.h
HEADERS += ./engine/whiteomega/inc/Atom.h
HEADERS += ./engine/whiteomega/inc/CPEDecode.h
HEADERS += ./engine/whiteomega/inc/Common.h
HEADERS += ./engine/whiteomega/inc/FillDecode.h
HEADERS += ./engine/whiteomega/inc/GAConfig.h
HEADERS += ./engine/whiteomega/inc/HCR.h
HEADERS += ./engine/whiteomega/inc/ICS.h
HEADERS += ./engine/whiteomega/inc/PNS.h
HEADERS += ./engine/whiteomega/inc/Prediction.h
HEADERS += ./engine/whiteomega/inc/PredictionLTP.h
HEADERS += ./engine/whiteomega/inc/ProgramConfig.h
HEADERS += ./engine/whiteomega/inc/TNS.h
HEADERS += ./engine/whiteomega/inc/WCompare.h
HEADERS += ./engine/whiteomega/inc/WSequence.h
HEADERS += ./engine/whiteomega/inc/WhiteCodec.h
HEADERS += ./engine/whiteomega/inc/WhiteOmegaDLL.h
HEADERS += ./engine/whiteomega/inc/Window.h

SOURCES += ./engine/whiteomega/src/AACDecode.cxx
SOURCES += ./engine/whiteomega/src/AACRoot.cxx
SOURCES += ./engine/whiteomega/src/ALACAtomContainer.cxx
SOURCES += ./engine/whiteomega/src/Atom.cxx
SOURCES += ./engine/whiteomega/src/CPEDecode.cxx
SOURCES += ./engine/whiteomega/src/Common.cxx
SOURCES += ./engine/whiteomega/src/FillDecode.cxx
SOURCES += ./engine/whiteomega/src/GAConfig.cxx
SOURCES += ./engine/whiteomega/src/HCR.cxx
SOURCES += ./engine/whiteomega/src/PNS.cxx
SOURCES += ./engine/whiteomega/src/Prediction.cxx
SOURCES += ./engine/whiteomega/src/PredictionLTP.cxx
SOURCES += ./engine/whiteomega/src/ProgramConfig.cxx
SOURCES += ./engine/whiteomega/src/TNS.cxx
SOURCES += ./engine/whiteomega/src/WCompare.cxx
SOURCES += ./engine/whiteomega/src/WSequence.cxx
SOURCES += ./engine/whiteomega/src/WhiteCodec.cxx
SOURCES += ./engine/whiteomega/src/Window.cxx

# --- network ---

HEADERS += ./network/inc/Controller.h
HEADERS += ./network/inc/NetMemory.h
HEADERS += ./network/inc/MemoryPacket.h
HEADERS += ./network/inc/NTPClient.h
HEADERS += ./network/inc/NetworkDLL.h
HEADERS += ./network/inc/Resource.h
HEADERS += ./network/inc/Service.h
HEADERS += ./network/inc/Socket.h
HEADERS += ./network/inc/TCPClientService.h
HEADERS += ./network/inc/TCPConnClientSocket.h
HEADERS += ./network/inc/TCPConnServerSocket.h
HEADERS += ./network/inc/TCPConnectionSocket.h
HEADERS += ./network/inc/TCPServerSocket.h
HEADERS += ./network/inc/TCPService.h
HEADERS += ./network/inc/UDPRead.h
HEADERS += ./network/inc/UDPWrite.h

SOURCES += ./network/src/Controller.cxx
SOURCES += ./network/src/NetMemory.cxx
SOURCES += ./network/src/MemoryPacket.cxx
SOURCES += ./network/src/NTPClient.cxx
SOURCES += ./network/src/Resource.cxx
SOURCES += ./network/src/Service.cxx
SOURCES += ./network/src/Socket.cxx
SOURCES += ./network/src/TCPClientService.cxx
SOURCES += ./network/src/TCPConnClientSocket.cxx
SOURCES += ./network/src/TCPConnServerSocket.cxx
SOURCES += ./network/src/TCPConnectionSocket.cxx
SOURCES += ./network/src/TCPServerSocket.cxx
SOURCES += ./network/src/TCPService.cxx
SOURCES += ./network/src/UDPRead.cxx
SOURCES += ./network/src/UDPWrite.cxx

# --- network/http ---

HEADERS += ./network/http/inc/Directory.h
HEADERS += ./network/http/inc/HTTPClient.h
HEADERS += ./network/http/inc/HTTPClientService.h
HEADERS += ./network/http/inc/HTTPConnection.h
HEADERS += ./network/http/inc/HTTPDLL.h
HEADERS += ./network/http/inc/HTTPServer.h
HEADERS += ./network/http/inc/HTTPService.h
HEADERS += ./network/http/inc/HTTPUtilities.h
HEADERS += ./network/http/inc/Query.h
HEADERS += ./network/http/inc/ResponseCodes.h
HEADERS += ./network/http/inc/Unit.h

SOURCES += ./network/http/src/HTTPClient.cxx
SOURCES += ./network/http/src/HTTPClientService.cxx
SOURCES += ./network/http/src/HTTPConnection.cxx
SOURCES += ./network/http/src/HTTPServer.cxx
SOURCES += ./network/http/src/HTTPService.cxx
SOURCES += ./network/http/src/HTTPUtilities.cxx
SOURCES += ./network/http/src/Query.cxx
SOURCES += ./network/http/src/ResponseCodes.cxx
SOURCES += ./network/http/src/Unit.cxx

# --- network/ftp ---

HEADERS += ./network/ftp/inc/FTPConfiguration.h
HEADERS += ./network/ftp/inc/FTPDLL.h
HEADERS += ./network/ftp/inc/FTPServer.h
HEADERS += ./network/ftp/inc/FTPService.h
HEADERS += ./network/ftp/inc/FTPSession.h
HEADERS += ./network/ftp/inc/FTPTransfer.h

SOURCES += ./network/ftp/src/FTPConfiguration.cxx
SOURCES += ./network/ftp/src/FTPServer.cxx
SOURCES += ./network/ftp/src/FTPService.cxx
SOURCES += ./network/ftp/src/FTPSession.cxx
SOURCES += ./network/ftp/src/FTPTransfer.cxx

# --- network/mime ---

HEADERS += ./network/mime/inc/MimeForm.h
HEADERS += ./network/mime/inc/MimeHeader.h
HEADERS += ./network/mime/inc/MimeHeaderItem.h
HEADERS += ./network/mime/inc/MimeNetworkDLL.h
HEADERS += ./network/mime/inc/MimeSection.h

SOURCES += ./network/mime/src/MimeForm.cxx
SOURCES += ./network/mime/src/MimeHeader.cxx
SOURCES += ./network/mime/src/MimeHeaderItem.cxx
SOURCES += ./network/mime/src/MimeSection.cxx

# --- network/rtp ---

HEADERS += ./network/rtp/inc/DataPacket.h
HEADERS += ./network/rtp/inc/NetworkRTPDLL.h
HEADERS += ./network/rtp/inc/RTCPPacket.h
HEADERS += ./network/rtp/inc/RTCPPacketAPP.h
HEADERS += ./network/rtp/inc/RTCPPacketBYE.h
HEADERS += ./network/rtp/inc/RTCPPacketRR.h
HEADERS += ./network/rtp/inc/RTCPPacketSDES.h
HEADERS += ./network/rtp/inc/RTCPPacketSR.h
HEADERS += ./network/rtp/inc/RTCPReportBlock.h
HEADERS += ./network/rtp/inc/RTPPacket.h
HEADERS += ./network/rtp/inc/Scheduler.h
HEADERS += ./network/rtp/inc/Session.h
HEADERS += ./network/rtp/inc/SourceState.h

SOURCES += ./network/rtp/src/DataPacket.cxx
SOURCES += ./network/rtp/src/RTCPPacket.cxx
SOURCES += ./network/rtp/src/RTCPPacketAPP.cxx
SOURCES += ./network/rtp/src/RTCPPacketBYE.cxx
SOURCES += ./network/rtp/src/RTCPPacketRR.cxx
SOURCES += ./network/rtp/src/RTCPPacketSDES.cxx
SOURCES += ./network/rtp/src/RTCPPacketSR.cxx
SOURCES += ./network/rtp/src/RTCPReportBlock.cxx
SOURCES += ./network/rtp/src/RTPPacket.cxx
SOURCES += ./network/rtp/src/Scheduler.cxx
SOURCES += ./network/rtp/src/Session.cxx
SOURCES += ./network/rtp/src/SourceState.cxx

# --- network/rtp/silveromega ---

HEADERS += ./network/rtp/silveromega/inc/RTPSilverOmegaDLL.h
HEADERS += ./network/rtp/silveromega/inc/SilverRTPCodec.h
HEADERS += ./network/rtp/silveromega/inc/SilverSession.h
HEADERS += ./network/rtp/silveromega/inc/VOggPackets.h
HEADERS += ./network/rtp/silveromega/inc/VTimeCalc.h

SOURCES += ./network/rtp/silveromega/src/SilverRTPCodec.cxx
SOURCES += ./network/rtp/silveromega/src/SilverSession.cxx
SOURCES += ./network/rtp/silveromega/src/VOggPackets.cxx
SOURCES += ./network/rtp/silveromega/src/VTimeCalc.cxx

# --- audioio ---

HEADERS += ./audioio/inc/AbstractAudioHardwareBuffer.h
HEADERS += ./audioio/inc/AOBase.h
HEADERS += ./audioio/inc/AudioInfo.h
HEADERS += ./audioio/inc/AudioIODLL.h
HEADERS += ./audioio/inc/AOQueryDevice.h
HEADERS += ./audioio/inc/AudioSettings.h
HEADERS += ./audioio/inc/FormatDescription.h
HEADERS += ./audioio/inc/FormatDescriptionUtils.h
HEADERS += ./audioio/inc/FormatsSupported.h
HEADERS += ./audioio/inc/IOTimeStamp.h
HEADERS += ./audioio/inc/SampleConverter.h

SOURCES += ./audioio/src/AbstractAudioHardwareBuffer.cxx
SOURCES += ./audioio/src/AOBase.cxx
SOURCES += ./audioio/src/AudioInfo.cxx
SOURCES += ./audioio/src/AOChannelMap.cxx
SOURCES += ./audioio/src/AOQueryDevice.cxx
SOURCES += ./audioio/src/AudioSettings.cxx
SOURCES += ./audioio/src/FormatDescription.cxx
SOURCES += ./audioio/src/FormatDescriptionUtils.cxx
SOURCES += ./audioio/src/FormatsSupported.cxx
SOURCES += ./audioio/src/IOTimeStamp.cxx
SOURCES += ./audioio/src/SampleConverter.cxx

ios
{
	HEADERS += ./audioio/inc/AOCoreAudioIOS.h
	HEADERS += ./audioio/inc/AOQueryCoreAudio.h
	HEADERS += ./audioio/inc/AOQueryCoreAudioIOS.h
	HEADERS += ./audioio/inc/AOCoreAudioSessionIOS.h
	HEADERS += ./audioio/inc/AudioHardwareBufferCoreAudioIOS.h
	OBJECTIVE_SOURCES += ./audioio/src/AOCoreAudio.mm
	OBJECTIVE_SOURCES += ./audioio/src/AOCoreAudioIOS.mm
	OBJECTIVE_SOURCES += ./audioio/src/AOQueryCoreAudioIOS.mm
	OBJECTIVE_SOURCES += ./audioio/src/AOCoreAudioSessionIOS.mm
	OBJECTIVE_SOURCES += ./audioio/src/AudioHardwareBufferCoreAudioIOS.mm
}

# --- track/info ---

HEADERS += ./track/info/inc/AIFFInfo.h
HEADERS += ./track/info/inc/APEInfo.h
HEADERS += ./track/info/inc/APETag.h
HEADERS += ./track/info/inc/AtomInfo.h
HEADERS += ./track/info/inc/CueInfo.h
HEADERS += ./track/info/inc/CueParser.h
HEADERS += ./track/info/inc/DirInfo.h
HEADERS += ./track/info/inc/EmbeddedID3Info.h
HEADERS += ./track/info/inc/FLACInfo.h
HEADERS += ./track/info/inc/FileInfo.h
HEADERS += ./track/info/inc/ID3Info1.h
HEADERS += ./track/info/inc/ID3Info2.h
HEADERS += ./track/info/inc/Info.h
HEADERS += ./track/info/inc/InfoDLL.h
HEADERS += ./track/info/inc/MPCChapterTag.h
HEADERS += ./track/info/inc/VorbisInfo.h
HEADERS += ./track/info/inc/WavInfo.h
HEADERS += ./track/info/inc/XMCDInfo.h

SOURCES += ./track/info/src/AIFFInfo.cxx
SOURCES += ./track/info/src/APEInfo.cxx
SOURCES += ./track/info/src/APETag.cxx
SOURCES += ./track/info/src/AtomInfo.cxx
SOURCES += ./track/info/src/CueInfo.cxx
SOURCES += ./track/info/src/CueParser.cxx
SOURCES += ./track/info/src/DirInfo.cxx
SOURCES += ./track/info/src/EmbeddedID3Info.cxx
SOURCES += ./track/info/src/FLACInfo.cxx
SOURCES += ./track/info/src/FileInfo.cxx
SOURCES += ./track/info/src/ID3Info1.cxx
SOURCES += ./track/info/src/ID3Info2.cxx
SOURCES += ./track/info/src/Info.cxx
SOURCES += ./track/info/src/MPCChapterTag.cxx
SOURCES += ./track/info/src/VorbisInfo.cxx
SOURCES += ./track/info/src/WavInfo.cxx
SOURCES += ./track/info/src/XMCDInfo.cxx

# --- widget ---

HEADERS += ./widget/inc/SandBoxBase.h
HEADERS += ./widget/inc/SandBoxWindows.h
HEADERS += ./widget/inc/WidgetDLL.h

SOURCES += ./widget/src/SandBoxBase.cxx
SOURCES += ./widget/src/SandBoxWindows.cxx

# --- track/db ---

HEADERS += ./track/db/inc/DBInfo.h
HEADERS += ./track/db/inc/DbDLL.h
HEADERS += ./track/db/inc/SQLiteDatabase.h
HEADERS += ./track/db/inc/SQLiteException.h
HEADERS += ./track/db/inc/SQLiteInsert.h
HEADERS += ./track/db/inc/SQLiteLog.h
HEADERS += ./track/db/inc/SQLiteOperation.h
HEADERS += ./track/db/inc/SQLiteQuery.h
HEADERS += ./track/db/inc/Schema.h
HEADERS += ./track/db/inc/TrackDB.h
HEADERS += ./track/db/inc/TrackDBMountPoints.h
HEADERS += ./track/db/inc/TrackFileDependencies.h
HEADERS += ./track/db/inc/sqlite3.h

SOURCES += ./track/db/src/DBInfo.cxx
SOURCES += ./track/db/src/SQLiteDatabase.cxx
SOURCES += ./track/db/src/SQLiteException.cxx
SOURCES += ./track/db/src/SQLiteInsert.cxx
SOURCES += ./track/db/src/SQLiteLog.cxx
SOURCES += ./track/db/src/SQLiteOperation.cxx
SOURCES += ./track/db/src/SQLiteQuery.cxx
SOURCES += ./track/db/src/Schema.cxx
SOURCES += ./track/db/src/TrackDB.cxx
SOURCES += ./track/db/src/TrackDBMountPoints.cxx
SOURCES += ./track/db/src/TrackFileDependencies.cxx
SOURCES += ./track/db/src/sqlite3.c

# --- track/model ---

HEADERS += ./track/model/inc/AbstractTrackModel.h
HEADERS += ./track/model/inc/AlbumModel.h
HEADERS += ./track/model/inc/AlbumModelKey.h
HEADERS += ./track/model/inc/AlbumModelUtilities.h
HEADERS += ./track/model/inc/AlbumTrackModel.h
HEADERS += ./track/model/inc/ImageRepositary.h
HEADERS += ./track/model/inc/TrackModelDLL.h
HEADERS += ./track/model/inc/TrackModelKey.h

SOURCES += ./track/model/src/AbstractTrackModel.cxx
SOURCES += ./track/model/src/AlbumModel.cxx
SOURCES += ./track/model/src/AlbumModelKey.cxx
SOURCES += ./track/model/src/AlbumModelUtilities.cxx
SOURCES += ./track/model/src/AlbumTrackModel.cxx
SOURCES += ./track/model/src/ImageRepositary.cxx
SOURCES += ./track/model/src/TrackModelKey.cxx

# --- playerapp/playercommon ---

HEADERS += ./playerapp/playercommon/inc/OmegaAudioInterface.h
HEADERS += ./playerapp/playercommon/inc/OmegaPlaylistInterface.h
HEADERS += ./playerapp/playercommon/inc/PlayerCommonDLL.h
HEADERS += ./playerapp/playercommon/inc/PlaybackStateController.h
HEADERS += ./playerapp/playercommon/inc/OmegaPlaylistInterface.h
HEADERS += ./playerapp/playercommon/inc/PlaybackStateController.h
HEADERS += ./playerapp/playercommon/inc/PlayListModel.h
HEADERS += ./playerapp/playercommon/inc/QAlbumListModel.h
HEADERS += ./playerapp/playercommon/inc/QAlbumTrackListModel.h
HEADERS += ./playerapp/playercommon/inc/QOmegaListModel.h
HEADERS += ./playerapp/playercommon/inc/QAlbumImageProvider.h

SOURCES += ./playerapp/playercommon/src/OmegaAudioInterface.cxx
SOURCES += ./playerapp/playercommon/src/PlaybackStateController.cxx
SOURCES += ./playerapp/playercommon/src/OmegaPlaylistInterface.cxx
SOURCES += ./playerapp/playercommon/src/PlaybackStateController.cxx
SOURCES += ./playerapp/playercommon/src/PlayListModel.cxx
SOURCES += ./playerapp/playercommon/src/QAlbumListModel.cxx
SOURCES += ./playerapp/playercommon/src/QAlbumTrackListModel.cxx
SOURCES += ./playerapp/playercommon/src/QOmegaListModel.cxx
SOURCES += ./playerapp/playercommon/src/QAlbumImageProvider.cxx

# --- playerapp/playerios ---

HEADERS += ./playerapp/playerios/inc/PlayerAudioIOInterface.h
HEADERS += ./playerapp/playerios/inc/PlayerIOSTrackDBManager.h
HEADERS += ./playerapp/playerios/inc/PlayerIOSUtils.h
HEADERS += ./playerapp/playerios/inc/PlayerUISettings.h
HEADERS += ./playerapp/playerios/inc/PlayListIOSModel.h

SOURCES += ./playerapp/playerios/src/PlayerAudioIOInterface.cxx
SOURCES += ./playerapp/playerios/src/PlayerIOSMain.cxx
SOURCES += ./playerapp/playerios/src/PlayerIOSTrackDBManager.cxx
SOURCES += ./playerapp/playerios/src/PlayerIOSUtils.cxx
SOURCES += ./playerapp/playerios/src/PlayerUISettings.cxx
SOURCES += ./playerapp/playerios/src/PlayListIOSModel.cxx

ios
{
	HEADERS += ./playerapp/playerios/inc/PlayerIOSAudioSession.h
	OBJECTIVE_SOURCES += ./playerapp/playerios/src/PlayerIOSAudioSession.mm
}

RESOURCES = ./playerapp/playerios/playerios.qrc

ios
{
	LIBS += -framework MediaPlayer
	LIBS += -framework AVFoundation
	QMAKE_INFO_PLIST = ./playerapp/playerios/Info.plist
}
