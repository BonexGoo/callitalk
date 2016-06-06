TARGET = callitalk
TEMPLATE = app
QT += core
QT += gui
QT += network
QT += opengl
QT += widgets
win32: QT += bluetooth
win32: RC_ICONS += ../common/main.ico

CONFIG += c++11
CONFIG += mobility
CONFIG += warn_off
MOBILITY += systeminfo

INCLUDEPATH += ../../Bx2D/addon
INCLUDEPATH += ../../Bx2D/core
INCLUDEPATH += ../../Bx2D/lib
INCLUDEPATH += ../../Bx2D/skin

DEFINES += __BX_APPLICATION
DEFINES += __BX_OPENGL
win32: DEFINES += __BX_MEMORY
debug: DEFINES += __BX_DEBUG
macx: DEFINES += __BX_USED_LONG_SIZET
ios: DEFINES += __BX_USED_LONG_SIZET

win32: DEFINES += CT_SIZEBASE=50
android: DEFINES += CT_SIZEBASE=100
SOURCES += ../source/callitalk_door.cpp
SOURCES += ../source/callitalk_room.cpp
SOURCES += ../source/callitalk_server.cpp
SOURCES += ../source/callitalk_testmode.cpp

###########################################################
# platyvg
###########################################################
INCLUDEPATH += ../../PlatyVG/source/api
DEFINES += PX_DEBUG
SOURCES += ../../PlatyVG/source/api/adapter.cpp
SOURCES += ../../PlatyVG/source/api/command.cpp
SOURCES += ../../PlatyVG/source/api/export.cpp
SOURCES += ../../PlatyVG/source/api/input.cpp
#SOURCES += ../../PlatyVG/source/api/listener.cpp
#SOURCES += ../../PlatyVG/source/api/platyvg.cpp
SOURCES += ../../PlatyVG/source/core/device.cpp
SOURCES += ../../PlatyVG/source/core/document.cpp
SOURCES += ../../PlatyVG/source/core/layer.cpp
SOURCES += ../../PlatyVG/source/core/record.cpp
SOURCES += ../../PlatyVG/source/core/shape.cpp
SOURCES += ../../PlatyVG/source/core/view.cpp
SOURCES += ../../PlatyVG/source/element/action.cpp
SOURCES += ../../PlatyVG/source/element/color.cpp
SOURCES += ../../PlatyVG/source/element/context.cpp
SOURCES += ../../PlatyVG/source/element/coord.cpp
SOURCES += ../../PlatyVG/source/element/edge.cpp
SOURCES += ../../PlatyVG/source/element/matrix.cpp
SOURCES += ../../PlatyVG/source/element/mesh.cpp
SOURCES += ../../PlatyVG/source/element/point.cpp
SOURCES += ../../PlatyVG/source/element/rect.cpp
SOURCES += ../../PlatyVG/source/element/selector.cpp
SOURCES += ../../PlatyVG/source/element/size.cpp
SOURCES += ../../PlatyVG/source/element/table.cpp
SOURCES += ../../PlatyVG/source/element/zoom.cpp
SOURCES += ../../PlatyVG/source/px/array.cpp
SOURCES += ../../PlatyVG/source/px/buffer.cpp
SOURCES += ../../PlatyVG/source/px/file.cpp
SOURCES += ../../PlatyVG/source/px/math.cpp
SOURCES += ../../PlatyVG/source/px/memory.cpp
SOURCES += ../../PlatyVG/source/px/parser.cpp
SOURCES += ../../PlatyVG/source/px/px.cpp
SOURCES += ../../PlatyVG/source/px/share.cpp
SOURCES += ../../PlatyVG/source/px/string.cpp
HEADERS += ../../PlatyVG/source/api/adapter.hpp
HEADERS += ../../PlatyVG/source/api/command.hpp
HEADERS += ../../PlatyVG/source/api/export.hpp
HEADERS += ../../PlatyVG/source/api/input.hpp
HEADERS += ../../PlatyVG/source/api/listener.hpp
HEADERS += ../../PlatyVG/source/api/platyvg.hpp
HEADERS += ../../PlatyVG/source/core/device.hpp
HEADERS += ../../PlatyVG/source/core/document.hpp
HEADERS += ../../PlatyVG/source/core/layer.hpp
HEADERS += ../../PlatyVG/source/core/record.hpp
HEADERS += ../../PlatyVG/source/core/shape.hpp
HEADERS += ../../PlatyVG/source/core/view.hpp
HEADERS += ../../PlatyVG/source/element/action.hpp
HEADERS += ../../PlatyVG/source/element/color.hpp
HEADERS += ../../PlatyVG/source/element/context.hpp
HEADERS += ../../PlatyVG/source/element/coord.hpp
HEADERS += ../../PlatyVG/source/element/edge.hpp
HEADERS += ../../PlatyVG/source/element/matrix.hpp
HEADERS += ../../PlatyVG/source/element/mesh.hpp
HEADERS += ../../PlatyVG/source/element/point.hpp
HEADERS += ../../PlatyVG/source/element/rect.hpp
HEADERS += ../../PlatyVG/source/element/selector.hpp
HEADERS += ../../PlatyVG/source/element/size.hpp
HEADERS += ../../PlatyVG/source/element/table.hpp
HEADERS += ../../PlatyVG/source/element/zoom.hpp
HEADERS += ../../PlatyVG/source/px/array.hpp
HEADERS += ../../PlatyVG/source/px/buffer.hpp
HEADERS += ../../PlatyVG/source/px/file.hpp
HEADERS += ../../PlatyVG/source/px/math.hpp
HEADERS += ../../PlatyVG/source/px/memory.hpp
HEADERS += ../../PlatyVG/source/px/parser.hpp
HEADERS += ../../PlatyVG/source/px/px.hpp
HEADERS += ../../PlatyVG/source/px/share.hpp
HEADERS += ../../PlatyVG/source/px/string.hpp

###########################################################
# Bx2D
###########################################################
SOURCES += ../../Bx2D/addon/BxAddOn_GIF.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_HQX.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_JPG.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_MD5.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_TTF.cpp
SOURCES += ../../Bx2D/addon/BxAddOn_ZIP.cpp
SOURCES += ../../Bx2D/core/BxCoreForQT5.cpp
SOURCES += ../../Bx2D/core/BxCoreImpl.cpp
SOURCES += ../../Bx2D/skin/BxIME.cpp
win32|macx: SOURCES += ../../Bx2D/skin/BxProjectHelper.cpp
win32|macx: SOURCES += ../../Bx2D/skin/BxSimulator.cpp
SOURCES += ../../Bx2D/skin/BxStartPage.cpp
SOURCES += ../../Bx2D/skin/ime/IMEBoard.cpp
SOURCES += ../../Bx2D/skin/ime/IMEButton.cpp
SOURCES += ../../Bx2D/skin/ime/IMEEdit.cpp
SOURCES += ../../Bx2D/skin/ime/IMESysButton.cpp
SOURCES += ../../Bx2D/skin/ime/IMETip.cpp
HEADERS += ../../Bx2D/core/BxCore.hpp
HEADERS += ../../Bx2D/core/BxCoreImpl.hpp
HEADERS += ../../Bx2D/core/BxCoreForQT5.hpp
HEADERS += ../../Bx2D/lib/BxAnimate.hpp
HEADERS += ../../Bx2D/lib/BxArgument.hpp
HEADERS += ../../Bx2D/lib/BxAutoBuffer.hpp
HEADERS += ../../Bx2D/lib/BxConsole.hpp
HEADERS += ../../Bx2D/lib/BxDraw.hpp
HEADERS += ../../Bx2D/lib/BxDynamic.hpp
HEADERS += ../../Bx2D/lib/BxExpress.hpp
HEADERS += ../../Bx2D/lib/BxImage.hpp
HEADERS += ../../Bx2D/lib/BxKeyword.hpp
HEADERS += ../../Bx2D/lib/BxMemory.hpp
HEADERS += ../../Bx2D/lib/BxPanel.hpp
HEADERS += ../../Bx2D/lib/BxPathFind.hpp
HEADERS += ../../Bx2D/lib/BxPool.hpp
HEADERS += ../../Bx2D/lib/BxScene.hpp
HEADERS += ../../Bx2D/lib/BxSingleton.hpp
HEADERS += ../../Bx2D/lib/BxString.hpp
HEADERS += ../../Bx2D/lib/BxTween.hpp
HEADERS += ../../Bx2D/lib/BxType.hpp
HEADERS += ../../Bx2D/lib/BxUtil.hpp
HEADERS += ../../Bx2D/lib/BxVar.hpp
HEADERS += ../../Bx2D/lib/BxVarMap.hpp
HEADERS += ../../Bx2D/lib/BxVarQueue.hpp
HEADERS += ../../Bx2D/lib/BxVarVector.hpp
HEADERS += ../../Bx2D/lib/BxWebContent.hpp
HEADERS += ../../Bx2D/lib/BxXml.hpp

ASSETS_CONFIG.files += ../resource/assets/config
ASSETS_CONFIG.path = /assets
INSTALLS += ASSETS_CONFIG

ASSETS_IMAGE.files += ../resource/assets/image
ASSETS_IMAGE.path = /assets
INSTALLS += ASSETS_IMAGE

ASSETS_SYS.files += ../../Bx2D/assets/sys
ASSETS_SYS.path = /assets
INSTALLS += ASSETS_SYS

android{
	ANDROID_PACKAGE_SOURCE_DIR = $$PWD/../resource/android
}

ios|macx{
	QMAKE_BUNDLE_DATA += ASSETS_CONFIG
	QMAKE_BUNDLE_DATA += ASSETS_IMAGE
	QMAKE_BUNDLE_DATA += ASSETS_SYS
}
