/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2011-2015  Regents of the University of California.
 *
 * This file is part of ndnSIM. See AUTHORS for complete list of ndnSIM authors and
 * contributors.
 *
 * ndnSIM is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * ndnSIM is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ndnSIM, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * Auther: Masoud Sadri, Urmia University.
 **/
#include "ns3/core-module.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/ndnSIM-module.h"

using ns3::ndn::StrategyChoiceHelper;

namespace ns3 {

int
main(int argc, char* argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);
  AnnotatedTopologyReader topologyReader("", 1);
  topologyReader.SetFileName("src/ndnSIM/examples/topologies/ndn-scenario-topology.txt");
  topologyReader.Read();

  ndn::StackHelper ndnHelper;
//CS Policies
	ndnHelper.setPolicy("nfd::cs::lru");
  	ndnHelper.setCsSize(100);
	ndnHelper.SetDefaultRoutes(true);
	ndnHelper.InstallAll();
//////////////////////////////

//Choosing forwarding strategy
	ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/best-route");
//Installing global routing interface on all nodes
	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
	ndnGlobalRoutingHelper.InstallAll();

//Getting containers for the consumer/producer
	Ptr<Node> producer = Names::Find<Node>("root");

	Ptr<Node> consumers[8] = {
	Names::Find<Node>("user-1"), Names::Find<Node>("user-2"),Names::Find<Node>("user-3"),Names::Find<Node>("auser-4"),
	Names::Find<Node>("user-5"),Names::Find<Node>("user-6"),Names::Find<Node>("user-7"),Names::Find<Node>("auser-8"),
								};

//Legitimate Nodes configuration based on ConsumerCbr
	ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
	consumerHelper.SetAttribute("Frequency", StringValue("100"));
	consumerHelper.SetPrefix("/prefix");
	consumerHelper.SetAttribute("Randomize",StringValue("exponential"));
	consumerHelper.Install(consumers[0]);

	consumerHelper.SetAttribute("Frequency", StringValue("100"));
	consumerHelper.SetPrefix("/prefix");
	consumerHelper.SetAttribute("Randomize",StringValue("exponential"));
	consumerHelper.Install(consumers[1]);

	consumerHelper.SetAttribute("Frequency", StringValue("140"));
	consumerHelper.SetPrefix("/prefix/1701883133209369/a81b4615");
	consumerHelper.SetAttribute("Randomize",StringValue("uniform"));
	consumerHelper.Install(consumers[2]);

	consumerHelper.SetAttribute("Frequency", StringValue("100"));
	consumerHelper.SetPrefix("/prefix/file2");
	consumerHelper.SetAttribute("Randomize",StringValue("uniform"));
	consumerHelper.Install(consumers[4]);

	consumerHelper.SetAttribute("Frequency", StringValue("100"));
	consumerHelper.SetPrefix("/prefix");
	consumerHelper.SetAttribute("Randomize",StringValue("uniform"));
	consumerHelper.Install(consumers[5]);

	consumerHelper.SetAttribute("Frequency", StringValue("140"));
	consumerHelper.SetPrefix("/prefix/1701883133209369/a81b4615");
	consumerHelper.SetAttribute("Randomize",StringValue("uniform"));
	consumerHelper.Install(consumers[6]);

//malicious nodes configuration

	consumerHelper.SetAttribute("Frequency", StringValue("120"));
	consumerHelper.SetPrefix("/prefix");
	consumerHelper.SetAttribute("Randomize",StringValue("uniform"));
	consumerHelper.Install(consumers[3]);
	
	consumerHelper.SetAttribute("Frequency", StringValue("120"));
	consumerHelper.SetPrefix("/prefix");
	consumerHelper.SetAttribute("Randomize",StringValue("uniform"));
	consumerHelper.Install(consumers[7]);

////////////////////////////////////////////////////////////////////////////////////

//Producer configuration

	ndn::AppHelper producerHelper("ns3::ndn::Producer");
	producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
	producerHelper.SetAttribute("Signature", UintegerValue(100));
	producerHelper.SetAttribute("KeyLocator", StringValue("/unique/key/locator"));

	ndnGlobalRoutingHelper.AddOrigins("/prefix", producer);
	producerHelper.SetPrefix("/prefix");
	producerHelper.Install(producer).Start(Seconds(0));

  // Calculate and install FIBs
  ndn::GlobalRoutingHelper::CalculateRoutes();

  //Tracers
  ndn::CsTracer::InstallAll("ndn_attack_Cs_tracer.csv");
  ndn::L3RateTracer::InstallAll("ndn_attack_L3-rate-trace.csv");
  ndn::L3RateTracer::InstallAll("ndn_attack_L3-rate-trace.txt", Seconds(0.5));
  ndn::AppDelayTracer::InstallAll("ndn_attack_app-delays-trace.csv");
  L2RateTracer::InstallAll("ndn_attack_drop-trace.csv");
  //NetAnim XML result//
  AnimationInterface anim ("ndn_attack-anim.xml"); 

  //ns3::Timer::RUNNING
  Simulator::Stop(Seconds(12));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
