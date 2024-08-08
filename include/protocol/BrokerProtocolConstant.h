////////////////////////////////////////////////////////////////////////////////
//
// BrokerProtocolConstant.h
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

#ifndef BROKERPROTOCOLCONSTANT_H_
#define BROKERPROTOCOLCONSTANT_H_
#include <string>
#include <cstdint>
#include "BrokerTypes.h"

namespace lightmq { namespace protocol {

enum class TransactionFinalState: uint8_t {
	UKNOWED_TRANSACTION = 0,
	COMMITED = 1,
	ROLLBACKED = 2
};

inline std::string to_string(TransactionFinalState endState) {
	return endState == TransactionFinalState::COMMITED ? "COMMITED"
		:  endState == TransactionFinalState::ROLLBACKED ? "ROLLBACKED"
		:  endState == TransactionFinalState::UKNOWED_TRANSACTION ? "UKNOWED_TRANSACTION"
		: "UknowedDistributeTransEndState";
}

namespace BrokerProtocolConstant {
//  GCC can't generate class static const var reference.
//class BrokerProtocolConstant {
//public:
    // Message Type Definition
    // Connect and System framework
    static constexpr uint8_t MESSAGE_TYPE_CONNECT = 'c';
    static constexpr uint8_t MESSAGE_TYPE_CONNECT_ACK = 'C';
    static constexpr uint8_t MESSAGE_TYPE_CONNECT_PTO = 'g';
    static constexpr uint8_t MESSAGE_TYPE_CONNECT_ACK_PTO = 'G';
    static constexpr uint8_t MESSAGE_TYPE_ERROR = 'E';

    // Client Frame
    static constexpr uint8_t MESSAGE_TYPE_SEND = 'S';
    static constexpr uint8_t MESSAGE_TYPE_SUBSCRIBE = 's';
    static constexpr uint8_t MESSAGE_TYPE_UNSUBSCRIBE = 'u';
    static constexpr uint8_t MESSAGE_TYPE_SUBSCRIBE_PTO = 'n';
    static constexpr uint8_t MESSAGE_TYPE_MESSAGEPULL = 'p';

    static constexpr uint8_t MESSAGE_TYPE_BEGIN = 'B';
    static constexpr uint8_t MESSAGE_TYPE_ABORT = 'a';
    static constexpr uint8_t MESSAGE_TYPE_COMMIT = 't';
    static constexpr uint8_t MESSAGE_TYPE_DISCONNECT = 'D';
    static constexpr uint8_t MESSAGE_TYPE_QUERY_LAST_MESSAGE = 'L';
	static constexpr uint8_t MESSAGE_TYPE_RESEND_MESSAGE = 'F';
	static constexpr uint8_t MESSAGE_TYPE_RECEIPT_RESEND = 'x';
	static constexpr uint8_t MESSAGE_TYPE_CREATE = 'm';
	static constexpr uint8_t MESSAGE_TYPE_ACK = 'A';
    static constexpr uint8_t MESSAGE_TYPE_ACK_PTO = 'I';
	static constexpr uint8_t MESSAGE_TYPE_LINK_QUEUE_MESSAGE_ACK_PTO = 'K';
	//static constexpr uint8_t MESSAGE_TYPE_LOCAL_TRANSACT = 'O';
	static constexpr uint8_t MESSAGE_TYPE_TRANSACT_OPERATE = 'T';
	static constexpr uint8_t MESSAGE_TYPE_TRANSACT_QUERY = 'Q';
	static constexpr uint8_t MESSAGE_TYPE_SYNC_SESSION = 'N';
    static constexpr uint8_t MESSAGE_TYPE_QUERY_LAST_MESSAGE_PTO = 46;

    //Server Frame
    static constexpr uint8_t MESSAGE_TYPE_MESSAGE = 'M';
	static constexpr uint8_t MESSAGE_TYPE_LINK_QUEUE_MESSAGE = 'q';
	static constexpr uint8_t MESSAGE_TYPE_LINK_TOPIC_MESSAGE = 'p';
    static constexpr uint8_t MESSAGE_TYPE_RECEIPT = 'R';
    static constexpr uint8_t MESSAGE_TYPE_CREATE_RESPONSE = 'x';
    static constexpr uint8_t MESSAGE_TYPE_SUBSCRIBE_RESPONSE = 'y';
    static constexpr uint8_t MESSAGE_TYPE_UNSUBSCRIBE_RESPONSE = 'z';
    static constexpr uint8_t MESSAGE_TYPE_ACKSEND = 'b';
    static constexpr uint8_t MESSAGE_TYPE_RECEIPT_LAST_MESSAGE = 'l';
    static constexpr uint8_t MESSAGE_TYPE_DESTINATION_ATTRIBUTES = 'U';
    static constexpr uint8_t MESSAGE_TYPE_RECEIPT_LAST_MESSAGE_PTO = 47;

    // HA frame
    static constexpr uint8_t MESSAGE_TYPE_LINKTEST_REQUEST = 'H';
    static constexpr uint8_t MESSAGE_TYPE_LINKTEST_RESPONSE = 'h';
    static constexpr uint8_t MESSAGE_TYPE_LOGON = 'O';
    static constexpr uint8_t MESSAGE_TYPE_LOGON_PTO = 'J';
    static constexpr uint8_t MESSAGE_TYPE_LOGON_RESPONSE = 'o';
    static constexpr uint8_t MESSAGE_TYPE_LOGON_RESPONSE_PTO = 'j';
    static constexpr uint8_t MESSAGE_TYPE_REQUEST_RECOVERY = 'r';
    static constexpr uint8_t MESSAGE_TYPE_RECOVERY_MESSAGE = 'T';

    static constexpr uint8_t MESSAGE_TYPE_RECOVERY_CONSUMER_ACK = 't';
    static constexpr uint8_t MESSAGE_TYPE_RECOVERY_CONSUMER_ACK_ACK = 'w';

    static constexpr uint8_t MESSAGE_TYPE_END_OF_RECOVERY = 'e';
    static constexpr uint8_t MESSAGE_TYPE_START_RECOVERY = 'D';
    static constexpr uint8_t MESSAGE_TYPE_END_OF_RECOVERY_FOR_ADDRESS = 'V';
    static constexpr uint8_t MESSAGE_TYPE_RECOVERY_MESSAGE_ACK = 'W';
    static constexpr uint8_t MESSAGE_TYPE_RECOVERY_MESSAGE_SESSION_CHANGE= 'v';

    
    // [AASCII, zASCII], 
    static constexpr uint8_t zASCII = 'z'; // 122
    static constexpr uint8_t AASCII = 'A'; // 65
    static constexpr uint8_t MESSAGE_TYPE_UPDATE_CONFIG_HA = zASCII + 1;
    static constexpr uint8_t MESSAGE_TYPE_UPDATE_CONFIG_HA_ACK = zASCII + 2;
    static constexpr uint8_t MESSAGE_TYPE_SYNC_ALL_CONFIG_HA = zASCII + 3;

    // producer register
    static constexpr uint8_t MESSAGE_TYPE_REGISTER_PRODUCER_MESSAGE_PTO= 'k';
    static constexpr uint8_t MESSAGE_TYPE_REGISTER_PRODUCER_RESPONSE_MESSAGE_PTO= 'K';
    static constexpr uint8_t MESSAGE_TYPE_STANDBY_ACCOUNT_AUTHENTICATION_UPDATE = 'a';
    static constexpr uint8_t MESSAGE_TYPE_STANDBY_ACCOUNT_AUTHENTICATION_UPDATE_ACK = 'A';

    static constexpr uint8_t MESSAGE_TYPE_TRANSACTION_ACK_PTO = 49;

    // Constant Definition
    static constexpr uint32_t VARIABLE_TYPE_LENGTH_SIZE=2;
    static constexpr uint32_t BODY_LENGTH_SIZE=4;

    // Create type
    static constexpr uint8_t CREATE_TYPE_QUEUE = 'Q';
    static constexpr uint8_t CREATE_TYPE_TOPIC = 'T';

    // Create persistentcy
    static constexpr uint8_t CREATE_PERSISTENCY_PERSIST = 'P';
    static constexpr uint8_t CREATE_PERSISTENCY_MEMORY = 'M';

	//delete messages
	static constexpr uint8_t MESSAGE_TYPE_RECOVERY_DELETE_MESSAGES = 'd';

    // expired messages
    static constexpr uint8_t MESSAGE_TYPE_RECOVERY_EXPIRED_MESSAGES = 48;

    // ACK mode for subscription
    static constexpr uint8_t CLIENT_AUTO_ACK_MODE        = 0;
    static constexpr uint8_t CLIENT_COMMULATIVE_ACK_MODE = 1;
    static constexpr uint8_t CLIENT_INDIVIDUAL_ACK_MODE  = 2;

    static constexpr uint8_t MESSAGE_TYPE_SUSPEND_PRODUCER = 50;

    // result code
    static constexpr auto RESULT_CODE_NO_ERROR = 0;
    static constexpr auto PROVIDER_MESSAGEID_NOT_MATCH = 1;
    static constexpr auto FAILURE_DUPLICATE_SUBSCRIPTION = 2;
    static constexpr auto FAILURE_SUBSCRIPTION_NOT_EXIST = 3;
    static constexpr auto INVALID_ACK_WINDOW_SIZE = 4;
    static constexpr auto NOT_YET_RECEIVE_CONNECT = 5;
    static constexpr auto ALREADY_RECEIVE_CONNECT = 6;
    static constexpr auto CANNOT_FIND_DESTINATION = 7;
    static constexpr auto CANNOT_OBTAIN_MESSAGE_FROM_SESSION_STORE = 8;
    static constexpr auto CANNOT_OBTAIN_SESSION_STORE = 9;
    static constexpr auto SUBSCRIPTION_NAME_ALREADY_EXIST=10;
    static constexpr auto CANNOT_FIND_SUBSRIPTION_NAME=11;
    static constexpr auto FAIL_SUBSCRIPTION_OTHER_INDEX_FILE_EXIST=12;
    static constexpr auto INVALID_CREATE_TYPE = 13;
    static constexpr auto INVALID_PERSISTENCY_TYPE = 14;
    static constexpr auto INVALID_ADDRESS_TYPE = 15;
    static constexpr auto OTHER_TOPIC_ALREADY_SUBSCRIBED = 16;
    static constexpr auto INVALID_DESTINATION_NAME = 17;
    static constexpr auto DUPLICATED_ADDRESS_CREATION = 18;
    static constexpr auto CANNOT_SEND_BEFORE_ADDRESS_CREATION = 19;
    static constexpr auto CREATION_PARAMETER_CONFLICT = 20;
    static constexpr auto LOGON_FAILRUE_ALREADY_CONNECTED_APP=21;
    static constexpr auto CANNOT_SUBSCRIBE_BEFORE_ADDRESS_CREATION = 22;
    static constexpr auto ADDRESS_COUNT_LIMIT_EXCEED = 23;
    static constexpr auto MATCHER_SYNTAX_ERROR = 24;
	static constexpr auto LINK_SUBSCRIPTION_EXCLUSIVE_CONFLICTION = 25;
	static constexpr auto LINK_SUBSCRIPTION_ADDRESSTYPE_ERROR = 26;
    static constexpr auto ACCOUNT_LOGIN_SUCCESS = 27;             /**<account login success */
	static constexpr auto ACCOUNT_LOGIN_FAILED = 28;      /**<account login failed */
	static constexpr auto INVALID_AUTHORIZATION = 29;
	static constexpr auto SAVE_STORAGE_FAILED = 30;

	static constexpr auto KEY_BALANCE_NOT_SUPPORT_TOPIC = 31;
	static constexpr auto KEY_BALANCE_REG_DUPLICATE_NODE = 32;

	//  TRANSACTIONS...
	const ResultCode TRANSACTION_NOT_EXIST = 33;
	const ResultCode TRANSACTION_IN_COMMITING = 34;
	const ResultCode TRANSACTION_IN_ROLLBACK = 35;
	const ResultCode TRANSACTION_IN_PRE_COMMITING = 36;
	const ResultCode TRANSACTION_ALREADY_COMMITED = 37;
	const ResultCode TRANSACTION_ALREADY_ROLLBACK = 38;
	const ResultCode TRANSACTION_ALREADY_PRECOMMITED = 39;
	const ResultCode TRANSACTION_IS_NOT_LOCAL = 40;
	const ResultCode TRANSACTION_ILLEGAL_OPERATE = 41;
	const ResultCode TRANSACTION_NEED_DESTINATION_CREATED = 42;
	const ResultCode TRANSACTION_WAIT_RECOVERY_FAILED = 43;
	const ResultCode TRANSACTION_CHECK_FAILED = 44;
	const ResultCode TRANSACTION_HAS_END = 45;
    const ResultCode ADD2WATCHLIST_NOCLIENTID_FOUND = 46;


	const ResultCode NOT_FIND_KEY = 46;
	const ResultCode NOT_FIND_KEY_END = 60;

    //matcher operator type
    static constexpr uint8_t OPERATOR_EQUAL = 1;
	static constexpr uint8_t OPERATOR_UNEQUAL = 2;
	static constexpr uint8_t OPERATOR_IN = 3;
	static constexpr uint8_t OPERATOR_NOT_IN = 4;

    //message property type
    static constexpr uint8_t PROPERTY_TYPE_STRING = 0;
}

}} // namespace lightmq { namespace protocol {

#endif /* BROKERPROTOCOLCONSTANT_H_ */
