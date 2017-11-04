#include <iostream>

#include "p2pCampusHelper.h"

int main(int argc, char** argv) {

	std::string innerLinkBW = "100Mbps";
	std::string innerLinkDelay = "5ms";
	std::string outerLinkDelay = "8ms";

	PointToPointHelper inner;
	inner.SetDeviceAttribute ("DataRate", StringValue(innerLinkBW));
	inner.SetChannelAttribute ("Delay", StringValue(innerLinkDelay));

	PointToPointHelper outer;
	outer.SetDeviceAttribute ("DataRate", StringValue(innerLinkBW));
	outer.SetChannelAttribute ("Delay", StringValue(outerLinkDelay));

	double min = 0.0;
    	double max = 100.0;
    	Ptr<UniformRandomVariable> randVar = CreateObject<UniformRandomVariable> ();
    	randVar->SetAttribute ("Min", DoubleValue (min));
    	randVar->SetAttribute ("Max", DoubleValue (max));

	PointToPointCampusHelper p2pCampusHelper1 = PointToPointCampusHelper(8,2,inner,outer,randVar, 0);
	PointToPointCampusHelper p2pCampusHelper2 = PointToPointCampusHelper(8,2,inner,outer,randVar, 24);
	PointToPointCampusHelper p2pCampusHelper3 = PointToPointCampusHelper(8,2,inner,outer,randVar, 48);
	PointToPointCampusHelper p2pCampusHelper4 = PointToPointCampusHelper(8,2,inner,outer,randVar, 72);
	

	std::cout << "Hello" << std::endl; 

}
