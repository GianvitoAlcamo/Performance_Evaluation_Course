//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __PROJECTGROUP10_SERVER_H_
#define __PROJECTGROUP10_SERVER_H_

#include <omnetpp.h>
#include "Request_m.h"

using namespace omnetpp;

namespace projectgroup10 {

/**
 * TODO - Generated class
 */
class Server : public cSimpleModule
{
    double C_;
    double Icont_;
    double Ifaas_;
    double FreeC_;
    int K_;
    cQueue queue_;
    int nJobs;

    simsignal_t nJobsSignal;
    simsignal_t nJobsInQueueSignal;
    simsignal_t responseTimeSignal;
    simsignal_t responseTimeContSignal;
    simsignal_t responseTimeFaasSignal;
    simsignal_t waitingTimeSignal;
    simsignal_t utilizationSignal;

    void createContainer(Request* request);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

} //namespace

#endif
