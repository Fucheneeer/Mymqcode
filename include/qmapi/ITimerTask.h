////////////////////////////////////////////////////////////////////////////////
//
// BaseSelectService.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Header file of BaseSelectService
//
////////////////////////////////////////////////////////////////////////////////

#ifndef ITIMERTASK_H_
#define ITIMERTASK_H_

namespace lightmq { namespace qmapi {

class ITimerTask {
public:
    virtual ~ITimerTask() = default;

    virtual void onTimer(uint32_t timerId) = 0;
};


}} // namespace lightmq { namespace qmapi {



#endif /* ITIMERTASK_H_ */
