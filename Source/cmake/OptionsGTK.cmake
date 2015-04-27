include(GNUInstallDirs)

set(PROJECT_VERSION_MAJOR 2)
set(PROJECT_VERSION_MINOR 7)
set(PROJECT_VERSION_MICRO 4)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_MICRO})
set(WEBKITGTK_API_VERSION 4.0)

# Libtool library version, not to be confused with API version.
# See http://www.gnu.org/software/libtool/manual/html_node/Libtool-versioning.html
CALCULATE_LIBRARY_VERSIONS_FROM_LIBTOOL_TRIPLE(WEBKIT2 42 0 5)
CALCULATE_LIBRARY_VERSIONS_FROM_LIBTOOL_TRIPLE(JAVASCRIPTCORE 19 3 1)

set(ENABLE_GTKDOC OFF CACHE BOOL "Whether or not to use generate gtkdoc.")
set(ENABLE_X11_TARGET ON CACHE BOOL "Whether to enable support for the X11 windowing target.")
set(ENABLE_WAYLAND_TARGET OFF CACHE BOOL "Whether to enable support for the Wayland windowing target.")
set(ENABLE_INTROSPECTION ON CACHE BOOL "Whether to enable GObject introspection.")
set(ENABLE_GLES2 OFF CACHE BOOL "Whether to enable OpenGL ES 2.0.")

# These are shared variables, but we special case their definition so that we can use the
# CMAKE_INSTALL_* variables that are populated by the GNUInstallDirs macro.
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBDIR}" CACHE PATH "Absolute path to library installation directory")
set(EXEC_INSTALL_DIR "${CMAKE_INSTALL_FULL_BINDIR}" CACHE PATH "Absolute path to executable installation directory")
set(LIBEXEC_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBEXECDIR}/webkit2gtk-${WEBKITGTK_API_VERSION}" CACHE PATH "Absolute path to install executables executed by the library")

set(DATA_BUILD_DIR "${CMAKE_BINARY_DIR}/share/${WebKit_OUTPUT_NAME}")
set(DATA_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/webkitgtk-${WEBKITGTK_API_VERSION}")
set(WEBKITGTK_HEADER_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/webkitgtk-${WEBKITGTK_API_VERSION}")
set(INTROSPECTION_INSTALL_GIRDIR "${CMAKE_INSTALL_FULL_DATADIR}/gir-1.0")
set(INTROSPECTION_INSTALL_TYPELIBDIR "${LIB_INSTALL_DIR}/girepository-1.0")

find_package(Cairo 1.10.2 REQUIRED)
find_package(Fontconfig 2.8.0 REQUIRED)
find_package(Freetype2 2.4.2 REQUIRED)
find_package(HarfBuzz 0.9.2 REQUIRED)
find_package(ICU REQUIRED)
find_package(JPEG REQUIRED)
find_package(LibSoup 2.42.0 REQUIRED)
find_package(LibXml2 2.8.0 REQUIRED)
find_package(LibXslt 1.1.7 REQUIRED)
find_package(PNG REQUIRED)
find_package(Sqlite REQUIRED)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(ATK REQUIRED)
find_package(WebP REQUIRED)
find_package(ATSPI 2.5.3)
find_package(GObjectIntrospection)
find_package(EGL)
find_package(GeoClue2 2.1.5)
find_package(GnuTLS 3.0.0)
find_package(LibNotify)

if (NOT GEOCLUE2_FOUND)
    find_package(GeoClue)
endif ()

if (ENABLE_GLES2)
    find_package(OpenGLES2 REQUIRED)
    if (OPENGLES2_FOUND AND NOT(EGL_FOUND))
        message(FATAL_ERROR "EGL is needed for OpenGL ES 2.0.")
    endif ()
else ()
    find_package(OpenGL)
endif ()

if (ENABLE_X11_TARGET)
    # With cmake 3.2.x we have to explicitly ask for X11 otherwise the X11_X11_LIB
    # variable won't be set thus the X11 linker flags won't be added and the build
    # will fail.
    find_package(X11 REQUIRED)
    if (OPENGL_FOUND)
        # We don't use find_package for GLX because it is part of -lGL, unlike EGL.
        check_include_files("GL/glx.h" GLX_FOUND)
    endif ()
endif ()

if (ENABLE_WAYLAND_TARGET)
    find_package(Wayland REQUIRED)
endif ()

WEBKIT_OPTION_BEGIN()
WEBKIT_OPTION_DEFINE(ENABLE_PLUGIN_PROCESS_GTK2 "Whether to build WebKitPluginProcess2 to load GTK2 based plugins." PUBLIC ON)
WEBKIT_OPTION_DEFINE(USE_REDIRECTED_XCOMPOSITE_WINDOW "Whether to use a Redirected XComposite Window for accelerated compositing in X11." PRIVATE ON)

if ((OPENGL_FOUND OR OPENGLES2_FOUND) AND (GLX_FOUND OR EGL_FOUND))
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_WEBGL PRIVATE ON)

    if (ENABLE_X11_TARGET)
        WEBKIT_OPTION_DEFAULT_PORT_VALUE(USE_REDIRECTED_XCOMPOSITE_WINDOW PRIVATE ON)
    else ()
        WEBKIT_OPTION_DEFAULT_PORT_VALUE(USE_REDIRECTED_XCOMPOSITE_WINDOW PRIVATE OFF)
    endif ()

    if (OPENGL_FOUND)
        if (GLX_FOUND)
            list(APPEND CAIRO_GL_COMPONENTS cairo-glx)
        endif ()
        if (EGL_FOUND)
            list(APPEND CAIRO_GL_COMPONENTS cairo-egl)
        endif ()
        find_package(CairoGL 1.10.2 COMPONENTS ${CAIRO_GL_COMPONENTS})
        if (CAIRO_GL_FOUND)
            WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_ACCELERATED_2D_CANVAS PRIVATE ON)
        else ()
            WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_ACCELERATED_2D_CANVAS PRIVATE OFF)
        endif ()
    else ()
        # FIXME: Should we search for cairo-glesv2 instead of cairo-gl in this case?
        WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_ACCELERATED_2D_CANVAS PRIVATE OFF)
    endif ()
else ()
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_WEBGL PRIVATE OFF)
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_ACCELERATED_2D_CANVAS PRIVATE OFF)
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(USE_REDIRECTED_XCOMPOSITE_WINDOW PRIVATE OFF)
endif ()

if (ENABLE_ACCELERATED_2D_CANVAS AND NOT(CAIRO_GL_FOUND))
    message(FATAL_ERROR "cairo-gl is needed for Accelerated 2D Canvas support")
endif ()

if (GNUTLS_FOUND)
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_SUBTLE_CRYPTO PUBLIC ON)
else ()
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_SUBTLE_CRYPTO PUBLIC OFF)
endif ()

if (ENABLE_SUBTLE_CRYPTO AND NOT(GNUTLS_FOUND))
    message(FATAL_ERROR "Subtle crypto support needs GnuTLS 3.0.0")
endif ()

if (GEOCLUE2_FOUND OR GEOCLUE_FOUND)
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_GEOLOCATION PUBLIC ON)
else ()
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_GEOLOCATION PUBLIC OFF)
endif ()

# FIXME: There is no reason these should be different.
SET_AND_EXPOSE_TO_BUILD(DEVELOPMENT_BUILD ${DEVELOPER_MODE})
if (DEVELOPER_MODE)
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_MINIBROWSER PUBLIC ON)
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_API_TESTS PUBLIC ON)
else ()
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_MINIBROWSER PUBLIC OFF)
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_API_TESTS PUBLIC OFF)
    set(WebKit2_VERSION_SCRIPT "-Wl,--version-script,${CMAKE_MODULE_PATH}/gtksymbols.filter")
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_MEMORY_SAMPLER PUBLIC ON)
else ()
    WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_MEMORY_SAMPLER PUBLIC OFF)
endif ()

WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_CREDENTIAL_STORAGE PUBLIC ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_DRAG_SUPPORT PUBLIC ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_ICONDATABASE PUBLIC ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_JIT PUBLIC ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_SPELLCHECK PUBLIC ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_TOUCH_EVENTS PUBLIC ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_VIDEO PUBLIC ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_WEB_AUDIO PUBLIC ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(USE_SYSTEM_MALLOC PUBLIC OFF)

WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_3D_TRANSFORMS PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_ACCESSIBILITY PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_ATTACHMENT_ELEMENT PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_BATTERY_STATUS PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_CANVAS_PATH PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_CSS3_TEXT PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_CSS_DEVICE_ADAPTATION PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_CSS_GRID_LAYOUT PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_CSS_IMAGE_SET PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_CSS_REGIONS PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_CSS_SELECTORS_LEVEL4 PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_CUSTOM_SCHEME_HANDLER PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_DATALIST_ELEMENT PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_DOWNLOAD_ATTRIBUTE PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_ENCRYPTED_MEDIA PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_ENCRYPTED_MEDIA_V2 PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_FILTERS_LEVEL_2 PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_FTPDIR PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_FULLSCREEN_API PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_GAMEPAD PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_GAMEPAD_DEPRECATED PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_INDEXED_DATABASE PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_INDEXED_DATABASE_IN_WORKERS PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_INPUT_TYPE_COLOR PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_LEGACY_WEB_AUDIO PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_LINK_PREFETCH PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_MEDIA_CAPTURE PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_MEDIA_CONTROLS_SCRIPT PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_MHTML PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_NAVIGATOR_CONTENT_UTILS PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_NAVIGATOR_HWCONCURRENCY PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_NETSCAPE_PLUGIN_API PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_NOTIFICATIONS PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_PICTURE_SIZES PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_QUOTA PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_RESOLUTION_MEDIA_QUERY PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_REQUEST_ANIMATION_FRAME PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_SECCOMP_FILTERS PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_SMOOTH_SCROLLING PUBLIC ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_TEMPLATE_ELEMENT PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_USERSELECT_ALL PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_USER_MESSAGE_HANDLERS PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_VIBRATION PRIVATE OFF)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_VIDEO_TRACK PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_VIEW_MODE_CSS_MEDIA PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_WEB_TIMING PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_XHR_TIMEOUT PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_NETWORK_PROCESS PRIVATE ON)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(ENABLE_THREADED_COMPOSITOR PRIVATE OFF)
WEBKIT_OPTION_END()

if (ENABLE_PLUGIN_PROCESS_GTK2)
    find_package(GTK2 2.24.10 REQUIRED)
    find_package(GDK2 2.24.10 REQUIRED)
endif ()

SET_AND_EXPOSE_TO_BUILD(WTF_USE_LIBNOTIFY ${LIBNOTIFY_FOUND})
SET_AND_EXPOSE_TO_BUILD(WTF_PLATFORM_X11 ${ENABLE_X11_TARGET})
SET_AND_EXPOSE_TO_BUILD(MOZ_X11 ${ENABLE_X11_TARGET})
SET_AND_EXPOSE_TO_BUILD(XP_UNIX ${WTF_OS_UNIX})
SET_AND_EXPOSE_TO_BUILD(WTF_PLATFORM_WAYLAND ${ENABLE_WAYLAND_TARGET})

set(ENABLE_WEBKIT OFF)
set(ENABLE_WEBKIT2 ON)
set(ENABLE_PLUGIN_PROCESS ON)

set(GDK_VERSION_MIN_REQUIRED GDK_VERSION_3_6)
set(GTK_API_VERSION 3.0)

set(WTF_USE_SOUP 1)
set(WTF_USE_UDIS86 1)

set(WTF_LIBRARY_TYPE STATIC)
set(WTF_OUTPUT_NAME WTFGTK)
set(JavaScriptCore_OUTPUT_NAME javascriptcoregtk-${WEBKITGTK_API_VERSION})
set(WebCore_OUTPUT_NAME WebCoreGTK)
set(WebKit_OUTPUT_NAME webkitgtk-${WEBKITGTK_API_VERSION})
set(WebKit2_OUTPUT_NAME webkit2gtk-${WEBKITGTK_API_VERSION})
set(WebKit2_WebProcess_OUTPUT_NAME WebKitWebProcess)
set(WebKit2_NetworkProcess_OUTPUT_NAME WebKitNetworkProcess)
set(WebKit2_PluginProcess_OUTPUT_NAME WebKitPluginProcess)
set(WebKit2_DatabaseProcess_OUTPUT_NAME WebKitDatabaseProcess)

add_definitions(-DBUILDING_GTK__=1)
add_definitions(-DGETTEXT_PACKAGE="WebKit2GTK-${WEBKITGTK_API_VERSION}")
add_definitions(-DDATA_DIR="${CMAKE_INSTALL_DATADIR}")
add_definitions(-DUSER_AGENT_GTK_MAJOR_VERSION=601)
add_definitions(-DUSER_AGENT_GTK_MINOR_VERSION=1)
add_definitions(-DWEBKITGTK_API_VERSION_STRING="${WEBKITGTK_API_VERSION}")

if (ENABLE_VIDEO OR ENABLE_WEB_AUDIO)
    set(GSTREAMER_COMPONENTS app pbutils)
    SET_AND_EXPOSE_TO_BUILD(WTF_USE_GSTREAMER TRUE)
    if (ENABLE_VIDEO)
        list(APPEND GSTREAMER_COMPONENTS video mpegts tag gl)
    endif ()

    if (ENABLE_WEB_AUDIO)
        list(APPEND GSTREAMER_COMPONENTS audio fft)
        SET_AND_EXPOSE_TO_BUILD(WTF_USE_WEBAUDIO_GSTREAMER TRUE)
    endif ()

    find_package(GStreamer 1.0.3 REQUIRED COMPONENTS ${GSTREAMER_COMPONENTS})

    if (PC_GSTREAMER_MPEGTS_FOUND)
        SET_AND_EXPOSE_TO_BUILD(WTF_USE_GSTREAMER_MPEGTS TRUE)
    endif ()
endif ()

if (ENABLE_WAYLAND_TARGET)
    set(GTK3_REQUIRED_VERSION 3.12.0)
else ()
    set(GTK3_REQUIRED_VERSION 3.6.0)
endif ()

find_package(GTK3 ${GTK3_REQUIRED_VERSION} REQUIRED)
find_package(GDK3 ${GTK3_REQUIRED_VERSION} REQUIRED)
set(GTK_LIBRARIES ${GTK3_LIBRARIES})
set(GTK_INCLUDE_DIRS ${GTK3_INCLUDE_DIRS})
set(GDK_LIBRARIES ${GDK3_LIBRARIES})
set(GDK_INCLUDE_DIRS ${GDK3_INCLUDE_DIRS})

set(glib_components gio gobject gthread gmodule)
if (ENABLE_GAMEPAD_DEPRECATED OR ENABLE_GEOLOCATION)
    list(APPEND glib_components gio-unix)
endif ()
find_package(GLIB 2.33.2 REQUIRED COMPONENTS ${glib_components})

if (ENABLE_GEOLOCATION)
    SET_AND_EXPOSE_TO_BUILD(WTF_USE_GEOCLUE2 ${GEOCLUE2_FOUND})
endif ()

if (ENABLE_MEDIA_STREAM)
    find_package(OpenWebRTC)
    SET_AND_EXPOSE_TO_BUILD(WTF_USE_OPENWEBRTC ${OPENWEBRTC_FOUND})
endif ()

find_package(GTKUnixPrint)
SET_AND_EXPOSE_TO_BUILD(HAVE_GTK_UNIX_PRINTING ${GTK_UNIX_PRINT_FOUND})

if (ENABLE_CREDENTIAL_STORAGE)
    find_package(Libsecret REQUIRED)
endif ()

SET_AND_EXPOSE_TO_BUILD(HAVE_GTK_GESTURES ${GTK_SUPPORTS_GESTURES})

# This part can be simplified once CMake 2.8.6 is required and
# CMakePushCheckState can be used. We need to have OPENGL_INCLUDE_DIR as part
# of the directories check_include_files() looks for in case OpenGL is
# installed into a non-standard location.
if (ENABLE_X11_TARGET)
    set(REQUIRED_INCLUDES_OLD ${CMAKE_REQUIRED_INCLUDES})
    set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${OPENGL_INCLUDE_DIR})
    set(CMAKE_REQUIRED_INCLUDES ${REQUIRED_INCLUDES_OLD})
endif ()

if (ENABLE_SPELLCHECK)
    find_package(Enchant REQUIRED)
endif ()

if ((OPENGL_FOUND OR OPENGLES2_FOUND) AND (GLX_FOUND OR EGL_FOUND))
    set(ENABLE_TEXTURE_MAPPER 1)

    SET_AND_EXPOSE_TO_BUILD(WTF_USE_3D_GRAPHICS TRUE)
    SET_AND_EXPOSE_TO_BUILD(WTF_USE_TEXTURE_MAPPER TRUE)
    SET_AND_EXPOSE_TO_BUILD(WTF_USE_TEXTURE_MAPPER_GL TRUE)
    set(ENABLE_3D_RENDERING ON) # This is already exposed by WEBKIT_OPTION_DEFINE.

    if (ENABLE_X11_TARGET AND USE_REDIRECTED_XCOMPOSITE_WINDOW)
        add_definitions(-DWTF_USE_REDIRECTED_XCOMPOSITE_WINDOW=1)
    endif ()

    if (OPENGLES2_FOUND)
        SET_AND_EXPOSE_TO_BUILD(WTF_USE_OPENGL_ES_2 TRUE)
    else ()
        SET_AND_EXPOSE_TO_BUILD(WTF_USE_OPENGL TRUE)
    endif ()

    SET_AND_EXPOSE_TO_BUILD(WTF_USE_EGL ${EGL_FOUND})
    SET_AND_EXPOSE_TO_BUILD(WTF_USE_GLX ${GLX_FOUND})

    SET_AND_EXPOSE_TO_BUILD(WTF_USE_TILED_BACKING_STORE ${ENABLE_THREADED_COMPOSITOR})
    SET_AND_EXPOSE_TO_BUILD(WTF_USE_COORDINATED_GRAPHICS ${ENABLE_THREADED_COMPOSITOR})
    SET_AND_EXPOSE_TO_BUILD(WTF_USE_COORDINATED_GRAPHICS_THREADED ${ENABLE_THREADED_COMPOSITOR})

    SET_AND_EXPOSE_TO_BUILD(WTF_USE_GSTREAMER_GL ${PC_GSTREAMER_GL_FOUND})
endif ()

if (ENABLE_GAMEPAD_DEPRECATED)
    find_package(GUdev)
endif ()

if (ENABLE_FTL_JIT)
    find_package(LLVM REQUIRED)
    SET_AND_EXPOSE_TO_BUILD(HAVE_LLVM TRUE)
endif ()

set(CPACK_SOURCE_GENERATOR TBZ2)

set(DERIVED_SOURCES_GOBJECT_DOM_BINDINGS_DIR ${DERIVED_SOURCES_DIR}/webkitdom)
set(DERIVED_SOURCES_WEBKITGTK_DIR ${DERIVED_SOURCES_DIR}/webkitgtk)
set(DERIVED_SOURCES_WEBKITGTK_API_DIR ${DERIVED_SOURCES_WEBKITGTK_DIR}/webkit)
set(DERIVED_SOURCES_GOBJECT_DOM_BINDINGS_DIR ${DERIVED_SOURCES_DIR}/webkitdom)
set(DERIVED_SOURCES_WEBKIT2GTK_DIR ${DERIVED_SOURCES_DIR}/webkit2gtk)
set(DERIVED_SOURCES_WEBKIT2GTK_API_DIR ${DERIVED_SOURCES_WEBKIT2GTK_DIR}/webkit2)
set(FORWARDING_HEADERS_DIR ${DERIVED_SOURCES_DIR}/ForwardingHeaders)
set(FORWARDING_HEADERS_WEBKIT2GTK_DIR ${FORWARDING_HEADERS_DIR}/webkit2gtk)
set(FORWARDING_HEADERS_WEBKIT2GTK_EXTENSION_DIR ${FORWARDING_HEADERS_DIR}/webkit2gtk-webextension)

set(WebKit_PKGCONFIG_FILE ${CMAKE_BINARY_DIR}/Source/WebKit/gtk/webkitgtk-${WEBKITGTK_API_VERSION}.pc)
set(WebKit2_PKGCONFIG_FILE ${CMAKE_BINARY_DIR}/Source/WebKit2/webkit2gtk-${WEBKITGTK_API_VERSION}.pc)
set(WebKit2WebExtension_PKGCONFIG_FILE ${CMAKE_BINARY_DIR}/Source/WebKit2/webkit2gtk-web-extension-${WEBKITGTK_API_VERSION}.pc)

set(SHOULD_INSTALL_JS_SHELL ON)

# Add a typelib file to the list of all typelib dependencies. This makes it easy to
# expose a 'gir' target with all gobject-introspection files.
macro(ADD_TYPELIB typelib)
    if (ENABLE_INTROSPECTION)
        get_filename_component(target_name ${typelib} NAME_WE)
        add_custom_target(${target_name}-gir ALL DEPENDS ${typelib})
        list(APPEND GObjectIntrospectionTargets ${target_name}-gir)
        set(GObjectIntrospectionTargets ${GObjectIntrospectionTargets} PARENT_SCOPE)
    endif ()
endmacro()

# CMake does not automatically add --whole-archive when building shared objects from
# a list of convenience libraries. This can lead to missing symbols in the final output.
# We add --whole-archive to all libraries manually to prevent the linker from trimming
# symbols that we actually need later.
macro(ADD_WHOLE_ARCHIVE_TO_LIBRARIES _list_name)
    foreach (library IN LISTS ${_list_name})
      list(APPEND ${_list_name}_TMP -Wl,--whole-archive ${library} -Wl,--no-whole-archive)
    endforeach ()
    set(${_list_name} "${${_list_name}_TMP}")
endmacro()

if (CMAKE_MAJOR_VERSION LESS 3)
    # Before CMake 3 it was necessary to use a build script instead of using cmake --build directly
    # to preserve colors and pretty-printing.

    build_command(COMMAND_LINE_TO_BUILD)
    # build_command unconditionally adds -i (ignore errors) for make, and there's
    # no reasonable way to turn that off, so we just replace it with -k, which has
    # the same effect, except that the return code will indicate that an error occurred.
    # See: http://www.cmake.org/cmake/help/v3.0/command/build_command.html
    string(REPLACE " -i" " -k" COMMAND_LINE_TO_BUILD ${COMMAND_LINE_TO_BUILD})
    file(WRITE
        ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/build.sh
        "#!/bin/sh\n"
        "${COMMAND_LINE_TO_BUILD} $@"
    )
    file(COPY ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/build.sh
        DESTINATION ${CMAKE_BINARY_DIR}
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
    )
endif ()
