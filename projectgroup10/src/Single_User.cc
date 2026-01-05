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

#include "Single_User.h"

namespace projectgroup10 {

Define_Module(Single_User);

void Single_User::initialize()
{
    // initialize parameters
    timer_ = new cMessage("generationTimer");
    Tnew_ = getParentModule()->par("T").doubleValue()/getParentModule()->par("N").intValue();
    R_ = par("R").doubleValue();
    D_ = par("D").doubleValue();
    P_ = getParentModule()->par("P").doubleValue();

    scheduleNext();
}

void Single_User::handleMessage(cMessage *msg)
{
    double R=uniform(0.5*R_,R_*1.5,0);
    double D=exponential(D_,1);

    // With probability P_ the request is of type Faas, otherwise it is Container-based
    if(bernoulli(P_,2)){
        Request *request = createRequest(R, D, true);
        send(request, "out");
    }
    else{
        Request *request = createRequest(R, D, false);
        send(request, "out");
    }

    scheduleNext();
}


void Single_User::scheduleNext()
{
    simtime_t nextGen = exponential(Tnew_,3);
    scheduleAt(simTime() + nextGen, timer_);
}

Request* Single_User::createRequest(double R, double D, bool type){
    Request *request = new Request("request");
    request->setD(D);
    request->setR(R);
    request->setType(type);
    return request;
}

void Single_User::finish(){
    cancelAndDelete(timer_);
}


} //namespace
