#include <QtWidgets>
#include <QTcpSocket>
#include <QTcpServer>
#include <qnetworkinterface>

#define APP_VERSION "0.3.0" 

#ifdef QT_DEBUG
#define CRYPTO_KEY 0x3cdd48c744c19481 //*** Randomize this on each new release version
#endif

