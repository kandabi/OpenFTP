#include <QtWidgets>
#include <QtNetwork>
#include <utility>
#include <QSound>

#define APP_VERSION "0.3.0" 

#ifdef QT_DEBUG 
#define CRYPTO_KEY 0xb2f641dc9fffec4e //*** Randomize this on each new release version
#endif
