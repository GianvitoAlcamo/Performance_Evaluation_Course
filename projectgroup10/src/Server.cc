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
    // Initialize parameters
    C_ = par("C").doubleValue();
    FreeC_ = C_;
    Icont_ = par("Icont").doubleValue();
    Ifaas_ = par("Ifaas").doubleValue();
    K_ = par("K").intValue();
    nJobs=0;

    // Register signals
    nJobsSignal = registerSignal("nJobs");
    nJobsInQueueSignal = registerSignal("nJobsInQueue");
    responseTimeSignal = registerSignal("responseTime");
    responseTimeContSignal = registerSignal("responseTimeCont");
    responseTimeFaasSignal = registerSignal("responseTimeFaas");
    waitingTimeSignal = registerSignal("waitingTime");
    utilizationSignal = registerSignal("utilization");

    // Record starting state
    emit(nJobsSignal, 0);
    emit(nJobsInQueueSignal, 0);
    emit(utilizationSignal, 0);
}

void Server::handleMessage(cMessage *msg)
{
    Request* request = check_and_cast<Request*>(msg);
    // Enters if a job terminated his execution
    if (msg->isSelfMessage()){
        FreeC_ += request->getR();
        double utilization = (C_ - FreeC_) / C_;
        emit(utilizationSignal, utilization);

        nJobs-=1;
        emit(nJobsSignal, nJobs);


        simtime_t st_response = simTime() - msg->getCreationTime();
        emit(responseTimeSignal, st_response);
        if(request->getType())
            emit(responseTimeFaasSignal, st_response);
        else
            emit(responseTimeContSignal, st_response);

        delete(msg);

        // Check if some of the jobs in the queue can start executing
        while(!queue_.isEmpty()){
            Request* requestFromQueue = check_and_cast<Request*>(queue_.front());
            if(FreeC_ >= requestFromQueue->getR()){
                queue_.pop();
                emit(nJobsInQueueSignal, queue_.getLength());

                simtime_t st_waiting = simTime() - requestFromQueue->getCreationTime();
                emit(waitingTimeSignal, st_waiting);

                FreeC_ -= requestFromQueue->getR();
                double util = (C_ - FreeC_) / C_;
                emit(utilizationSignal, util);

                createContainer(requestFromQueue);
            }
            else
                break;
        }

    }
    // New request
    else{
        nJobs+=1;
        emit(nJobsSignal, nJobs);
        // If there is enough space the job start executing
        if(FreeC_ > request->getR()){
            FreeC_ -= request->getR();
            double util = (C_ - FreeC_) / C_;
            emit(utilizationSignal, util);
            createContainer(request);
            emit(waitingTimeSignal, 0);
        }
        // If there isn't, it gets stored in the queue
        else{
            queue_.insert(msg);
            emit(nJobsInQueueSignal, queue_.getLength());
        }
    }
}

void Server::createContainer(Request* request){
    simtime_t nextGen = request->getD();
    // The service time is set based on the type of the request
    if(request->getType())
        nextGen += 2 * K_ * Ifaas_;
    else
        nextGen += 2 * Icont_;
    scheduleAt(simTime() + nextGen, request);
}


void Server::finish(){
    while(!queue_.isEmpty()){
        cMessage *msg = (cMessage *) queue_.pop();
        delete msg;
    }
}


} //namespace

