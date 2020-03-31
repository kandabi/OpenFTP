#include <QtWidgets>
#include <QtNetwork>

#define APP_VERSION "0.3.1" 

//*** Replace debug OPENSSL certificates with release version!!

#ifdef QT_DEBUG
#define CRYPTO_KEY 0x3cdd48c744c19481 //*** Randomize this on each new release version
#endif

