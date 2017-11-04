#pragma once

#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv6-address-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/ipv6-interface-container.h"
#include "ns3/random-variable-stream.h"
#include "ns3/ipv4-nix-vector-helper.h"
#include "ns3/core-module.h"


using namespace ns3;


class PointToPointCampusHelper {
public:

	PointToPointCampusHelper(uint32_t maxInner, uint32_t maxOuter, PointToPointHelper inner, PointToPointHelper outer, Ptr<UniformRandomVariable> rnd, int ipIncremen);

	~PointToPointCampusHelper();

	bool CreateNodeOrNot(bool nodeTypeInner, Ptr<UniformRandomVariable> rnd);

};
