cd %CD% \..\..\bin\x64\Release
set binariesDir=%CD%
set qtDir=E:\Qt\5.14.1\msvc2017_64\bin
set drive=%qtDir:~0,2%
mkdir "%binariesDir%\client"
mkdir "%binariesDir%\server"
copy "%binariesDir%\OpenFTP Client.exe" "%binariesDir%\client\OpenFTP Client.exe" && copy "%binariesDir%\OpenFTP Server.exe" "%binariesDir%\server\OpenFTP Server.exe" 

%drive% && cd %qtDir%


windeployqt "%binariesDir%\client\OpenFTP Client.exe" --release --plugindir "%binariesDir%\client\plugins" -core -gui -widgets -multimedia --no-multimediawidgets -network --no-translations  --no-angle --no-opengl-sw --no-bluetooth --no-concurrent --no-declarative --no-designer --no-designercomponents --no-enginio --no-gamepad --no-qthelp  --no-nfc --no-opengl --no-positioning --no-printsupport --no-qml --no-qmltooling --no-quick --no-quickparticles --no-quickwidgets --no-script --no-scripttools --no-sensors --no-serialport --no-sql --no-svg --no-test --no-webkit --no-webkit2 --no-webkitwidgets --no-websockets --no-winextras --no-xml --no-xmlpatterns --no-webenginecore --no-webengine --no-webenginewidgets --no-3dcore --no-3drenderer --no-3dquick --no-3dquickrenderer --no-3dinput --no-3danimation --no-3dextras --no-geoservices --no-webchannel --no-texttospeech --no-serialbus --no-webview
windeployqt "%binariesDir%\server\OpenFTP Server.exe"  --release --force --plugindir "%binariesDir%\server\plugins" -core -gui -widgets -network --no-translations --no-multimediawidgets --no-angle --no-opengl-sw  --no-bluetooth --no-concurrent --no-declarative --no-designer --no-designercomponents --no-enginio --no-gamepad --no-qthelp --no-multimedia --no-nfc --no-opengl --no-positioning --no-printsupport --no-qml --no-qmltooling --no-quick --no-quickparticles --no-quickwidgets --no-script --no-scripttools --no-sensors --no-serialport --no-sql --no-svg --no-test --no-webkit --no-webkit2 --no-webkitwidgets --no-websockets --no-winextras --no-xml --no-xmlpatterns --no-webenginecore --no-webengine --no-3dcore --no-webenginewidgets --no-3drenderer --no-3dquick --no-3dquickrenderer --no-3dinput --no-3danimation --no-3dextras --no-geoservices --no-webchannel --no-texttospeech --no-serialbus --no-webview


cmd /k
