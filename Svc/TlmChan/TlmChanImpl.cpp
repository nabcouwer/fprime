/**
 * \file
 * \author T. Canham
 * \brief Implementation file for channelized telemetry storage component
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */
#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <cstring>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Com/ComBuffer.hpp>

#include <stdio.h>

namespace Svc {

    TlmChanImpl::TlmChanImpl(const char* name) : TlmChanComponentBase(name)
    {
        // clear data
        this->m_activeBuffer = 0;
        // clear slot pointers
        for (NATIVE_UINT_TYPE entry = 0; entry < TLMCHAN_NUM_TLM_HASH_SLOTS; entry++) {
            this->m_tlmEntries[0].slots[entry] = 0;
            this->m_tlmEntries[1].slots[entry] = 0;
        }
        // clear buckets
        for (NATIVE_UINT_TYPE entry = 0; entry < TLMCHAN_HASH_BUCKETS; entry++) {
            this->m_tlmEntries[0].buckets[entry].used = false;
            this->m_tlmEntries[0].buckets[entry].updated = false;
            this->m_tlmEntries[0].buckets[entry].bucketNo = entry;
            this->m_tlmEntries[0].buckets[entry].next = 0;
            this->m_tlmEntries[0].buckets[entry].id = 0;
            this->m_tlmEntries[1].buckets[entry].used = false;
            this->m_tlmEntries[1].buckets[entry].updated = false;
            this->m_tlmEntries[1].buckets[entry].bucketNo = entry;
            this->m_tlmEntries[1].buckets[entry].next = 0;
            this->m_tlmEntries[1].buckets[entry].id = 0;
        }
        // clear free index
        this->m_tlmEntries[0].free = 0;
        this->m_tlmEntries[1].free = 0;


    }

    TlmChanImpl::~TlmChanImpl() {
    }

    void TlmChanImpl::init(
            NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
            NATIVE_INT_TYPE instance /*!< The instance number*/
            ) {
        TlmChanComponentBase::init(queueDepth,instance);
    }

    NATIVE_UINT_TYPE TlmChanImpl::doHash(FwChanIdType id) {
        return (id % TLMCHAN_HASH_MOD_VALUE)%TLMCHAN_NUM_TLM_HASH_SLOTS;
    }

    void TlmChanImpl::Service_handler(NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context){
      //      ActiveComponentBase::s_baseBareTask(this);
            if (!this->m_task.isStarted()) {
                this->m_task.setStarted(true);
                this->preamble();
            }
            if (this->m_queue.getNumMsgs() == 0) {
                return;
            }
            ActiveComponentBase::MsgDispatchStatus loopStatus = this->doDispatch();
            switch (loopStatus) {
                case QueuedComponentBase::MSG_DISPATCH_OK: // if normal message processing, continue
                    break;
                case QueuedComponentBase::MSG_DISPATCH_EXIT:
                    this->finalizer();
                    this->m_task.setStarted(false);
                    break;
                default:
                    FW_ASSERT(0,(NATIVE_INT_TYPE)loopStatus);
            }
          }

    void TlmChanImpl::pingIn_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      )
    {
        // return key
        this->pingOut_out(0,key);
    }


}
