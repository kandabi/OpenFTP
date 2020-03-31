#include <QtWidgets>
#include <QtNetwork>
#include <QSound>

#define APP_VERSION "0.3.1" 

//*** Replace debug OPENSSL certificates with release version!!

#ifdef QT_DEBUG 
#define CRYPTO_KEY 0xb2f641dc9fffec4e //*** Randomize this on each new release version
#define STYLE_DIR "./resources/"
#else 
#define STYLE_DIR "./plugins/styles/"
#endif
