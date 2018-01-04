#pragma once

#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv6-address-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/internet-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/ipv6-interface-container.h"
#include "ns3/random-variable-stream.h"
#include "ns3/ipv4-nix-vector-helper.h"
#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-nix-vector-helper.h"

#include <map>

using namespace ns3;


struct NodeInfo
{
 /* NodeInfo()
    : infected(false)
  {

  }
*/
  bool infected=false;
  ApplicationContainer sourceApps;
  Ptr<Node> node;
};

class PointToPointCampusHelper
{
public:
  friend void receiveCallBack(uint32_t nodeId, PointToPointCampusHelper *p2p, Ptr<const Packet> item);
  friend void sendCallBack(uint32_t nodeId, PointToPointCampusHelper *p2p, Ptr<const Packet> item);

  PointToPointCampusHelper(uint32_t maxInner, uint32_t maxOuter, PointToPointHelper inner, PointToPointHelper outer, const Ptr <UniformRandomVariable>& rnd, int systemId, double sR, std::string sP);

  ~PointToPointCampusHelper();

  bool CreateNodeOrNot(bool nodeTypeInner, const Ptr<UniformRandomVariable>& rnd);


  NetDeviceContainer devices;
  NodeContainer innerNodes;
  NodeContainer outerNodes;
  NodeContainer allNodes;
  double scanRate;
  std::string scanPattern;

  Ipv4InterfaceContainer interfaces;


  std::map<uint32_t, NodeInfo> nodeInfoList;
};
