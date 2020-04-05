#include <QtWidgets>
#include <QtNetwork>
#include <openssl/aes.h>

#define APP_VERSION "0.3.4" 

#ifdef QT_DEBUG
#define CRYPTO_KEY 0x3cdd48c744c19481 //*** Randomize this on each new release version
#define CERTIFICATES_DIR ":/certificates-debug/"
#else 
#define CERTIFICATES_DIR ":/certificates-release/"
#endif

