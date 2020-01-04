
# How to deploy

Copy and paste the following lines into windeployqt using cmd in order to complete the deployment process of the .exe files created after compiling the solution.
this process is necessary so anyone could run OpenFTP.exe without having Qt installed on their system, the following command includes all of the necessary dll's for the client and the server to run correctly.



## client:

windeployqt E:\Projects\ftpProject\0.2.0\bin\x64\Release\client\OpenFTP.exe --release --plugindir E:\Projects\ftpProject\0.2.0\bin\x64\Release\client\plugins -core -gui -widgets -multimedia -multimediawidgets -network --no-translations  --no-angle --no-opengl-sw --no-bluetooth --no-concurrent --no-declarative --no-designer --no-designercomponents --no-enginio --no-gamepad --no-qthelp  --no-nfc --no-opengl --no-positioning --no-printsupport --no-qml --no-qmltooling --no-quick --no-quickparticles --no-quickwidgets --no-script --no-scripttools --no-sensors --no-serialport --no-sql --no-svg --no-test --no-webkit --no-webkit2 --no-webkitwidgets --no-websockets --no-winextras --no-xml --no-xmlpatterns --no-webenginecore --no-webengine --no-webenginewidgets --no-3dcore --no-3drenderer --no-3dquick --no-3dquickrenderer --no-3dinput --no-3danimation --no-3dextras --no-geoservices --no-webchannel --no-texttospeech --no-serialbus --no-webview




-----------------------------------------------------------------------------------------------


## server:

windeployqt E:\Projects\ftpProject\0.2.0\bin\x64\Release\server --release --force --plugindir E:\Projects\ftpProject\0.2.0\bin\x64\Release\server\plugins -core -gui -widgets -network --no-translations --no-multimediawidgets --no-angle --no-opengl-sw  --no-bluetooth --no-concurrent --no-declarative --no-designer --no-designercomponents --no-enginio --no-gamepad --no-qthelp --no-multimedia --no-nfc --no-opengl --no-positioning --no-printsupport --no-qml --no-qmltooling --no-quick --no-quickparticles --no-quickwidgets --no-script --no-scripttools --no-sensors --no-serialport --no-sql --no-svg --no-test --no-webkit --no-webkit2 --no-webkitwidgets --no-websockets --no-winextras --no-xml --no-xmlpatterns --no-webenginecore --no-webengine --no-webenginewidgets --no-3dcore --no-3drenderer --no-3dquick --no-3dquickrenderer --no-3dinput --no-3danimation --no-3dextras --no-geoservices --no-webchannel --no-texttospeech --no-serialbus --no-webview

