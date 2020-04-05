#include <QtWidgets>
#include <QtNetwork>
#include <QSound>

#define APP_VERSION "0.3.4" 

#ifdef QT_DEBUG 
#define CRYPTO_KEY 0xb2f641dc9fffec4e
#define STYLE_DIR "./resources/"
#define CERTIFICATES_DIR ":/certificates-debug/"
#else 
#define STYLE_DIR "./plugins/styles/"
#define CERTIFICATES_DIR ":/certificates-release/"
#endif
