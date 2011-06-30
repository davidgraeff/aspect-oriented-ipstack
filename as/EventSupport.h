#ifndef __IPSTACK_EVENT_SUPPORT_H__
#define __IPSTACK_EVENT_SUPPORT_H__

#include "as/autosar_types.h"
#include "objects.h" //this file must contain an event 'IPStackReceiveEvent'

const EventMaskType IPStackReceiveEvent_ = IPStackReceiveEvent;
const EventMaskType IPStackAlarmEvent = IPStackReceiveEvent_;

#endif // __IPSTACK_EVENT_SUPPORT_H__

