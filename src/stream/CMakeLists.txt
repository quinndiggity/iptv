SET(GLOBAL_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/constants.h
  ${CMAKE_SOURCE_DIR}/src/stream/gst_types.h
  ${CMAKE_SOURCE_DIR}/src/stream/stypes.h

  ${CMAKE_SOURCE_DIR}/src/stream/ilinker.h

  ${CMAKE_SOURCE_DIR}/src/stream/ibase_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/ibase_builder_observer.h
  ${CMAKE_SOURCE_DIR}/src/stream/ibase_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/probes.h
  ${CMAKE_SOURCE_DIR}/src/stream/timeshift.h
  ${CMAKE_SOURCE_DIR}/src/stream/process_wrapper.h

  ${CMAKE_SOURCE_DIR}/src/stream/main_wrapper.h
  ${CMAKE_SOURCE_DIR}/src/stream/gstreamer_utils.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams_factory.h
  ${CMAKE_SOURCE_DIR}/src/stream/configs_factory.h
  ${CMAKE_SOURCE_DIR}/src/stream/config.h
)
SET(GLOBAL_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/constants.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/gst_types.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/stypes.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/ilinker.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/ibase_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/ibase_builder_observer.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/ibase_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/probes.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/timeshift.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/process_wrapper.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/main_wrapper.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/gstreamer_utils.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams_factory.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/configs_factory.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/config.cpp
)

SET(STREAM_CONFIGS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/relay_config.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/encoding_config.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/audio_video_config.h
)

SET(STREAM_CONFIGS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/relay_config.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/encoding_config.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/configs/audio_video_config.cpp
)

SET(STREAM_BUILDERS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/gst_base_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/mosaic_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/screen_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/src_decodebin_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/relay_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/timeshift_player_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/timeshift_recorder_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/catchup_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/playlist_relay_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding_only_audio_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding_only_video_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/test_stream_builder.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/fake_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/device_stream_builder.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/playlist_encoding_stream_builder.h
)
SET(STREAM_BUILDERS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/gst_base_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/mosaic_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/screen_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/src_decodebin_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/relay_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/timeshift_player_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/timeshift_recorder_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/catchup_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/playlist_relay_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding_only_audio_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/encoding_only_video_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/test_stream_builder.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/fake_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/device_stream_builder.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/builders/playlist_encoding_stream_builder.cpp
)

SET(STREAMS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/streams/mosaic_options.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/mosaic_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/screen_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/src_decodebin_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/relay_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift_player_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/itimeshift_recorder_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift_recorder_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/catchup_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/playlist_relay_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding_only_audio_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding_only_video_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/test_stream.h

  ${CMAKE_SOURCE_DIR}/src/stream/streams/fake_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/device_stream.h
  ${CMAKE_SOURCE_DIR}/src/stream/streams/playlist_encoding_stream.h

  ${STREAM_BUILDERS_HEADERS}
  ${STREAM_CONFIGS_HEADERS}
)
SET(STREAMS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/streams/mosaic_options.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/mosaic_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/screen_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/src_decodebin_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/relay_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift_player_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/itimeshift_recorder_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/timeshift_recorder_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/catchup_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/playlist_relay_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding_only_audio_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/encoding_only_video_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/test_stream.cpp

  ${CMAKE_SOURCE_DIR}/src/stream/streams/fake_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/device_stream.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/streams/playlist_encoding_stream.cpp

  ${STREAM_BUILDERS_SOURCES}
  ${STREAM_CONFIGS_SOURCES}
)

SET(SOURCES_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/element.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/multifilesrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/httpsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/appsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/rtmpsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/udpsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/tcpsrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/v4l2src.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/alsasrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/filesrc.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/build_input.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/sources.h
)
SET(SOURCES_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/element.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/multifilesrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/httpsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/appsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/rtmpsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/udpsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/tcpsrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/v4l2src.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/alsasrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/filesrc.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/build_input.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sources/sources.cpp
)

SET(SINKS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/rtmp.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/udp.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/tcp.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/http.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/fake.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/screen.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/build_output.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/sink.h
)
SET(SINKS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/rtmp.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/udp.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/tcp.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/http.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/fake.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/screen.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/build_output.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/sink/sink.cpp
)

SET(ENCODERS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/encoders/audio_encoders.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/encoders/video_encoders.h
)
SET(ENCODERS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/encoders/audio_encoders.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/encoders/video_encoders.cpp
)

SET(PARSER_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/parser/video_parsers.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/parser/audio_parsers.h
)
SET(PARSER_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/parser/video_parsers.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/parser/audio_parsers.cpp
)

SET(PAY_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/pay.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/video_pay.h
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/audio_pay.h
)
SET(PAY_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/pay.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/video_pay.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/elements/pay/audio_pay.cpp
)

SET(VIDEO_HEADERS ${CMAKE_SOURCE_DIR}/src/stream/elements/video/video.h)
SET(VIDEO_SOURCES ${CMAKE_SOURCE_DIR}/src/stream/elements/video/video.cpp)

SET(AUDIO_HEADERS ${CMAKE_SOURCE_DIR}/src/stream/elements/audio/audio.h)
SET(AUDIO_SOURCES ${CMAKE_SOURCE_DIR}/src/stream/elements/audio/audio.cpp)

SET(MUXER_HEADERS ${CMAKE_SOURCE_DIR}/src/stream/elements/muxer/muxer.h)
SET(MUXER_SOURCES ${CMAKE_SOURCE_DIR}/src/stream/elements/muxer/muxer.cpp)

SET(ELEMENTS_HEADERS
  ${MUXER_HEADERS}
  ${PAY_HEADERS}
  ${PARSER_HEADERS}
  ${ENCODERS_HEADERS}
  ${SINKS_HEADERS}
  ${SOURCES_HEADERS}
  ${VIDEO_HEADERS}
  ${AUDIO_HEADERS}
)
SET(ELEMENTS_SOURCES
  ${MUXER_SOURCES}
  ${PAY_SOURCES}
  ${PARSER_SOURCES}
  ${ENCODERS_SOURCES}
  ${SINKS_SOURCES}
  ${SOURCES_SOURCES}
  ${VIDEO_SOURCES}
  ${AUDIO_SOURCES}
)

SET(PAD_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/pad/pad.h
)
SET(PAD_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/pad/pad.cpp
)

SET(DUMPERS_HEADERS
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/idumper.h
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/htmldump.h
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/dumpers_factory.h
)
SET(DUMPERS_SOURCES
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/htmldump.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/dumpers_factory.cpp
  ${CMAKE_SOURCE_DIR}/src/stream/dumpers/idumper.cpp
)

FIND_PACKAGE(GLIB REQUIRED gobject)
FIND_PACKAGE(Gstreamer 1.11.1 REQUIRED)
FIND_PACKAGE(Cairo REQUIRED)

IF(OS_WINDOWS)
  SET(PLATFORM_HEADER)
  SET(PLATFORM_SOURCES)
  SET(PLATFORM_LIBRARIES)
ELSEIF(OS_POSIX)
  SET(PLATFORM_HEADER)
  SET(PLATFORM_SOURCES)
  SET(PLATFORM_LIBRARIES pthread)
  FIND_PACKAGE(X11 REQUIRED)
    IF(X11_FOUND)
      SET(PLATFORM_LIBRARIES ${PLATFORM_LIBRARIES} ${X11_LIBRARIES})
      SET(DEPENDENS_INCLUDE_DIRS ${DEPENDENS_INCLUDE_DIRS} ${X11_INCLUDE_DIR})
      ADD_DEFINITIONS(-DHAVE_X11)
  ENDIF(X11_FOUND)
ENDIF(OS_WINDOWS)

SET(CLIENT_SOURCES
  ${GLOBAL_HEADERS} ${GLOBAL_SOURCES}
  ${STREAMS_HEADERS} ${STREAMS_SOURCES}
  ${ELEMENTS_HEADERS} ${ELEMENTS_SOURCES}
  ${PAD_HEADERS} ${PAD_SOURCES}
  ${PLATFORM_HEADER} ${PLATFORM_SOURCES}
  ${DUMPERS_HEADERS} ${DUMPERS_SOURCES}
)

SET(CLIENT_LIBRARIES
  ${CLIENT_LIBRARIES}
  ${PLATFORM_LIBRARIES}
  ${GLIB_LIBRARIES} ${GLIB_GOBJECT_LIBRARIES}
  ${GSTREAMER_LIBRARIES} ${GSTREAMER_APP_LIBRARY}
  ${CAIRO_LIBRARIES}
  ${COMMON_LIBRARIES}
  ${STREAMER_COMMON}
)

SET(PRIVATE_COMPILE_DEFINITIONS_CORE
  ${PRIVATE_COMPILE_DEFINITIONS_CORE}
)
SET(PRIVATE_INCLUDE_DIRECTORIES_CORE
  ${PRIVATE_INCLUDE_DIRECTORIES_CORE}
  ${CMAKE_SOURCE_DIR}/src
  ${GSTREAMER_INCLUDE_DIR}
  ${GLIB_INCLUDE_DIR}
  ${GLIBCONFIG_INCLUDE_DIR}
  ${CAIRO_INCLUDE_DIRS}
  ${DEPENDENS_INCLUDE_DIRS}
  ${JSONC_INCLUDE_DIRS}
  ${COMMON_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/src/third-party/sds
)

ADD_LIBRARY(${STREAMER_CORE} SHARED ${CLIENT_SOURCES} ${SDS_SOURCES})
TARGET_INCLUDE_DIRECTORIES(${STREAMER_CORE} PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES_CORE})
TARGET_COMPILE_DEFINITIONS(${STREAMER_CORE} PRIVATE ${PRIVATE_COMPILE_DEFINITIONS_CORE})
TARGET_LINK_LIBRARIES(${STREAMER_CORE} ${CLIENT_LIBRARIES})

IF(PROJECT_BUILD_TYPE_VERSION STREQUAL "release")
  STRIP_TARGET(${STREAMER_CORE})
ENDIF(PROJECT_BUILD_TYPE_VERSION STREQUAL "release")
INSTALL(TARGETS ${STREAMER_CORE} DESTINATION ${LIB_INSTALL_DESTINATION} COMPONENT APPLICATIONS)

IF (DEVELOPER_CHECK_STYLE)
  SET(CHECK_SOURCES
    ${CLIENT_SOURCES}
  )
  REGISTER_CHECK_STYLE_TARGET(check_style_${STREAMER_CORE} "${CHECK_SOURCES}")
  REGISTER_CHECK_INCLUDES_TARGET(${STREAMER_CORE})
ENDIF(DEVELOPER_CHECK_STYLE)

IF(DEVELOPER_ENABLE_TESTS)
  FIND_PACKAGE(GTest REQUIRED)

  ## Unit tests
  SET(PRIVATE_INCLUDE_DIRECTORIES_UNIT_TESTS
    ${PRIVATE_INCLUDE_DIRECTORIES_UNIT_TESTS}
    ${CMAKE_SOURCE_DIR}/src
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIR}
    ${GLIBCONFIG_INCLUDE_DIR}
  )
  SET(UNIT_TESTS_LIBS
      ${GTEST_BOTH_LIBRARIES}
      ${STREAMER_COMMON}
      ${PLATFORM_LIBRARIES} ${STREAMER_CORE})
  SET(UNIT_TESTS unit_tests_stream)
  ADD_EXECUTABLE(${UNIT_TESTS}
    ${CMAKE_SOURCE_DIR}/tests/stream/unit_test_types.cpp
    ${CMAKE_SOURCE_DIR}/tests/stream/unit_test_api.cpp
  )
  TARGET_INCLUDE_DIRECTORIES(${UNIT_TESTS} PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES_UNIT_TESTS} ${JSONC_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(${UNIT_TESTS} ${UNIT_TESTS_LIBS})
  ADD_TEST_TARGET(${UNIT_TESTS})
  SET_PROPERTY(TARGET ${UNIT_TESTS} PROPERTY FOLDER "Unit tests")

  ## Mock tests
  SET(PRIVATE_INCLUDE_DIRECTORIES_MOCK_TESTS
    ${PRIVATE_INCLUDE_DIRECTORIES_MOCK_TESTS}
    ${CMAKE_SOURCE_DIR}/src
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIR}
    ${GLIBCONFIG_INCLUDE_DIR}
  )
  SET(GMOCK_TESTS_LIBS ${GTEST_BOTH_LIBRARIES} gmock ${STREAMER_CORE})
  ADD_EXECUTABLE(gmock_tests
    ${CMAKE_SOURCE_DIR}/tests/stream/mock_test_job.cpp
  )
  TARGET_INCLUDE_DIRECTORIES(gmock_tests PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES_MOCK_TESTS})
  TARGET_LINK_LIBRARIES(gmock_tests ${GMOCK_TESTS_LIBS})
  ADD_TEST_TARGET(gmock_tests)
  SET_PROPERTY(TARGET gmock_tests PROPERTY FOLDER "Mock tests")

  ## Workflow tests
  SET(PRIVATE_INCLUDE_DIRECTORIES_WORKFLOW_TESTS
    ${PRIVATE_INCLUDE_DIRECTORIES_WORKFLOW_TESTS}
    ${CMAKE_SOURCE_DIR}/src
    ${GSTREAMER_INCLUDE_DIR}
    ${GLIB_INCLUDE_DIR}
    ${GLIBCONFIG_INCLUDE_DIR}
  )
  SET(WORKFLOW_TESTS_LIBS ${STREAMER_CORE})
  ADD_EXECUTABLE(workflow_tests
    ${CMAKE_SOURCE_DIR}/tests/stream/workflow_test_restarts.cpp
  )
  TARGET_INCLUDE_DIRECTORIES(workflow_tests PRIVATE ${PRIVATE_INCLUDE_DIRECTORIES_WORKFLOW_TESTS})
  TARGET_LINK_LIBRARIES(workflow_tests ${WORKFLOW_TESTS_LIBS})
  TARGET_COMPILE_DEFINITIONS(workflow_tests PRIVATE -DPROJECT_TEST_SOURCES_DIR="${CMAKE_SOURCE_DIR}/tests")
  SET_PROPERTY(TARGET workflow_tests PROPERTY FOLDER "Workflow tests")
ENDIF(DEVELOPER_ENABLE_TESTS)
