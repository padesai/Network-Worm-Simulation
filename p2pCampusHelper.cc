#include "p2pCampusHelper.h"

#include <iostream>

using namespace ns3;

bool PointToPointCampusHelper::CreateNodeOrNot(bool nodeTypeInner, Ptr<UniformRandomVariable> rand) {

	double value = rand->GetValue();

	bool decision;

	if (nodeTypeInner) {	
		
		if (value<=80.0) {
			decision = true;
		}
		else {
			decision = false;
		}
	}
	else {
		if (value <= 60) {
			decision = true;
		}	
		else {
			decision = false;
		}
	}	
	return decision;
}

PointToPointCampusHelper::PointToPointCampusHelper(uint32_t maxInner, uint32_t maxOuter, PointToPointHelper inner, PointToPointHelper outer, Ptr<UniformRandomVariable> rnd, int ipIncrement) {

	NodeContainer nodes;
	nodes.Create(1);	
	
	bool nTypeInner;
	bool createInner;	
	bool createOuter;
	int innerNodesNum = 0;	
	bool innerExistsOrNot [8];

	InternetStackHelper stack;	
	
	stack.Install(nodes);		

	for (int i = 0; i < 8 ; i++) {

		nTypeInner = true;

		createInner = CreateNodeOrNot(nTypeInner, rnd);

		if (createInner) {
			NodeContainer node1;
			node1.Create(1);
			nodes.Add(node1);
			NetDeviceContainer devices;

    			devices = inner.Install(nodes.Get(0), nodes.Get(innerNodesNum+1));
			
			stack.Install(nodes.Get(nodes.GetN()-1));			
			
			Ipv4AddressHelper address;			
			
			address.SetBase(("10." + std::to_string(i+ipIncrement) + ".1.0").c_str(), "255.255.255.0");	
			Ipv4InterfaceContainer interfaces = address.Assign(devices);
			
			innerExistsOrNot[i] = true;
			innerNodesNum++;
		}
		else {
			innerExistsOrNot[i] = false;
		}
	}
	
	int nodeIterator=0;	
		
	for (int i=0; i < 8; i++) {

		if (innerExistsOrNot[i] == true) {

			for (int j = 0; j < 2; j++) {

				nTypeInner = false;

				createOuter = CreateNodeOrNot(nTypeInner, rnd);
				if (createOuter) {

					NodeContainer node2;
					node2.Create(1);
					nodes.Add(node2);
					NetDeviceContainer devices;
					
    					devices = outer.Install(nodes.Get(nodeIterator+1), nodes.Get(nodes.GetN()-1));	
					
					stack.Install(nodes.Get(nodes.GetN()-1));

					Ipv4AddressHelper address;					

					address.SetBase(("10." + std::to_string(8+2*i+j+ipIncrement) + ".1.0").c_str(), "255.255.255.0");	
					Ipv4InterfaceContainer interfaces = address.Assign(devices);
					
					
				}
			}
		
			nodeIterator++;
		}
	}		
	
	Ipv4NixVectorHelper nixRouting;
	stack.SetRoutingHelper (nixRouting);
			

}

PointToPointCampusHelper::~PointToPointCampusHelper() {
	
}

