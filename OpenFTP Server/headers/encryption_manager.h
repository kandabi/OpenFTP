#pragma once
#include "stdafx.h"
#include <inttypes.h>
#include <string>
#include <algorithm>
#include <openssl/aes.h>
#include <openssl/rand.h>


class encryptionManager : public QObject
{
    Q_OBJECT
public:
    encryptionManager(QObject* parent = Q_NULLPTR);

    QString encrypt(const QString& text);
    QString decrypt(const QString& stringToDecrypt);

private:
    uint8_t Key[32];
    uint8_t IV[AES_BLOCK_SIZE]; // Generate an AES Key
    uint8_t IVd[AES_BLOCK_SIZE];
};
