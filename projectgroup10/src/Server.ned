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

package projectgroup10;

//
// TODO auto-generated module
//
simple Server
{
    parameters:
        double C;
        double Icont;
        double Ifaas;
        int K;

		@signal[nJobs];
		@statistic[nJobsStat](source="nJobs"; record=timeavg,vector;);

		@signal[nJobsInQueue];
		@statistic[nJobsInQueueStat](source="nJobsInQueue"; record=timeavg,vector;);

        @signal[responseTime];
        @statistic[responseTimeStat](source="responseTime"; record=mean,vector;);

        @signal[responseTimeCont];
        @statistic[responseTimeContStat](source="responseTimeCont"; record=mean,vector;);

        @signal[responseTimeFaas];
        @statistic[responseTimeFaasStat](source="responseTimeFaas"; record=mean,vector;);

		@signal[waitingTime];
        @statistic[waitingTimeStat](source="waitingTime"; record=mean,vector;);

        @signal[utilization];
        @statistic[utilizationStat](source="utilization"; record=timeavg);


    gates:
        input in;
}
