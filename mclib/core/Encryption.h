#ifndef MCLIB_CORE_ENCRYPTION_H_
#define MCLIB_CORE_ENCRYPTION_H_



#include <mclib.h>
#include <protocol/packets/PacketsLogin.h>


class DataBuffer;


struct EncryptionStrategy
{
	virtual ~EncryptionStrategy() {}
	virtual DataBuffer Encrypt(const DataBuffer& buffer) = 0;
	virtual DataBuffer Decrypt(const DataBuffer& buffer) = 0;
};


class EncryptionStrategyNone
	: public EncryptionStrategy
{
public:
	DataBuffer MCLIB_API Encrypt(const DataBuffer& buffer);
	DataBuffer MCLIB_API Decrypt(const DataBuffer& buffer);
};


class EncryptionStrategyAES
	: public EncryptionStrategy
{
private:
	class Impl;
	Impl* m_Impl;

public:
	MCLIB_API EncryptionStrategyAES(const std::string& publicKey, const std::string& verifyToken);
	MCLIB_API ~EncryptionStrategyAES();

	EncryptionStrategyAES(const EncryptionStrategyAES& other) = delete;
	EncryptionStrategyAES& operator=(const EncryptionStrategyAES& other) = delete;
	EncryptionStrategyAES(EncryptionStrategyAES&& other) = delete;
	EncryptionStrategyAES& operator=(EncryptionStrategyAES&& other) = delete;

	DataBuffer MCLIB_API Encrypt(const DataBuffer& buffer);
	DataBuffer MCLIB_API Decrypt(const DataBuffer& buffer);

	std::string MCLIB_API GetSharedSecret() const;
	MCLIB_API login::out::EncryptionResponsePacket* GenerateResponsePacket() const;
};

#endif
