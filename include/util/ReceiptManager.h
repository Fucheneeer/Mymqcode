////////////////////////////////////////////////////////////////////////////////
//
// ReceiptManager.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description:
//
////////////////////////////////////////////////////////////////////////////////
#ifndef RECEIPTMANAGER_H_
#define RECEIPTMANAGER_H_
#include <atomic>
#include <memory>
#include <unordered_map>
#include <condition_variable>
#include "MsgChunk.h"
#include "ExpireService.h"

namespace lightmq { namespace util {

class ReceiptManager;

struct IReceiptListener: public ExpirableObject {
	IReceiptListener()
		: ExpirableObject(std::bind(&IReceiptListener::callReceiptTimeout, this))
	{}

	virtual ~IReceiptListener() = default;

	virtual void onReceiptMessage(std::unique_ptr<protocol::IMessage> &&msg) = 0;
	virtual void onReceiptTimeout() = 0;
	virtual void disMiss() = 0;
	virtual ReceiptManager *getListenManager() = 0;
	virtual protocol::ReceiptID getListenReceiptID() = 0;

	//virtual void lockReceiptListener() = 0;
	//virtual void unlockReceiptListener() = 0;

//  BasicLockable for unique_lock
	virtual void lock() = 0;
	virtual void unlock() = 0;

	virtual bool isReceiptOnce() { return true; }

	const std::shared_ptr<IReceiptListener> pretendAsSharedListener() {
		return std::shared_ptr<IReceiptListener>(this, [](IReceiptListener *lis) { });
	}

private:
	void callReceiptTimeout() {
		disMiss();
		onReceiptTimeout();
	}
};

class ReceiptListenerBase: public IReceiptListener {
public:
	virtual ReceiptManager *getListenManager() override { return m_receiptManager; }
	virtual protocol::ReceiptID getListenReceiptID() override { return m_receiptID; }

protected:
	ReceiptListenerBase();
	virtual ~ReceiptListenerBase();

	virtual void disMiss() override;
	virtual void lock() override { }
	virtual void unlock() override { }

public:
	void enterReceiptManager(ReceiptManager *receiptMana, protocol::ReceiptID rid) {
		m_receiptManager = receiptMana;
		m_receiptID = rid;
	}

	void leaveReceiptManager(ReceiptManager *receiptMana, protocol::ReceiptID rid) {
		(void)receiptMana;
		(void)rid;
		m_receiptManager = nullptr;
		m_receiptID = 0;
	}

private:
	ReceiptManager *m_receiptManager;
	protocol::ReceiptID m_receiptID;
};

struct ReceiptListenerFunc: public ReceiptListenerBase {
	typedef void ListenerFuncType (std::unique_ptr<protocol::IMessage> &&);
	typedef void TimeoutFuncType ();
	std::function<ListenerFuncType> m_listenFunc;
	std::function<TimeoutFuncType> m_timeoutFunc;

	ReceiptListenerFunc(std::function<ListenerFuncType> && f, std::function<TimeoutFuncType> && tf)
		: m_listenFunc(std::move(f))
		, m_timeoutFunc(std::move(tf))
	{}

//	ReceiptListenerFunc(const std::function<ListenerFuncType> & f, const std::function<TimeoutFuncType> & tf)
//			: m_listenFunc(f)
//			, m_timeoutFunc(tf)
//	{}


	virtual void onReceiptMessage(std::unique_ptr<protocol::IMessage> &&msg) override
	{
		m_listenFunc(std::move(msg));
	}

	virtual void onReceiptTimeout() override
	{
		m_timeoutFunc();
	}
};

class ReceiptIDGenerator {
public:
	ReceiptIDGenerator();
	~ReceiptIDGenerator();

	protocol::ReceiptID generateReceiptID();
	static protocol::ReceiptID transReceiptID16(uint32_t id16);

private:
	std::atomic<protocol::ReceiptID> m_receiptIDSeq;
};

class ReceiptManager {
	ReceiptManager(const ReceiptManager &) = delete;
	void operator=(const ReceiptManager &) = delete;
public:
	ReceiptManager();
	~ReceiptManager();

	bool initializeReceiptManager();
	void finalizeReceiptManager();

	bool registerReceipt(const std::shared_ptr<IReceiptListener> &lis, std::chrono::milliseconds timeout);
	bool registerReceipt(protocol::ReceiptID rid, const std::shared_ptr<IReceiptListener> &lis, std::chrono::milliseconds timeout);
	void unregisterReceipt(protocol::ReceiptID rid);

	bool onReceiptMessage(protocol::ReceiptID rid, std::unique_ptr<protocol::IMessage> &&msg);

	ReceiptIDGenerator &getReceiptIDGenerator() {
		return m_IDGenerateor;
	}

	void clearReceipts();

private:
	/*
	 * Demo:
	 *    ReceiptManager &rm = ...
	 *    std::shared_ptr<IReceiptListener> lis = rm.getListenerByReceiptID(rid);
	 *    if (lis) {
	 *        std::unique_lock<IReceiptListener> lock(*(lis.get()), std::adopt_lock);
	 *        ...
	 *    }
	 */
	std::shared_ptr<IReceiptListener> getListenerByReceiptID(protocol::ReceiptID rid);

	std::recursive_mutex m_mtxReceipts;
	std::unordered_map<protocol::ReceiptID, const std::shared_ptr<IReceiptListener>> m_receipts;

	util::ExpireService m_expireService;

	ReceiptIDGenerator m_IDGenerateor;
};


class FutureReceiptMessage: private ReceiptListenerBase {
public:
	FutureReceiptMessage();
	~FutureReceiptMessage();

	FutureReceiptMessage(ReceiptManager &rm, int timeoutMS)
	{
		listenInReceiptManager(rm, timeoutMS);
	}

public:  //  IReceiptListener
	void onReceiptMessage(std::unique_ptr<protocol::IMessage> &&msg) override;
	void onReceiptTimeout() override;

public:
	bool listenInReceiptManager(ReceiptManager &rm, int timeoutMS)
	{
		m_gotReceipt = false;
		m_receiptTimeout = false;
		m_waitTimeoutMS = timeoutMS;
		return rm.registerReceipt(pretendAsSharedListener(), std::chrono::milliseconds(timeoutMS));
	}

	using ReceiptListenerBase::getListenManager;
	using ReceiptListenerBase::getListenReceiptID;
	using ReceiptListenerBase::pretendAsSharedListener;
	//ReceiptManager *getListenManager() override { return m_receiptManager; }
	//protocol::ReceiptID getListenReceiptID() override { return m_receiptID; }

	bool waitReceipt();

public:
	//std::condition_variable &getWaiter() { return m_cv; }
	//std::mutex &getLock() { return m_lock; }

	protocol::IMessage *getReceiptMessage() { return m_receiptMessage.get(); }
	std::unique_ptr<protocol::IMessage> && moveReceiptMessage() {
		return static_cast<std::unique_ptr<protocol::IMessage>&&>(m_receiptMessage);
	}

//	const std::shared_ptr<IReceiptListener> & makeSharedListener() {
//		return std::shared_ptr<IReceiptListener>(static_cast<ReceiptListenerBase*>(this), [](IReceiptListener *lis) { });
//	}

	void lock() override {
		m_lock.lock();
	}

	void unlock() override {
		m_lock.unlock();
	}

private:
	std::mutex m_lock;
	std::condition_variable m_cv;
	bool m_gotReceipt{false};
	bool m_receiptTimeout{false};
	std::unique_ptr<protocol::IMessage> m_receiptMessage;
	int m_waitTimeoutMS {0};
};

}} // namespace lightmq { namespace util {

#endif /* RECEIPTMANAGER_H_ */
