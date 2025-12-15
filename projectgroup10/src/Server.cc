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

#include "Server.h"
#include "Request_m.h"

namespace projectgroup10 {

Define_Module(Server);

void Server::initialize()
{
    C_ = par("C").doubleValue();
    FreeC_ = C_;
    Icont_ = par("Icont").doubleValue();
    Ifaas_ = par("Ifaas").doubleValue();
    nJobsInQueueSignal = registerSignal("nJobsInQueue");
    responseTimeSignal = registerSignal("responseTime");
    waitingTimeSignal = registerSignal("waitingTime");
    utilizationSignal = registerSignal("utilization");
}

void Server::handleMessage(cMessage *msg)
{
    Request* request = check_and_cast<Request*>(msg);
    if (msg->isSelfMessage()){
        double utilization = (C_ - FreeC_) / C_;
        emit(utilizationSignal, utilization);

        FreeC_ += request->getR();

        simtime_t st_response = simTime() - msg->getCreationTime();
        emit(responseTimeSignal, st_response);

        delete(msg);

        while(!queue_.isEmpty()){
            Request* requestFromQueue = check_and_cast<Request*>(queue_.front());
            if(FreeC_ >= requestFromQueue->getR()){
                emit(nJobsInQueueSignal, queue_.getLength());
                queue_.pop();

                simtime_t st_waiting = simTime() - msg->getCreationTime();
                emit(waitingTimeSignal, st_waiting);

                double util = (C_ - FreeC_) / C_;
                emit(utilizationSignal, util);

                FreeC_ -= requestFromQueue->getR();
                createContainer(requestFromQueue);
            }
            else
                break;
        }

    }
    else{
        if(FreeC_ > request->getR()){
            FreeC_ -= request->getR();
            createContainer(request);
        }
        else{
            if(!queue_.isEmpty())
                emit(nJobsInQueueSignal, queue_.getLength());
            queue_.insert(msg);
        }
    }
}

void Server::createContainer(Request* request){
    simtime_t nextGen = exponential(request->getD());
    if(request->getType())
        nextGen += 2 * Ifaas_;
    else
        nextGen += 2 * Icont_;
    scheduleAt(simTime() + nextGen, request);
}


void Server::finish(){
    while(!queue_.isEmpty()){
        queue_.pop();
    }
}


} //namespace

