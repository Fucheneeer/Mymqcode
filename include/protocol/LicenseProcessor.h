////////////////////////////////////////////////////////////////////////////////
//
// LicenseProcessor.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// CFMQ is the copyrighted,
// proprietary property of CFIT which retain all right, title and interest therein.
//
// Description: License Generator
//
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "BrokerConstant.h"
#include "ModuleManager.h"
#include "Properties.h"
#include "ISymmetricEncryptionEngine.h"
#include "ICryptoFactory.h"

using namespace dep;
using namespace lightmq::qmapi;
using namespace lightmq::broker;

namespace lightmq { namespace protocol {

class LicenseProcessor{
public:
    LicenseProcessor(const std::string& encryptionLib);

    ~LicenseProcessor();

    std::string getLicenseKeys(){
        return std::string(BrokerConstant::LICENSE_VERSION) + std::string(BrokerConstant::LICENSE_CUSTOMER) + std::string(BrokerConstant::LICENSE_CONTRACTNUMBER) + std::string(BrokerConstant::LICENSE_PRODUCTNAME) + std::string(BrokerConstant::LICENSE_AUTHORIZEDBODIES) + std::string(BrokerConstant::LICENSE_EXPIRATIONDATE) + std::string(BrokerConstant::LICENSE_EQUIPMENTSERIALNUMBER) + std::string(BrokerConstant::LICENSE_LICENSESERIALNUMBER) + std::string(BrokerConstant::LICENSE_LICENSEAUTHORIZATIONCODE);
    }

    void setEncryptionEngineKey(const std::string& encryptionEngineKey){
        m_encryptionEngineKey = encryptionEngineKey;
    }

    void encrypt(std::string& input, std::string& output);

    void decrypt(std::string& input, std::string& output);

    std::string generateLicense(const std::map<std::string, std::string>& licenseMap);

    void VerifyLicense();

    void printLicenseInfo(std::map<std::string, std::string>& licenseContextMap);

#ifdef SUPPORT_SADK
    void createEncryptKey(char*& pszBase64EncryptedData);
#else
    void createEncryptKey(std::string& pszBase64EncryptedData);
#endif
private:
    const std::string IMPL_ENCRYPTION_LIB{ "sm4" };
    const std::string KEY_ENCRYPTION{ "EncryptionKey" };
    const std::string LICENSE_DEFAULT_NAME{ "license.dat" };
private:
    ModuleManager<ICryptoFactory> m_moduleManager;
    ICryptoFactory* m_pCryptoFactory;
    Properties m_cryptoProperties;
    std::string m_encryptionEngineKey;
    std::unique_ptr<ISymmetricEncryptionEngine> m_pEncryptionEngine;
#ifdef SUPPORT_SADK
    char* m_pszSymKeyData;
#else
    std::string m_pszSymKeyData;
#endif
};

}} // namespace lightmq { namespace protocol {