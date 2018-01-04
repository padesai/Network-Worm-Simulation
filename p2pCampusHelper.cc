#include "p2pCampusHelper.h"

#include <iostream>
#include <sstream>

using namespace ns3;

uint32_t MaxPacketSize = 512;
uint32_t maxPacketCount = 2;
uint16_t port = 5000;
uint32_t count = 0;
double min = 0.0;
double max = 95.0;

void receiveCallBack(uint32_t nodeId, PointToPointCampusHelper *p2p, Ptr<const Packet> item)
{
  static uint32_t value = 4;
  NodeInfo& nodeInfo = p2p->nodeInfoList[nodeId];

  if (!nodeInfo.infected)
  {
    count++;
    nodeInfo.infected = true;
    //std::cout << "Node infected " << Simulator::Now().GetSeconds() << " infected nodes "<< count << std::endl;
    //std::cout << "Dequeue:" << port << ":" << packet_count << ":" << Simulator::Now().GetSeconds() << std::endl;
    
    if(p2p->scanPattern == "Uniform"){    

    Ptr<UniformRandomVariable> randVar = CreateObject<UniformRandomVariable> ();
    randVar->SetAttribute ("Min", DoubleValue (min));
    randVar->SetAttribute ("Max", DoubleValue (max));
 
    value = (uint32_t)randVar->GetValue();
   }
    //std::cout << value << std::endl; 
    //value = 8;
 if(p2p->scanPattern == "Sequential" || p2p->scanPattern == "Local"){
	value++;
	}

   
    std::string address = "10." + std::to_string(value) + ".1.1";

    UdpClientHelper client(Ipv4Address(address.c_str()), port);

    client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
    client.SetAttribute ("Interval", TimeValue (Seconds (p2p->scanRate)));
    client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

    ApplicationContainer capps;
    capps = client.Install(nodeInfo.node);
    capps.Start ( Seconds(Simulator::Now().GetSeconds()) );
    nodeInfo.sourceApps.Add(capps);

  }
}


void sendCallBack(uint32_t nodeId, PointToPointCampusHelper *p2p, Ptr<const Packet> item)
{
  static uint32_t value;
  NodeInfo& nodeInfo = p2p->nodeInfoList[nodeId];

    //std::cout << "Infected Node started spreading the worm" << std::endl;
    nodeInfo.sourceApps.Stop(Seconds(Simulator::Now().GetSeconds()));

if(p2p->scanPattern == "Uniform"){

    Ptr<UniformRandomVariable> randVar = CreateObject<UniformRandomVariable> ();
    randVar->SetAttribute ("Min", DoubleValue (min));
    randVar->SetAttribute ("Max", DoubleValue (max));
    value = (uint32_t)randVar->GetValue();
  }
 if(p2p->scanPattern == "Sequential" || p2p->scanPattern == "Local"){
	value++;
 }

 
    //std::cout << value << std::endl; 
    //value =8;
  std::string address = "10." + std::to_string(value) + ".1.2";

  UdpClientHelper client(Ipv4Address(address.c_str()), port);

  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (Seconds (p2p->scanRate)));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

  ApplicationContainer capps;
  capps = client.Install(nodeInfo.node);
  capps.Start ( Seconds(Simulator::Now().GetSeconds()) );
  nodeInfo.sourceApps = capps;
  
}

bool PointToPointCampusHelper::CreateNodeOrNot(bool nodeTypeInner, const Ptr <UniformRandomVariable>& rand)
{
  double value = rand->GetValue();

  bool decision;
//    return true;
  if (nodeTypeInner)
  {

    if (value <= 80.0)
    {
      decision = true;
    }
    else
    {
      decision = false;
    }
  }
  else
  {
    if (value <= 60)
    {
      decision = true;
    }
    else
    {
      decision = false;
    }
  }
  return decision;
}

PointToPointCampusHelper::PointToPointCampusHelper(uint32_t maxInner, uint32_t maxOuter, PointToPointHelper inner,
                                                   PointToPointHelper outer, const Ptr <UniformRandomVariable>& rnd, int sysId, double sr, std::string sp)
{

  this->scanRate = sr; 
  this->scanPattern = sp;

  uint32_t nodeNum = 0;

  allNodes.Create(1,sysId);  // create the hub node
  nodeInfoList[nodeNum].node = allNodes.Get(0);
  //nodeInfoList[nodeNum++].infected = false;

  
  InternetStackHelper stack;
  //Ipv4NixVectorHelper nixRouting;
  //stack.SetRoutingHelper(nixRouting);
  stack.Install (allNodes);



  static int32_t ipAddressCount = -1;
  for (uint32_t i = 0; i < 8; ++i)
  {
    bool isInnerNode = true;
    //std::cout << "Address -- " << ipAddressCount << std::endl;
    if (!CreateNodeOrNot(isInnerNode, rnd))
    {
      ipAddressCount += 1;
      continue;
    }

    // ----------------- INNER NODES ----------------------------------
    NodeContainer iNode;
    iNode.Create(1,sysId);

    stack.Install(iNode);
    innerNodes.Add(iNode);

    NetDeviceContainer devices_l;
    devices_l = inner.Install(allNodes.Get(0), iNode.Get(0));
    devices.Add(devices_l);

    ipAddressCount += 1;
    Ipv4AddressHelper address;
    address.SetBase(("10." + std::to_string(ipAddressCount) + ".1.0").c_str(), "255.255.255.0");
    interfaces.Add(address.Assign(devices_l));


    devices_l.Get(1)->TraceConnectWithoutContext("PhyRxEnd", MakeBoundCallback(&receiveCallBack, nodeNum, this));
    devices_l.Get(1)->TraceConnectWithoutContext("PhyTxEnd", MakeBoundCallback(&sendCallBack, nodeNum, this));

    UdpServerHelper sink(port);
    ApplicationContainer apps = sink.Install(iNode);
//    apps.Start(Seconds(0.0));

    nodeInfoList[nodeNum].node = iNode.Get(0);
    nodeInfoList[nodeNum++].infected = false;

    for (int j = 0; j < 2; j++)
    {
      isInnerNode = false;
      if (!CreateNodeOrNot(isInnerNode, rnd))
      {
        ipAddressCount++;
        continue;
      }

      // ----------------- OUTER NODES ----------------------------------
      NodeContainer oNode;
      oNode.Create(1,sysId);
      stack.Install(oNode);
      outerNodes.Add(oNode);


      NetDeviceContainer devices_l;
      devices_l = outer.Install(iNode.Get(0), oNode.Get(0));
      devices.Add(devices_l);
      devices_l.Get(1)->TraceConnectWithoutContext("PhyRxEnd", MakeBoundCallback(&receiveCallBack, nodeNum, this));
      devices_l.Get(1)->TraceConnectWithoutContext("PhyTxEnd", MakeBoundCallback(&sendCallBack, nodeNum, this));

      //std::cout << "Address -- " << ipAddressCount << std::endl;
      ipAddressCount += 1;
      Ipv4AddressHelper address;
      address.SetBase(("10." + std::to_string(ipAddressCount) + ".1.0").c_str(), "255.255.255.0");
      interfaces.Add(address.Assign(devices_l));
      


      UdpServerHelper sink1 (port);
      apps = sink1.Install(oNode);
      apps.Start(Seconds(0.0));

      nodeInfoList[nodeNum].node = oNode.Get(0);
      nodeInfoList[nodeNum++].infected = false;
    }
  }
//   Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("nix-simple.routes", std::ios::out);
//   nixRouting.PrintRoutingTableAllAt (Seconds (8), routingStream);

  allNodes.Add(innerNodes);
  allNodes.Add(outerNodes);


  uint32_t index = 0;
  for (auto it = interfaces.Begin(); it != interfaces.End(); ++it)
  
{    std::stringstream str;
    std::cout << "Address -- ";
    interfaces.GetAddress(index).Print(std::cout);
    std::cout << std::endl;
    ++index;
  }

 std::cout << " -------------------------------------------------- " << std::endl;

}

PointToPointCampusHelper::~PointToPointCampusHelper()
{
}
