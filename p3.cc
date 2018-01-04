#include <iostream>
#include <sstream>
#include <chrono>

#include "p2pCampusHelper.h"
#include "ns3/mpi-interface.h"

#ifdef NS3_MPI
#include <mpi.h>
#endif

extern uint16_t port;
extern uint32_t MaxPacketSize ;
extern uint32_t maxPacketCount;
extern uint32_t count;
std::stringstream time_axis;
std::stringstream count_axis;

void SampleInfectedNodes()
{
  std::cout << Simulator::Now().GetSeconds() << "," << count << std::endl;
  time_axis << Simulator::Now().GetSeconds() << ",";
  count_axis << count <<",";
  Simulator::Schedule(Seconds(50), &SampleInfectedNodes); 
}

int main(int argc, char** argv)
{

#ifdef NS3_MPI

	double scanRate = 0.005;
	std::string scanPattern = "Uniform";
	std::string backboneDelay = "5ms";
	std::string syncType = "Yawns";

	CommandLine cmd;
	cmd.AddValue("ScanRate", "Rate to generate worm traffic (5,10,20) ms, default 5", scanRate);
	cmd.AddValue("ScanPattern", "Scanning pattern (Uniform, Local, Sequential), default Uniform", scanPattern);
	cmd.AddValue("BackboneDelay", "ms delay for backbone link, default 5ms", backboneDelay);
	cmd.AddValue("SyncType", "Conservative algorithm (Yawns,Null), defaul Yawns", syncType);
	
        cmd.Parse(argc,argv);

	if(syncType == "Null") {
    		GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::NullMessageSimulatorImpl"));
  	}	
	else {
    		GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::DistributedSimulatorImpl"));
  	}

	// Enable parallel simulator with the command line arguments
	MpiInterface::Enable (&argc, &argv);

  	//uint32_t systemId = MpiInterface::GetSystemId ();
  	uint32_t systemCount = MpiInterface::GetSize ();


  std::string innerLinkBW = "100Mbps";
  std::string innerLinkDelay = "5ms";
  std::string outerLinkDelay = "8ms";

  PointToPointHelper p2pHelperInner;
  PointToPointHelper p2pHelperOuter;

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

	int sysId1, sysId2, sysId3, sysId4;

	if (systemCount == 1) {
		 sysId1 = 0;
	         sysId2 = 0;
		 sysId3 = 0;
		 sysId4 = 0;
	}
	
	if (systemCount == 2) {
		 sysId1 = 0;
	         sysId2 = 0;
		 sysId3 = 1;
		 sysId4 = 1;		
	}
	
	if (systemCount == 4) {
		 sysId1 = 0;
	         sysId2 = 1;
		 sysId3 = 2;
		 sysId4 = 3;
	}

  PointToPointCampusHelper p2pCampusHelper1(8,2,inner,outer,randVar,sysId1,scanRate,scanPattern);
  PointToPointCampusHelper p2pCampusHelper2(8,2,inner,outer,randVar,sysId2,scanRate,scanPattern);
  PointToPointCampusHelper p2pCampusHelper3(8,2,inner,outer,randVar,sysId3,scanRate,scanPattern);
  PointToPointCampusHelper p2pCampusHelper4(8,2,inner,outer,randVar,sysId4,scanRate,scanPattern);
  
  Ptr<Node> hub1 = p2pCampusHelper1.allNodes.Get(0);
  Ptr<Node> hub2 = p2pCampusHelper2.allNodes.Get(0);
  Ptr<Node> hub3 = p2pCampusHelper3.allNodes.Get(0);
  Ptr<Node> hub4 = p2pCampusHelper4.allNodes.Get(0);

  PointToPointHelper backbone;
  backbone.SetDeviceAttribute ("DataRate", StringValue(innerLinkBW));
  backbone.SetChannelAttribute ("Delay", StringValue(backboneDelay));
 

    NetDeviceContainer devices1;
    NetDeviceContainer devices2;
    NetDeviceContainer devices3;
    NetDeviceContainer devices4;
    devices1 = backbone.Install(hub1, hub2);
    devices2 = backbone.Install(hub2, hub3);
    devices3 = backbone.Install(hub3, hub4);
    devices4 = backbone.Install(hub4, hub1);
/*
  InternetStackHelper internet;
  backbone.InstallStack(internet);
*/


    Ipv4AddressHelper address;
    address.SetBase(("10." + std::to_string(97) + ".1.0").c_str(), "255.255.255.0");
    address.Assign(devices1);
    Ipv4AddressHelper address1;
    address1.SetBase(("10." + std::to_string(98) + ".1.0").c_str(), "255.255.255.0");
    address1.Assign(devices2);
    Ipv4AddressHelper address2;
    address2.SetBase(("10." + std::to_string(99) + ".1.0").c_str(), "255.255.255.0");
    address2.Assign(devices3);
    Ipv4AddressHelper address3;
    address3.SetBase(("10." + std::to_string(100) + ".1.0").c_str(), "255.255.255.0");
    address3.Assign(devices4);

  
  
  double min_l = 0.0;
  double max_l = 150.0;
  randVar = CreateObject<UniformRandomVariable> ();
  randVar->SetAttribute ("Min", DoubleValue (min_l));
  randVar->SetAttribute ("Max", DoubleValue (max_l));
  uint32_t value = (uint32_t)randVar->GetValue();

  std::string addressA = "10." + std::to_string(value) + ".1.1";

  UdpClientHelper client(Ipv4Address(addressA.c_str()), port);

  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (Seconds (0.05)));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

  NodeContainer allN;
  allN.Add(p2pCampusHelper1.allNodes);
  allN.Add(p2pCampusHelper2.allNodes);
  allN.Add(p2pCampusHelper3.allNodes);
  allN.Add(p2pCampusHelper4.allNodes);
  
  std::cout << "Total nodes created: " << allN.GetN() << std::endl;
  ApplicationContainer capps;
  capps = client.Install(p2pCampusHelper1.allNodes.Get(3));
  capps.Start (Seconds(1.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();
  Simulator::Stop(Seconds(1200.0));  
  Simulator::ScheduleNow(&SampleInfectedNodes);	
  using namespace std::chrono;
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  Simulator::Run();

  Simulator::Destroy();
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double, std::milli> time_span = t2-t1;
  std::cout << "Wall clock time = " << time_span.count() << " ms" << std::endl;
  std::cout << "time value: " <<time_axis.str() << std::endl;
  std::cout << "count value: " <<count_axis.str() << std::endl;
  std::cout << "Total nodes infected : " << count << std::endl; 

MpiInterface::Disable ();
  
  return 0;
#else
NS_FATAL_ERROR("Can't use distributed simulator without MPI compiled in");
#endif

}
