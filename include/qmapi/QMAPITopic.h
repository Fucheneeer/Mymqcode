////////////////////////////////////////////////////////////////////////////////
//
// QMAPITopic.h
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

#ifndef QMAPITOPIC_H_
#define QMAPITOPIC_H_

#include "ITopic.h"

namespace lightmq { namespace qmapi {

class QMAPITopic : public ITopic {
public:
	virtual ~QMAPITopic() override;
	//QMAPITopic() = delete;
	QMAPITopic(const std::string &name): m_name(name) {}
	QMAPITopic(std::string &&name): m_name(std::move(name)) {}

    /**
     * Gets the name of this topic.
     *
     * @return The topic name.
     *
     */
    std::string getName() const override;
    IDestination::Type getType() const override;

private:
    std::string m_name;
};

}} // namespace lightmq { namespace qmapi {

#endif /* QMAPITOPIC_H_ */
