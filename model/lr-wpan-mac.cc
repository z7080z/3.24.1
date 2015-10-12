/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 The Boeing Company
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:
 *  Gary Pei <guangyu.pei@boeing.com>
 *  kwong yin <kwong-sang.yin@boeing.com>
 *  Tom Henderson <thomas.r.henderson@boeing.com>
 *  Sascha Alexander Jopen <jopen@cs.uni-bonn.de>
 *  Erwan Livolant <erwan.livolant@inria.fr>
 */
#include "lr-wpan-mac.h"
#include "lr-wpan-csmaca.h"
#include "lr-wpan-mac-header.h"
#include "lr-wpan-mac-trailer.h"
#include <ns3/simulator.h>
#include <ns3/log.h>
#include <ns3/uinteger.h>
#include <ns3/node.h>
#include <ns3/packet.h>
#include <ns3/random-variable-stream.h>
#include <ns3/double.h>

//Ubines
#include "ns3/mac16-address.h"
#include "ns3/mac64-address.h"
#include "ns3/seq-ts-header.h"
//

#undef NS_LOG_APPEND_CONTEXT
#define NS_LOG_APPEND_CONTEXT                                   \
  std::clog << "[address " << m_shortAddress << "] ";

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LrWpanMac");

NS_OBJECT_ENSURE_REGISTERED (LrWpanMac);

const uint32_t LrWpanMac::aMinMPDUOverhead = 9; // Table 85

TypeId
LrWpanMac::GetTypeId (void)
{
	//std::cout << m_PANCoordinator << " : " << "GetTypeId" << std::endl;
  static TypeId tid = TypeId ("ns3::LrWpanMac")
    .SetParent<Object> ()
    .SetGroupName ("LrWpan")
    .AddConstructor<LrWpanMac> ()
    .AddAttribute ("PanId", "16-bit identifier of the associated PAN",
                   UintegerValue (),
                   MakeUintegerAccessor (&LrWpanMac::m_macPanId),
                   MakeUintegerChecker<uint16_t> ())
    .AddTraceSource ("MacTxEnqueue",
                     "Trace source indicating a packet has been "
                     "enqueued in the transaction queue",
                     MakeTraceSourceAccessor (&LrWpanMac::m_macTxEnqueueTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MacTxDequeue",
                     "Trace source indicating a packet has was "
                     "dequeued from the transaction queue",
                     MakeTraceSourceAccessor (&LrWpanMac::m_macTxDequeueTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MacTx",
                     "Trace source indicating a packet has "
                     "arrived for transmission by this device",
                     MakeTraceSourceAccessor (&LrWpanMac::m_macTxTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MacTxOk",
                     "Trace source indicating a packet has been "
                     "successfully sent",
                     MakeTraceSourceAccessor (&LrWpanMac::m_macTxOkTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MacTxDrop",
                     "Trace source indicating a packet has been "
                     "dropped during transmission",
                     MakeTraceSourceAccessor (&LrWpanMac::m_macTxDropTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MacPromiscRx",
                     "A packet has been received by this device, "
                     "has been passed up from the physical layer "
                     "and is being forwarded up the local protocol stack.  "
                     "This is a promiscuous trace,",
                     MakeTraceSourceAccessor (&LrWpanMac::m_macPromiscRxTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MacRx",
                     "A packet has been received by this device, "
                     "has been passed up from the physical layer "
                     "and is being forwarded up the local protocol stack.  "
                     "This is a non-promiscuous trace,",
                     MakeTraceSourceAccessor (&LrWpanMac::m_macRxTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MacRxDrop",
                     "Trace source indicating a packet was received, "
                     "but dropped before being forwarded up the stack",
                     MakeTraceSourceAccessor (&LrWpanMac::m_macRxDropTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("Sniffer",
                     "Trace source simulating a non-promiscuous "
                     "packet sniffer attached to the device",
                     MakeTraceSourceAccessor (&LrWpanMac::m_snifferTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PromiscSniffer",
                     "Trace source simulating a promiscuous "
                     "packet sniffer attached to the device",
                     MakeTraceSourceAccessor (&LrWpanMac::m_promiscSnifferTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MacStateValue",
                     "The state of LrWpan Mac",
                     MakeTraceSourceAccessor (&LrWpanMac::m_lrWpanMacState),
                     "ns3::TracedValueCallback::LrWpanMacState")
    .AddTraceSource ("MacState",
                     "The state of LrWpan Mac",
                     MakeTraceSourceAccessor (&LrWpanMac::m_macStateLogger),
                     "ns3::LrWpanMac::StateTracedCallback")
    .AddTraceSource ("MacSentPkt",
                     "Trace source reporting some information about "
                     "the sent packet",
                     MakeTraceSourceAccessor (&LrWpanMac::m_sentPktTrace),
                     "ns3::LrWpanMac::SentTracedCallback")
  ;
  return tid;
}

LrWpanMac::LrWpanMac ()
{
	std::cout << m_PANCoordinator << " : " << "LrWpanMac no argv" << std::endl;
  // First set the state to a known value, call ChangeMacState to fire trace source.
  m_lrWpanMacState = MAC_IDLE;
  ChangeMacState (MAC_IDLE);

  m_macRxOnWhenIdle = true;
  m_macPanId = 0;
  m_associationStatus = ASSOCIATED;
  m_selfExt = Mac64Address::Allocate ();
  m_macPromiscuousMode = false;
  m_macMaxFrameRetries = 3;
  m_retransmission = 0;
  m_numCsmacaRetry = 0;
  m_txPkt = 0;

  Ptr<UniformRandomVariable> uniformVar = CreateObject<UniformRandomVariable> ();
  uniformVar->SetAttribute ("Min", DoubleValue (0.0));
  uniformVar->SetAttribute ("Max", DoubleValue (255.0));
  m_macDsn = SequenceNumber8 (uniformVar->GetValue ());
  m_macBsn = SequenceNumber8 (uniformVar->GetValue ());

	m_deviceDescriptorHandleList.clear();

	m_shortAddress = Mac16Address::Allocate ();

	m_FullFunctionDevice = true; //true FFD. 

	m_BLE = false;
	m_macAssociationPermit = false;

	SetDefaultPIB(false);

	IsGTSBeaconTracking = false;
	View = false;
	m_PANCoordinator = false;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//added by Ubines 20131017 
//createor that can inintalize panCoordinator or coordinator
LrWpanMac::LrWpanMac (bool panCoordinator)
{
	std::cout << m_PANCoordinator << " : " << "LrWpanMac with panC" << std::endl;
	Ptr<UniformRandomVariable> uniformVar = CreateObject<UniformRandomVariable> ();
  uniformVar->SetAttribute ("Min", DoubleValue (0.0));
  uniformVar->SetAttribute ("Max", DoubleValue (255.0));

	m_lrWpanMacState = MAC_IDLE;
  ChangeMacState (MAC_IDLE);
  
	//////////////////////////////////
	//added by Ubines 20131101
	//for initialize deviceDesciptorVector
	m_deviceDescriptorHandleList.clear();
	//////////////////////////////////
	//modified by Ubines 20131031
	//To assign different panID between nodes. 
	m_macRxOnWhenIdle = true;
	m_macPanId = (uint16_t)uniformVar->GetValue ();
	m_associationStatus = DISASSOCIATED;
	m_selfExt = Mac64Address::Allocate ();
	m_macPromiscuousMode = false;
	m_macMaxFrameRetries = 3;
	m_retransmission = 0;
  m_numCsmacaRetry = 0;
  m_txPkt = 0;
  
	m_macDsn = SequenceNumber8 (uniformVar->GetValue ());
	m_macBsn = SequenceNumber8 (uniformVar->GetValue ());
	m_shortAddress = Mac16Address ("00:00");
	m_FullFunctionDevice = true; //true FFD. 
	SetDefaultPIB(false);
	//added by Ubines 20131107
	//determine wheter GTS request command frame receive acknowledgment frame or not
	IsGTSBeaconTracking = false;
	View = false;
	//end Ubines

	if (panCoordinator == true)
	{
		//added by Ubines 20131104
		m_BLE = false;
		m_macAssociationPermit = true;
		m_PANCoordinator = true;

		m_shortAddress = Mac16Address::Allocate ();
		SetDefaultPIB(panCoordinator);
		m_CoordPANId = m_macPanId;
		
		m_beaconEvent = Simulator::ScheduleNow (&LrWpanMac::SendOneBeacon, this);
	}
	else
	{
		//added by Ubines 20131204
		//for beacon tracking
		Simulator::Schedule (m_beaconSearchDuration, &LrWpanMac::beaconCheck, this);
	}
}
//end Ubines 

LrWpanMac::~LrWpanMac ()
{
}

//////////////////////////////////////////////////////////////////////////////
//added by Ubines 20131030 
//set default PIB to send for MLME_REST.reauest parameter
bool
LrWpanMac::SetDefaultPIB (bool type)
{
	//std::cout << m_PANCoordinator << " : " << "SetDefaultPIB" << std::endl;
	m_aBaseSlotDuration			= 60; //if superframe order is zero,
	m_macBeaconOrder			= 4; // 0-15 //indicates the frequency with which the beacon is transmitted, as defined in 5.1.1.1
	m_aNumSuperframeSlots		= 16; // The number of slots contained in any superframe.
	m_aBaseSuperframeDuration	= m_aBaseSlotDuration * m_aNumSuperframeSlots; // aBaseSlotDuration ¡¿ aNumSuperframeSlots
	m_aMaxLostBeacons			= 4;
	m_macBattLifeExt			= false;

	///////////////////////////////////////////////////
	//modified by Ubines 20130106
	//increase MinCapLength
	m_aMinCAPLength				= 440 * pow(2, m_macSuperframeOrder); //The minimum number of symbols forming the CAP.
	//end Ubines
	//m_FinalCAPSlot				= m_aMinCAPLength / m_aBaseSlotDuration;
	m_FinalCAPSlot				= 15;
	m_macPanId					= 0xffff;
	m_macSyncSymbolOffset       = 0x100;
	m_beaconSearchDuration		= Seconds(CalculateSymbolToTime(m_aBaseSuperframeDuration * (pow(2, m_macBeaconOrder) + 1)).GetSeconds()) + Seconds(0.018);;
	m_aMaxSIFSFrameSize         = 18; //The maximum size of an MPDU, in octets, that can be followed by a SIFS period.
	m_aMinMPDUOverhead			= 9; //The minimum number of octets added by the MAC sublayer to the PSDU.
	m_macLIFSPeriod				= 40; //macLIFSPeriod - 40 symbols
	m_macSIFSPeriod				= 12; //macSIFSPeriod - 12 symbols

	//////////////////////////////////////////////
  	//added by Ubines 20131106
	//for GTS request frame setting
	m_aGTSDescPersistenceTime = 4; //The number of superframe in which a GTS descriptor
	m_CAPLength = m_aNumSuperframeSlots;
	m_PANCoordinator = false;
  	//end Ubines

	//////////////////////////////////////////////
	//added by Ubines 20131119
	//macAckWaitDuration setting
	//modified by Ubines 20131227
	m_aUnitBackoffPeriod = 20;
	phySymbolsPerOctet = 2;
	phySHRDuration = 0; 
	m_macAckWaitDuration = m_aUnitBackoffPeriod + m_phy->aTurnaroundTime + phySHRDuration + 6 * phySymbolsPerOctet + 250;
	
	if (type == true)
	{
		Ptr<UniformRandomVariable> uniformVar = CreateObject<UniformRandomVariable> ();
  	uniformVar->SetAttribute ("Min", DoubleValue (0.0));
  	uniformVar->SetAttribute ("Max", DoubleValue (255.0));
		m_macPanId = uniformVar->GetValue();
		m_PANCoordinator = true;
		
		//////////////////////////////////////////////////////
		//added by Ubines 20131126
		//set superframeOrder
		m_macSuperframeOrder = 4; //default superframe order : 4

		///////////////////////////////////////////////////
		//modified by Ubines 20130106
		//increase MinCapLength
		m_aMinCAPLength				= 440 * pow(2, m_macSuperframeOrder); //The minimum number of symbols forming the CAP.
		//end Ubines

		m_aBaseSlotDuration = m_aBaseSlotDuration * pow(2, m_macSuperframeOrder);
		//end Ubines
		return true; 
	}

	return false; 
}

//////////////////////////////////////////////////////////////////////////////
//added by Ubines 20131017 
//Send beacon 
//modified by Ubines 20131104
//specified beacon frame construction
void
LrWpanMac::SendOneBeacon (void)
{
	std::cout << m_PANCoordinator << " : " << "SendOneBeacon : " << std::endl;
	//////////////////////////////////
	//added by Ubines 20131227
	//pan coordinator can operate in slotted csma-ca
	m_csmaCa->SetSlottedCsmaCa();
	//end Ubines
	//std::cout << m_PANCoordinator << " : " << "send one beacon" << std::endl;
	NS_LOG_FUNCTION (this);
	LrWpanMacHeader macHdr (LrWpanMacHeader::LRWPAN_MAC_BEACON, (uint8_t)m_macBsn.GetValue ()); //change to m_macBsn
	LrWpanBeacon beaconHdr (m_macBeaconOrder);
	m_macBsn++;

	if (m_macBsn > SequenceNumber8 (255))
		m_macBsn = m_macBsn - SequenceNumber8 (255);

	if (SHORT_ADDR != 0xfffe && SHORT_ADDR != 0x0000)
	{
		macHdr.SetSrcAddrMode (SHORT_ADDR);
		macHdr.SetSrcAddrFields (GetPanId (), GetShortAddress ());
	}
	else
	{
		macHdr.SetSrcAddrMode (EXT_ADDR);
		macHdr.SetSrcAddrFields (GetPanId (), GetExtendedAddress ());
	}

	m_StartTime = m_macBeaconTxTime = Simulator::Now().GetMilliSeconds(); // Tick: Nano seconds
	beaconHdr.SetSupSpecBeaconOrder (m_macBeaconOrder);
	beaconHdr.SetSupSpecSuperOrder (m_macSuperframeOrder);
	beaconHdr.SetSupSpecFinalCAPSlot (m_FinalCAPSlot);
	beaconHdr.SetSupSpecBLE (m_BLE); 
	beaconHdr.SetSupSpecReserved (m_PANCoordinator);
	beaconHdr.SetSupSpecAssocPermit (m_macAssociationPermit); 

	if(m_GTSList.size() != 0)
	{
		beaconHdr.CreateGTSList(m_GTSList);
		/*
		for(std::vector<GTSList>::iterator IterPos = m_GTSList.begin(); IterPos != m_GTSList.end(); ++IterPos )
		{
			std::cout << m_PANCoordinator << " : " << "m_GTSStartSlot : " <<  (int)IterPos->m_GTSStartSlot << "\t" << "m_GTSLength : " << (int)IterPos->m_GTSLength << std::endl;;
		}
		*/
		m_GTSList.clear();
		m_FinalCAPSlot = 15;
		m_CAPLength = 15;
	}

	Ptr<Packet> p = Create<Packet>();

	////////////////////////////////////////////////
	//added by Ubines 20131119
	//add synchronization time using beacon frame
	SeqTsHeader seqTs;
	seqTs.SetSeq(m_StartTime);
	p->AddHeader(seqTs);
	//end Ubines
	////////////////////////////////////////////////
	
	p->AddHeader (beaconHdr);
	p->AddHeader (macHdr);
	GTSDeallocationForZero();//TODO : deallocation (Startng slot is zero)

	LrWpanMacTrailer macTrailer;
	p->AddTrailer (macTrailer);

	m_macTxTrace (p);

	/////////////////////////////////////////////////////////////
	//modified by Ubines 20131215
	//Direct transmission
	m_setMacState.Cancel ();
  ChangeMacState (MAC_IDLE);
  m_setMacState = Simulator::ScheduleNow (&LrWpanMac::DirectTransmission, this, p);
  //DirectTransmission(p);
	//end Ubines

	m_beacon_interval = CalculateSymbolToTime(m_aBaseSuperframeDuration * pow(2, m_macBeaconOrder));

	/////////////////////////////////////////////////////////////
	//modified by Ubines 20131219
	m_beaconEvent = Simulator::Schedule (Seconds(m_beacon_interval.GetSeconds()) , &LrWpanMac::SendOneBeacon, this);
	
	/////////////////////////////////////////////////
	//added by Ubines 20131227
	//PAN coordinator can transmit packet in slotted csma-ca
	if (m_txQueue.size() > 0)
	{
		if (m_csmaCa->IsSlottedCsmaCa ())
		{

			//m_txPkt = 0;
			m_txPkt = m_txQueue.front ()->txQPkt->Copy ();
			Ptr<UniformRandomVariable> uniformVar = CreateObject<UniformRandomVariable> ();
  		uniformVar->SetAttribute ("Min", DoubleValue (0.0));
  		uniformVar->SetAttribute ("Max", DoubleValue (0.01));
			Time randomStart = NanoSeconds (uniformVar->GetValue());
			Simulator::Schedule(randomStart, &LrWpanCsmaCa::Start, m_csmaCa);
		}else
		{
		}
	}
	//end Ubines
}
//end Ubines

/////////////////////////////////////////////////////////////////////////////////////////////
//added by Ubines 20131107
//deallocate for that starting slot is zero
void 
LrWpanMac::GTSDeallocationForZero(void)
{
	//std::cout << m_PANCoordinator << " : " << "GTSDeallocationForZero" << std::endl;
	int ZeroCount = 0;
	for(std::vector<GTSList>::iterator IterPos = m_GTSList.begin(); IterPos != m_GTSList.end(); ++IterPos )
	{
		if(IterPos->m_GTSStartSlot == 0)
		{
			ZeroCount ++;
		}
	}
	for(int i = 0; i < ZeroCount; i++)
	{
		for(std::vector<GTSList>::iterator IterPos = m_GTSList.begin(); IterPos != m_GTSList.end(); ++IterPos )
		{
			if(IterPos->m_GTSStartSlot == 0)
			{
				m_GTSList.erase(IterPos);
				break;
			}
		}
	}
}
//end Ubines

/////////////////////////////////////////
//added by Ubines 20131128
//Direct Transmission procedure
void
LrWpanMac::DirectTransmission (Ptr<Packet> directPkt)
{
	std::cout << m_PANCoordinator << " : " << "DirectTransmission" << std::endl;
	//NS_ASSERT (m_lrWpanMacState == MAC_IDLE);

	m_txPkt = directPkt;
  
  //std::cout << m_PANCoordinator << " : " << "state: " << m_lrWpanMacState << std::endl;

  ChangeMacState (MAC_SENDING);

	//std::cout << m_PANCoordinator << " : " << "state: " << m_lrWpanMacState << std::endl;
		
  m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_TX_ON);
	
}
//end Ubines

///////////////////////////////////////
//added by Ubines 
//calculate time from symbol
Time
LrWpanMac::CalculateSymbolToTime(double symbol)
{
	//std::cout << m_PANCoordinator << " : " << "CalculateSymbolToTime" << std::endl;
	//data rate : 250kbps, symbol rate : 62.5 ksymbol/s
	return Seconds(symbol / 62500);
}
//end Ubines

uint32_t NotReceivedBeaconNum = 0;
//////////////////////////////////////////////
//added by Ubines 20131226
//check beacon loss
void 
LrWpanMac::beaconCheck()
{
	//std::cout << m_PANCoordinator << " : " << "Beacon Check"<< std::endl; 
	NotReceivedBeaconNum++;
	if(NotReceivedBeaconNum >= m_aMaxLostBeacons)
	{
		NotReceivedBeaconNum = 0;
		m_associationStatus = DISASSOCIATED;
		m_shortAddress = Mac16Address ("ff:ff");
	}
	Simulator::Schedule (Seconds(m_beaconSearchDuration), &LrWpanMac::beaconCheck, this);
}
//end Ubines

////////////////////////////////////////
//added by Ubines
//get ont slot time
uint64_t
LrWpanMac::GetOneSlotTime(void)
{
	m_aBaseSlotDuration = 60 * pow(2, m_macSuperframeOrder);
	return m_aBaseSlotDuration; //return the number of symbol about slot duration
}
//end Ubines

////////////////////////////////////////////////////////////////////////
//added by Ubines 20131030
//modified by Ubines 20131121
//received beacon processing
void 
LrWpanMac::receivedBeaconProcess(LrWpanMacHeader receivedMacHdr, Ptr<Packet> p) 
{
	std::cout << m_PANCoordinator << " : " << "receivedBeaconProcess" << std::endl;
	std::cout << m_PANCoordinator << " : " << "my Association State is " << m_associationStatus << std::endl;
	/////////////////////////////////////////////////////////////////////
	//modified by Ubines 20131119
	//receive synchronization time by removing beacon header 
	LrWpanBeacon beaconHdr(1);
	p->RemoveHeader(beaconHdr);
	SeqTsHeader seqTs;
	p->RemoveHeader(seqTs);

	/////////////////////////////////////////////////////////////////////
	//modified by Ubines 20131106
	// content: etc. 
	m_macCoordShortAddress = receivedMacHdr.GetShortSrcAddr ();

	///////////////////////////////////////////////////////////////////
	//modified by Ubines 20131119
	//change m_macBeaconTxTime for setting synchronization time
	m_macBeaconTxTime = seqTs.GetSeq(); 

	///////////////////////////////////////////////////////////////////////
	//modified by Ubines 20131126
	//set parameter from beacon
	m_macBeaconOrder = beaconHdr.GetSupSpecBeaconOrder();
	m_FinalCAPSlot = beaconHdr.GetSupSpecFinalCAPSlot();
	m_macSuperframeOrder = beaconHdr.GetSupSpecSuperOrder();
	m_aBaseSlotDuration = 60 * pow(2, m_macSuperframeOrder);

	///////////////////////////////////////////////////////////////////////
	//added by Ubines 20140106
	//modify mincaplength
	m_aMinCAPLength				= 440 * pow(2, m_macSuperframeOrder); //The minimum number of symbols forming the CAP.
	//end Ubines
	
	if (m_associationStatus != ASSOCIATED)
	{
		MlmeAssociateRequestParams params;
		params.m_ChannelNumber = GetPhy()->GetChannelNumber();
		params.m_ChannelPage = 0;
		params.m_CoordAddrMode = receivedMacHdr.GetSrcAddrMode();
		if (params.m_CoordAddrMode == SHORT_ADDR)
		{
			params.m_CoordAddress = receivedMacHdr.GetShortSrcAddr ();
	    }
		params.m_CoordPANId = receivedMacHdr.GetSrcPanId();
		params.m_SecurityLevel = receivedMacHdr.GetSecLevel();
		params.m_KeyIdMode = receivedMacHdr.GetKeyIdMode();
		params.m_KeySource = receivedMacHdr.GetKeyIdSrc32();
		params.m_KeyIndex = receivedMacHdr.GetKeyIdIndex();
		params.m_DeviceType =  m_FullFunctionDevice;
		params.m_PowerSource = 0;
		params.m_ReceiverOnWhenIdle  = 1;
		params.m_SecurityCapability = 0;
		params.m_AllocateAddress  = 1;

		Simulator::ScheduleNow (&LrWpanMac::MlmeAssociateRequest, this, params);

	}else
	{	
		// already associated

		////////////////////////////////////////////////////////////////////
		// modifiec by Ubines 20131107
		// save GTS slot
		GTSSlotList.clear();

		/////////////////////////////////////////////////////////////////////////////////////////////
		//modified by Ubines 20131107
		//procedure of received GTS request command frame ack
		if(IsGTSBeaconTracking == true)
		{
			IsGTSBeaconTracking = false;
;
			std::vector<GTSList> gtsDescriptor = beaconHdr.GetGTSList2();		  
			for(std::vector<GTSList>::iterator IterPos = gtsDescriptor.begin(); IterPos != gtsDescriptor.end(); ++IterPos )
			{
				if(IterPos->m_GTSDevShortAddress == m_shortAddress)
				{
					//gtslist reset
					GTSList gtslist;
					gtslist.m_GTSDevShortAddress = IterPos->m_GTSDevShortAddress;
					gtslist.m_GTSStartSlot = IterPos->m_GTSStartSlot;
					gtslist.m_GTSLength = IterPos->m_GTSLength;
					
					MlmeGtsConfirmParams params;
					if(gtslist.m_GTSStartSlot == 0)
					{
						params.m_status = GTS_REQUEST_NO_DATA;
					}else
					{
						params.m_status = GTS_REQUEST_SUCCESS;
					}
					params.m_GTSLength = gtslist.m_GTSLength; //0-3 bit
					GTSSlotList.push_back(gtslist);//gtslist push
					MlmeGTSConfirm(params);
				}
			}
		}else
		{
			std::vector<GTSList> gtsDescriptor = beaconHdr.GetGTSList2();		  
			for(std::vector<GTSList>::iterator IterPos = gtsDescriptor.begin(); IterPos != gtsDescriptor.end(); ++IterPos )
			{
				if(IterPos->m_GTSDevShortAddress == m_shortAddress)
				{
					//gtslist reset
					GTSList gtslist;
					gtslist.m_GTSDevShortAddress = IterPos->m_GTSDevShortAddress;
					gtslist.m_GTSStartSlot = IterPos->m_GTSStartSlot;
					gtslist.m_GTSLength = IterPos->m_GTSLength;
					MlmeGtsConfirmParams params;
					if(gtslist.m_GTSStartSlot == 0)
					{
						params.m_status = GTS_REQUEST_NO_DATA;
						params.m_GTSLength = gtslist.m_GTSLength; //0-3 bit
						MlmeGTSConfirm(params);
					}else
					{
						GTSSlotList.push_back(gtslist);//gtslist push
					}
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////
	//modified by Ubines, 20131126
	//make function for superframe process
	OneSuperframeProcess(beaconHdr);
}

	///////////////////////////////////////////////////////
	//added by Ubines 20131030
MlmeAssociateResponseParams 
	LrWpanMac::updateDeviceDescriptorWithResponseParam(LrWpanMacHeader receivedMacHdr)
	{
		MlmeAssociateResponseParams param;
		param.m_AssocShortAddress="00:00";
	
		for(std::vector<DeviceDescriptor>::iterator IterPos = m_deviceDescriptorHandleList.begin(); IterPos != m_deviceDescriptorHandleList.end(); ++IterPos )
		{
			if (receivedMacHdr.GetExtSrcAddr() == IterPos->m_ExtAddress)
			{
				param.m_DeviceAddress=IterPos->m_ExtAddress;
				param.m_AssocShortAddress=IterPos->m_ShortAddress;
				param.m_status=ASSOCIATED;
				param.m_SecurityLevel=0;
				param.m_KeyIdMode=0;
				param.m_KeySource=0;
				param.m_KeyIndex=0;
				return param;
			}
		}
	
		DeviceDescriptor descriptor;
		descriptor.m_PANId = receivedMacHdr.GetSrcPanId();
		descriptor.m_ShortAddress = Mac16Address::Allocate ();
		descriptor.m_ExtAddress = receivedMacHdr.GetExtSrcAddr();
		descriptor.m_FrameCounter = 0;
		descriptor.m_Exempt = 0;
	
		m_deviceDescriptorHandleList.push_back (descriptor);
	
		param.m_DeviceAddress=descriptor.m_ExtAddress;
		param.m_AssocShortAddress=descriptor.m_ShortAddress;
		param.m_status=ASSOCIATED;
		param.m_SecurityLevel=0;
		param.m_KeyIdMode=0;
		param.m_KeySource=0;
		param.m_KeyIndex=0;
	
		return param;
	}	
	//end Ubines

//////////////////////////////////////////////
//added by Ubines 20131126
//procedure setting superframe
void
LrWpanMac::OneSuperframeProcess(LrWpanBeacon beaconHdr)
{
	////////////////////////////////////////////////////////////////////////
	//modified by Ubines 20131119
	//enqueue to GTSqueue
	if (!m_GTStxQueue.empty())
	{
		if (GTSSlotList.size() > 0)
		{
			for(std::vector<GTSList>::iterator IterPos = GTSSlotList.begin(); IterPos != GTSSlotList.end(); ++IterPos )
			{
				if(m_GTStxQueue.size() < 1)
				{
					break;
				}
				
				for(int i = 0; i < (int)IterPos->m_GTSLength ; i++)
				{
					if(m_GTStxQueue.size() < 1)
					{
						break;
					}
					if(IterPos->m_GTSStartSlot + i > 16)
					{
						std::cout << m_PANCoordinator << " : " << "Error : out of superframe" << std::endl;
						break;
					}
					Ptr<Packet> p = m_GTStxQueue.front ()->txQPkt->Copy ();
					m_GTStxQueue.pop_front();
					
					Simulator::Schedule (Seconds(CalculateSymbolToTime((IterPos->m_GTSStartSlot + i) * m_aBaseSlotDuration).GetSeconds()), &LrWpanMac::DirectTransmission, this, p);
				}
			}

		}else
		{
			///////////////////////////////////////////////////////
			//added by Ubines 20140106
			//if there is not allocated Gts slot and is packet in GTStxqueue, send GTS request to PANC
			Ptr<UniformRandomVariable> uniformVar = CreateObject<UniformRandomVariable> ();
  		uniformVar->SetAttribute ("Min", DoubleValue (0.0));
  		uniformVar->SetAttribute ("Max", DoubleValue (3000.0));
			Time randomStart = NanoSeconds(uniformVar->GetValue());
			Simulator::Schedule (randomStart, &LrWpanMac::SendGTSAllocationRequest, this);
			//end Ubines
		}
	}else
	{
		//no-operation because there is not sending packet in CFP period
	}

	if (m_txQueue.size() > 0)
	{
		if (m_csmaCa->IsSlottedCsmaCa ())
		{
			m_txPkt = m_txQueue.front ()->txQPkt->Copy ();
			Ptr<UniformRandomVariable> uniformVar = CreateObject<UniformRandomVariable> ();
  		uniformVar->SetAttribute ("Min", DoubleValue (0.0));
  		uniformVar->SetAttribute ("Max", DoubleValue (100.0));
			Time randomStart = NanoSeconds (uniformVar->GetValue());
			Simulator::Schedule(randomStart, &LrWpanCsmaCa::Start, m_csmaCa);
		}else
		{
		}
	}
	//end Ubines
}

/////////////////////////////////////////////////////////////////////////////////////////////
//added by Ubines 20131101
//Send GTS allocation request control frame
//modified by Ubines 20131105
void 
LrWpanMac::SendGTSAllocationRequest(void){
	MlmeGtsRequestParams GTSRequestParam;
	GTSRequestParam.m_CharacteristicsType = 1; //The Characteristics Type field of the GTS Characteristics field of the request shall be set to one (GTS allocation),
	GTSRequestParam.m_GTSDirection = 0	; //1 : receive-only, 0 : transmit-only
	///////////////////////////////////////////////////////
	//added by Ubines 20131120
	Time txTime = CalculateTxTime();
	Time slot_interval  = CalculateSymbolToTime(GetOneSlotTime()); 
	GTSRequestParam.m_GTSLength = (int)(txTime.GetSeconds() / (slot_interval.GetSeconds()) + 1);
	MlmeGtsRequest(GTSRequestParam);
}
//end Ubines

/////////////////////////////////////////////////////////
//added by Ubines 20131120
//calculate GTS slot time for GTS allocation 
Time
LrWpanMac::CalculateTxTime(void){
	uint32_t packetSize = m_GTStxQueue.size() * 100;
	return GetPhy()->CalculateGTSTxTime(packetSize);
}
//end Ubines

////////////////////////////////////////////////////////////////////////
//added by Ubines 20131031
//send association request and response message
void 
LrWpanMac::MlmeAssociateRequest(MlmeAssociateRequestParams params){
	std::cout << m_PANCoordinator << " : " << "MlmeAssociationRequest" << std::endl;
	LrWpanMacCommandFrame CommandHdr (1);
	LrWpanMacHeader macHdr (LrWpanMacHeader::LRWPAN_MAC_COMMAND, (uint8_t)m_macDsn.GetValue ());
	m_macDsn++;

	if (m_macDsn > SequenceNumber8 (255))
    {
		m_macDsn = m_macDsn - SequenceNumber8 (255);
    }

	macHdr.SetDstAddrFields(params.m_CoordPANId, params.m_CoordAddress);
	macHdr.SetSrcAddrFields(GetPanId (), GetExtendedAddress());
	macHdr.SetSrcAddrMode (EXT_ADDR); //Source Addressing Mode field - extended addressing
	macHdr.SetDstAddrMode (params.m_CoordAddrMode); //same mode as indicated in the beacon frame to which the association request command
	macHdr.SetNoFrmPend(); //frame pending field shall be set to zero
	macHdr.SetAckReq(); //AR rield shall be set to one

	CommandHdr.MakeAssociationRequestFrame(params.m_DeviceType,
											params.m_PowerSource,
											params.m_ReceiverOnWhenIdle,
											params.m_SecurityCapability,
											params.m_AllocateAddress);

	Ptr<Packet> p = Create<Packet>();
	p->AddHeader(CommandHdr);
	p->AddHeader(macHdr);

	LrWpanMacTrailer macTrailer;
	p->AddTrailer (macTrailer);

//modified by JW
	m_macTxEnqueueTrace (p);
	
	TxQueueElement *txQElement = new TxQueueElement;
	txQElement->txQPkt = p;
	m_txQueue.push_back (txQElement);
	CheckQueue ();
//end JW
/*
	m_macTxTrace (p);

	TxQueueElement *txQElement = new TxQueueElement;
	txQElement->txQPkt = p->Copy();

	/////////////////////////////////////////////////////
	//modified by Ubines 20131217
	//scheduling
	if (m_txQueue.size() < 1)
    {
   
		m_txPkt = txQElement->txQPkt->Copy ();
		m_txQueue.push_front (txQElement); // the first packet in the queue is currently worked on
		//////////////////////////////////////////////////////////
		//modified by Ubines 20131230
		double currentTime = Simulator::Now().GetSeconds();
		double beaconReceiveTime = MilliSeconds(m_macBeaconTxTime).GetSeconds();
		double SuperframeTime = currentTime - beaconReceiveTime;
		double oneSlotTime = CalculateSymbolToTime(GetOneSlotTime()).GetSeconds();
		if((int)(SuperframeTime / oneSlotTime) <=  m_FinalCAPSlot)
		{
			Simulator::ScheduleNow (&LrWpanCsmaCa::Start, m_csmaCa);
		}
		//end Ubines
    }
	else
    {
		///////////////////////////////////////////////////////////////////////////
		//modified by Ubines 20131107
		//command priority control (queue reordering)
		std::deque<TxQueueElement*>::iterator it = m_txQueue.begin();
		if (m_txQueue.size() >= 1)
		{
	  		++it;
    		it = m_txQueue.insert (it, txQElement); // beacon is highest priority packet
		}
		else
			m_txQueue.push_back (txQElement);
		}
		*/
	
}

void
LrWpanMac::MlmeAssociateResponse(MlmeAssociateResponseParams param, LrWpanMacHeader receivedMacHdr)	{
	std::cout << m_PANCoordinator << " : " << "MlmeAssociationResponse" << std::endl;
	
	LrWpanMacCommandFrame commandHdr (1);
	LrWpanMacHeader macHdr (LrWpanMacHeader::LRWPAN_MAC_COMMAND, (uint8_t)m_macDsn.GetValue ());
	m_macDsn++;
	if (m_macDsn > SequenceNumber8 (255))
    {
		m_macDsn = m_macDsn - SequenceNumber8 (255);
    }

	macHdr.SetSrcAddrFields(GetPanId (), GetExtendedAddress());
	macHdr.SetDstAddrFields(receivedMacHdr.GetSrcPanId(), receivedMacHdr.GetExtSrcAddr());
	macHdr.SetSrcAddrMode (EXT_ADDR); //Source Addressing Mode field - extended addressing
	macHdr.SetDstAddrMode (EXT_ADDR); //same mode as indicated in the beacon frame to which the association request command
	macHdr.SetNoFrmPend(); //frame pending field shall be set to zero
	macHdr.SetAckReq(); //AR rield shall be set to one

	commandHdr.MakeAssociationResponseFrame(param.m_AssocShortAddress, param.m_status);
	
	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(commandHdr);
	packet->AddHeader(macHdr);

	LrWpanMacTrailer macTrailer;
	packet->AddTrailer (macTrailer);
	
	m_macTxEnqueueTrace (packet);
	
	TxQueueElement *txQElement = new TxQueueElement;
	txQElement->txQPkt = packet;
	m_txQueue.push_back (txQElement);
	CheckQueue ();
	/*
	m_macTxTrace (packet);

	TxQueueElement *txQElement = new TxQueueElement;
	txQElement->txQPkt = packet;

	if (m_txQueue.empty ())
    {
		m_txPkt = txQElement->txQPkt->Copy ();
		m_txQueue.push_front (txQElement); // the first packet in the queue is currently worked on
		
		/////////////////////////////////////////////////////////
		//modified by Ubines 20131230
		double currentTime = Simulator::Now().GetSeconds();
		double beaconReceiveTime = MilliSeconds(m_macBeaconTxTime).GetSeconds();
		double SuperframeTime = currentTime - beaconReceiveTime;
		double oneSlotTime = CalculateSymbolToTime(GetOneSlotTime()).GetSeconds();
		if((int)(SuperframeTime / oneSlotTime) <=  m_FinalCAPSlot)
		{
			Simulator::ScheduleNow (&LrWpanCsmaCa::Start, m_csmaCa);
		}
		//end Ubines
    }
	else
    {
		m_txQueue.push_back (txQElement);
		}
	*/
}
//end Ubines
///////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
//added by Ubines 20131105
//MlmeGtsRequest
void
LrWpanMac::MlmeGtsRequest(MlmeGtsRequestParams GTSReqParam)
{
	Ptr<Packet> packet = Create<Packet>();
	LrWpanMacCommandFrame commandHdr(1);
	LrWpanMacHeader macHdr (LrWpanMacHeader::LRWPAN_MAC_COMMAND, (uint8_t)m_macDsn.GetValue ());
	macHdr.SetSrcAddrMode(SHORT_ADDR);
	macHdr.SetSrcAddrFields(m_macPanId, m_shortAddress);
	macHdr.SetNoFrmPend();
	macHdr.SetAckReq();
	commandHdr.MakeGTSRequestFrame(GTSReqParam.m_GTSLength,GTSReqParam.m_GTSDirection,GTSReqParam.m_CharacteristicsType);
	packet->AddHeader(commandHdr);
	packet->AddHeader(macHdr);

	LrWpanMacTrailer macTrailer;
	packet->AddTrailer (macTrailer);
	m_macTxTrace (packet);

	TxQueueElement *txQElement = new TxQueueElement;
	txQElement->txQPkt = packet->Copy();

	if (m_txQueue.size() < 1)
    {
		double currentTime = Simulator::Now().GetSeconds();
		double beaconReceiveTime = MilliSeconds(m_macBeaconTxTime).GetSeconds();
		double SuperframeTime = currentTime - beaconReceiveTime;
		double oneSlotTime = CalculateSymbolToTime(GetOneSlotTime()).GetSeconds();
		if((int)(SuperframeTime / oneSlotTime) <=  m_FinalCAPSlot)
		{
			m_txPkt = txQElement->txQPkt->Copy ();
			m_txQueue.push_front (txQElement); // the first packet in the queue is currently worked on
			Ptr<UniformRandomVariable> uniformVar = CreateObject<UniformRandomVariable> ();
  		uniformVar->SetAttribute ("Min", DoubleValue (0.0));
  		uniformVar->SetAttribute ("Max", DoubleValue (3000.0));
			Time randomStart = NanoSeconds (uniformVar->GetValue());
			Simulator::Schedule(randomStart, &LrWpanCsmaCa::Start, m_csmaCa);		
		}
		else
		{
			// if queue is not empty put this packet at the end of Tx queue
			m_txQueue.push_back (txQElement);
		}
    }
	else
    {
		// if queue is not empty put this packet at the end of Tx queue
		m_txQueue.push_back (txQElement);
    }
}

///////////////////////////////////////////
//added by Ubines
//check whehter short address already exist or not
bool
LrWpanMac::IsGetDeviceShortAddress(LrWpanMacHeader receivedMacHdr)
{
	for(std::vector<DeviceDescriptor>::iterator IterPos = m_deviceDescriptorHandleList.begin(); IterPos != m_deviceDescriptorHandleList.end(); ++IterPos )
	{	
		if (receivedMacHdr.GetShortSrcAddr() == IterPos->m_ShortAddress)
		{
			return true;
		}
	}
	return false;
}
//end Ubines

//////////////////////////////////////////////
//added by Ubines 20131106
//deallocate GTS slot
void
LrWpanMac::GTSDeallocation(Mac16Address DeviceAddress, int8_t GTSLength)
{
	for(std::vector<GTSList>::iterator IterPos = m_GTSList.begin(); IterPos != m_GTSList.end(); ++IterPos )
	{
		if (DeviceAddress == IterPos->m_GTSDevShortAddress )
		{
			m_GTSList.erase(IterPos);
			m_CAPLength = m_CAPLength + GTSLength;
			m_FinalCAPSlot = m_CAPLength;
			return;
		}
	}
}
//end Ubines

//////////////////////////////////////////////
//modified by Ubines 20131121
//procedure function of received GTS request
void
LrWpanMac::receivedGTSRequest(LrWpanMacHeader receivedMacHdr, LrWpanMacCommandFrame receivedCommandHdr)
{
	//////////////////////////////////////////////
	//added by Ubines 20131106
	if(m_PANCoordinator == true && IsGetDeviceShortAddress(receivedMacHdr))
	{
		if(receivedCommandHdr.GetCharacteristicType() == 0)
		{
			GTSDeallocation(receivedMacHdr.GetShortSrcAddr(),receivedCommandHdr.GetGTSLength());
			MlmeGtsIndicationParams params;
			params.m_DeviceAddress = receivedMacHdr.GetShortSrcAddr();
			params.m_GTSLength = receivedCommandHdr.GetGTSLength();
			params.m_GTSDirection = receivedCommandHdr.GetGTSDirection();
			params.m_CharacteristicsType = receivedCommandHdr.GetCharacteristicType();
			MlmeGTSIndication(params);
			return;
		}
		
		//std::cout << m_PANCoordinator << " : " << "((m_CAPLength - receivedCommandHdr.GetGTSLength()) * m_aBaseSlotDuration) : " << ((m_CAPLength - receivedCommandHdr.GetGTSLength()) * m_aBaseSlotDuration) << std::endl;
		//std::cout << m_PANCoordinator << " : " << "m_aMinCAPLength : " << m_aMinCAPLength << std::endl;

		if((((m_CAPLength - receivedCommandHdr.GetGTSLength()) * m_aBaseSlotDuration) > m_aMinCAPLength) && m_GTSList.size() < 7)
		{
			//std::cout << m_PANCoordinator << " : " << "((m_CAPLength - receivedCommandHdr.GetGTSLength()) * m_aBaseSlotDuration) : " << ((m_CAPLength - receivedCommandHdr.GetGTSLength()) * m_aBaseSlotDuration) << std::endl;
			
			GTSList gtsDescriptorList;
			gtsDescriptorList.m_GTSDevShortAddress = receivedMacHdr.GetShortSrcAddr();
			gtsDescriptorList.m_GTSStartSlot = m_aNumSuperframeSlots - receivedCommandHdr.GetGTSLength();
			gtsDescriptorList.m_GTSLength = receivedCommandHdr.GetGTSLength();
			m_CAPLength = m_CAPLength - receivedCommandHdr.GetGTSLength();
			
			for(std::vector<GTSList>::iterator IterPos = m_GTSList.begin(); IterPos != m_GTSList.end(); ++IterPos )
			{
				/////////////////////////////////////////////////////////////
				//modified by Ubines 20131226
				//if allocated GTS already exist, prevent GTS reallocation
				if(IterPos->m_GTSDevShortAddress == gtsDescriptorList.m_GTSDevShortAddress)
					return;
				//end Ubines
				IterPos->m_GTSStartSlot -= receivedCommandHdr.GetGTSLength();
			}
			
			m_GTSList.push_back (gtsDescriptorList);
			Time deleteTime = CalculateSymbolToTime(m_aGTSDescPersistenceTime * m_aBaseSuperframeDuration * pow(2, m_macBeaconOrder));
			Simulator::Schedule (deleteTime, &LrWpanMac::GTSDeallocation, this, receivedMacHdr.GetShortSrcAddr(),
									receivedCommandHdr.GetGTSLength());

			//GTS allocation indication
			MlmeGtsIndicationParams params;
			params.m_DeviceAddress = receivedMacHdr.GetShortSrcAddr();
			params.m_GTSLength = receivedCommandHdr.GetGTSLength();
			params.m_GTSDirection = receivedCommandHdr.GetGTSDirection();
			params.m_CharacteristicsType = receivedCommandHdr.GetCharacteristicType();
			MlmeGTSIndication(params);
			m_FinalCAPSlot = m_CAPLength;
			return;
		}
		else
		{
			//fail gts allocation
			GTSList gtsDescriptorList;
			gtsDescriptorList.m_GTSDevShortAddress = receivedMacHdr.GetShortSrcAddr();
			gtsDescriptorList.m_GTSStartSlot = 0;
			gtsDescriptorList.m_GTSLength = m_CAPLength - (m_aMinCAPLength / m_aBaseSlotDuration) + 1;
	
			//GTS allocation indication
			MlmeGtsIndicationParams params;
			params.m_DeviceAddress = receivedMacHdr.GetShortSrcAddr();
			params.m_GTSLength = m_CAPLength - (m_aMinCAPLength / m_aBaseSlotDuration) + 1;
			params.m_GTSDirection = receivedCommandHdr.GetGTSDirection();
			params.m_CharacteristicsType = receivedCommandHdr.GetCharacteristicType();
			MlmeGTSIndication(params);
			return;
		}
	}
	else
	{
		//no-op
		return;
	}
}
//end Ubines

////////////////////////////////////////////////
//added by Ubines
//GTS Indication
void
LrWpanMac::MlmeGTSIndication(MlmeGtsIndicationParams params)
{
	if(View)
	{
		std::cout << m_PANCoordinator << " : " << "===MLME GTS INDICATION===" << std::endl;
		std::cout << m_PANCoordinator << " : " << "allocated device's address : " << params.m_DeviceAddress << std::endl;
		std::cout << m_PANCoordinator << " : " << "GTS length : " << (int)params.m_GTSLength << std::endl; //0-3 bit
		if(params.m_GTSDirection == 0)
		{
			std::cout << m_PANCoordinator << " : " << "GTS direction : transmit-only GTS slot" << std::endl;
		}
		else if(params.m_GTSDirection == 1)
		{ // 1 : receive-only, 0 : transmit-only
			std::cout << m_PANCoordinator << " : " << "GTS direction : receive-only GTS slot" << std::endl;
		}
		else
		{
			std::cout << m_PANCoordinator << " : " << "error" << std::endl;; //4bit
		}
		
		if(params.m_CharacteristicsType == 1)
		{
			std::cout << m_PANCoordinator << " : " << "CharacteristicsType : Allocation" << std::endl;
		}
		else if(params.m_CharacteristicsType == 0)
		{
			std::cout << m_PANCoordinator << " : " << "CharacteristicsType : Deallocation" << std::endl;
		}
		else
		{
			std::cout << m_PANCoordinator << " : " << "error" << std::endl;; //4bit
		}
		std::cout << m_PANCoordinator << " : " << "=========================" << std::endl;
	}
}
//end Ubines

//////////////////////////////////////////////
//added by Ubines 20131107
//GTS Confirm
void
LrWpanMac::MlmeGTSConfirm(MlmeGtsConfirmParams params)
{
	if(View)
	{
		std::cout << m_PANCoordinator << " : " << "===MLME GTS CONFIRM===" << std::endl;
		if(params.m_status == GTS_REQUEST_SUCCESS)
		{
			std::cout << m_PANCoordinator << " : " << "GTS request success" << std::endl;
			std::cout << m_PANCoordinator << " : " << "Size of allocated GTS slot : " << (int)params.m_GTSLength << std::endl; //0-3 bit
		}else if(params.m_status == GTS_REQUEST_NO_DATA)
		{
			std::cout << m_PANCoordinator << " : " << "GTS reqeust fail(no capacity)" << std::endl;
		}else if(params.m_status == GTS_REQUEST_NO_ACK)
		{
			std::cout << m_PANCoordinator << " : " << "GTS request fail(no ack)" << std::endl;
		}else
		{
			std::cout << m_PANCoordinator << " : " << "ERROR" << std::endl;
		}
		std::cout << m_PANCoordinator << " : " << "======================" << std::endl;
	}
}
//end Ubines

void
LrWpanMac::DoInitialize ()
{
	//std::cout << m_PANCoordinator << " : " << "DoInitialize" << std::endl;
  if (m_macRxOnWhenIdle)
    {
      m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_RX_ON);
    }
  else
    {
      m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_TRX_OFF);
    }

  Object::DoInitialize ();
}

void
LrWpanMac::DoDispose ()
{
	//std::cout << m_PANCoordinator << " : " << "DoDispose" << std::endl;
  if (m_csmaCa != 0)
    {
      m_csmaCa->Dispose ();
      m_csmaCa = 0;
    }
  m_txPkt = 0;
  for (uint32_t i = 0; i < m_txQueue.size (); i++)
    {
      m_txQueue[i]->txQPkt = 0;
      delete m_txQueue[i];
    }
  m_txQueue.clear ();
  m_phy = 0;
  m_mcpsDataIndicationCallback = MakeNullCallback< void, McpsDataIndicationParams, Ptr<Packet> > ();
  m_mcpsDataConfirmCallback = MakeNullCallback< void, McpsDataConfirmParams > ();

  Object::DoDispose ();
}

bool
LrWpanMac::GetRxOnWhenIdle ()
{
	//std::cout << m_PANCoordinator << " : " << "GetRxOnWhenIdle" << std::endl;
  return m_macRxOnWhenIdle;
}

void
LrWpanMac::SetRxOnWhenIdle (bool rxOnWhenIdle)
{
	//std::cout << m_PANCoordinator << " : " << "SetRxOnWhenIdle" << std::endl;
  NS_LOG_FUNCTION (this << rxOnWhenIdle);
  m_macRxOnWhenIdle = rxOnWhenIdle;

  if (m_lrWpanMacState == MAC_IDLE)
    {
      if (m_macRxOnWhenIdle)
        {
          m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_RX_ON);
        }
      else
        {
          m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_TRX_OFF);
        }
    }
}

void
LrWpanMac::SetShortAddress (Mac16Address address)
{
	//std::cout << m_PANCoordinator << " : " << "SetShortAddress" << std::endl;
  //NS_LOG_FUNCTION (this << address);
  m_shortAddress = address;
}

void
LrWpanMac::SetExtendedAddress (Mac64Address address)
{
	//std::cout << m_PANCoordinator << " : " << "SetExtendedAddress" << std::endl;
  //NS_LOG_FUNCTION (this << address);
  m_selfExt = address;
}


Mac16Address
LrWpanMac::GetShortAddress () const
{
	//std::cout << m_PANCoordinator << " : " << "GetShortAddress" << std::endl;
  NS_LOG_FUNCTION (this);
  return m_shortAddress;
}

Mac64Address
LrWpanMac::GetExtendedAddress () const
{
	//std::cout << m_PANCoordinator << " : " << "GetExtendedAddress" << std::endl;
  NS_LOG_FUNCTION (this);
  return m_selfExt;
}
void
LrWpanMac::McpsDataRequest (McpsDataRequestParams params, Ptr<Packet> p)
{
	std::cout << m_PANCoordinator << " : " << "McpsDataRequest" << std::endl;
  NS_LOG_FUNCTION (this << p);

  McpsDataConfirmParams confirmParams;
  confirmParams.m_msduHandle = params.m_msduHandle;

  // TODO: We need a drop trace for the case that the packet is too large or the request parameters are maleformed.
  //       The current tx drop trace is not suitable, because packets dropped using this trace carry the mac header
  //       and footer, while packets being dropped here do not have them.

  LrWpanMacHeader macHdr (LrWpanMacHeader::LRWPAN_MAC_DATA, m_macDsn.GetValue ());
  m_macDsn++;

  if (p->GetSize () > LrWpanPhy::aMaxPhyPacketSize - aMinMPDUOverhead)
    {
      // Note, this is just testing maximum theoretical frame size per the spec
      // The frame could still be too large once headers are put on
      // in which case the phy will reject it instead
      NS_LOG_ERROR (this << " packet too big: " << p->GetSize ());
      std::cout << m_PANCoordinator << " : " << "packet too big" << std::endl;
      confirmParams.m_status = IEEE_802_15_4_FRAME_TOO_LONG;
      if (!m_mcpsDataConfirmCallback.IsNull ())
        {
          m_mcpsDataConfirmCallback (confirmParams);
        }
      return;
    }

  if ((params.m_srcAddrMode == NO_PANID_ADDR)
      && (params.m_dstAddrMode == NO_PANID_ADDR))
    {
    	std::cout << m_PANCoordinator << " : " << "Can not send packet with no Address field" << std::endl;
      NS_LOG_ERROR (this << " Can not send packet with no Address field" );
      confirmParams.m_status = IEEE_802_15_4_INVALID_ADDRESS;
      if (!m_mcpsDataConfirmCallback.IsNull ())
        {
          m_mcpsDataConfirmCallback (confirmParams);
        }
      return;
    }
  switch (params.m_srcAddrMode)
    {
    case NO_PANID_ADDR:
      macHdr.SetSrcAddrMode (params.m_srcAddrMode);
      macHdr.SetNoPanIdComp ();
      break;
    case ADDR_MODE_RESERVED:
      macHdr.SetSrcAddrMode (params.m_srcAddrMode);
      break;
    case SHORT_ADDR:
      macHdr.SetSrcAddrMode (params.m_srcAddrMode);
      macHdr.SetSrcAddrFields (GetPanId (), GetShortAddress ());
      break;
    case EXT_ADDR:
      macHdr.SetSrcAddrMode (params.m_srcAddrMode);
      macHdr.SetSrcAddrFields (GetPanId (), GetExtendedAddress ());
      break;
    default:
      NS_LOG_ERROR (this << " Can not send packet with incorrect Source Address mode = " << params.m_srcAddrMode);
      std::cout << m_PANCoordinator << " : " << "Can not send packet with incorrect Source Address mode" << std::endl;
      confirmParams.m_status = IEEE_802_15_4_INVALID_ADDRESS;
      if (!m_mcpsDataConfirmCallback.IsNull ())
        {
          m_mcpsDataConfirmCallback (confirmParams);
        }
      return;
    }

  macHdr.SetDstAddrMode (params.m_dstAddrMode);
  // TODO: Add field for EXT_ADDR destination address (and use it here).
  macHdr.SetDstAddrFields (params.m_dstPanId, params.m_dstAddr);
  macHdr.SetSecDisable ();
  //extract the last 3 bits in TxOptions and map to macHdr
  int b0 = params.m_txOptions & TX_OPTION_ACK;
  int b1 = params.m_txOptions & TX_OPTION_GTS;
  int b2 = params.m_txOptions & TX_OPTION_INDIRECT;
  if (b0 == TX_OPTION_ACK)
    {
      // Set AckReq bit only if the destination is not the broadcast address.
      if (!(macHdr.GetDstAddrMode () == SHORT_ADDR && macHdr.GetShortDstAddr () == "ff:ff"))
        {
          macHdr.SetAckReq ();
        }
    }
  else if (b0 == 0)
    {
      macHdr.SetNoAckReq ();
    }
  else
    {
      confirmParams.m_status = IEEE_802_15_4_INVALID_PARAMETER;
      NS_LOG_ERROR (this << "Incorrect TxOptions bit 0 not 0/1");
      std::cout << m_PANCoordinator << " : " << "Incorrect TxOptions bit 0 not 0/1" << std::endl;
      if (!m_mcpsDataConfirmCallback.IsNull ())
        {
          m_mcpsDataConfirmCallback (confirmParams);
        }
      return;
    }

  //if is Slotted CSMA means its beacon enabled
  if (m_csmaCa->IsSlottedCsmaCa ())
    {
      if (b1 == TX_OPTION_GTS)
        {
          //TODO:GTS Transmission
          std::cout << m_PANCoordinator << " : " << "GTS Transmission" << std::endl;
          ////////////////////////////////////////////////////////////////////////////////////////////////////////////
					//added by Ubines 20131119
					//enqueue to GTSqueue for CFP
					if(m_associationStatus == ASSOCIATED && GTSSlotList.size() > 0) //there already is GTS slot --> enqueue to GTStxQueue
					{
						
						//TODO:GTS Transmission
						p->AddHeader (macHdr);
						LrWpanMacTrailer macTrailer;
						p->AddTrailer (macTrailer);
		
						m_macTxTrace (p);
						confirmParams.m_status = IEEE_802_15_4_SUCCESS;
		
						if (!m_mcpsDataConfirmCallback.IsNull ())
						{
							m_mcpsDataConfirmCallback (confirmParams);
						}
		
						TxQueueElement *txQElement = new TxQueueElement;
						txQElement->txQPkt = p->Copy();
						if(m_GTStxQueue.empty()) //if there is not GTS slot
						{
							//std::cout << m_PANCoordinator << " : " << "m_GTStxQueue.size() : " << std::endl;
							m_GTStxQueue.push_front(txQElement);
						}
						else
						{
					  		m_GTStxQueue.push_back(txQElement);
						}
					}
					else 
					{
						//there is not GTS slot still --> send GTS request and enqueue data to gtstxqueue
						if(IsSendingGTSRequest == false)
						{
							IsSendingGTSRequest = true;
							SendGTSAllocationRequest();
						}
						
						p->AddHeader (macHdr);
						LrWpanMacTrailer macTrailer;
						p->AddTrailer (macTrailer);
		
						m_macTxTrace (p);
						confirmParams.m_status = IEEE_802_15_4_SUCCESS;
		
						if (!m_mcpsDataConfirmCallback.IsNull ())
						{
							m_mcpsDataConfirmCallback (confirmParams);
						}
						TxQueueElement *txQElement = new TxQueueElement;
						txQElement->txQPkt = p->Copy();
		
						if(m_GTStxQueue.empty()) //if there is not GTS slot
						{
							m_GTStxQueue.push_front(txQElement);
						}
						else
						{
					  		m_GTStxQueue.push_back(txQElement);
						}
						
					}
        }
      else if (b1 == 0)
        {
          //TODO:CAP Transmission
          std::cout << m_PANCoordinator << " : " << "CAP Transmission" << std::endl;
          ////////////////////////////////////////////////////////////
					//added by Ubines 20131127
					//modify Txqueue for CAP
		
					p->AddHeader (macHdr);
					LrWpanMacTrailer macTrailer;
					p->AddTrailer (macTrailer);
		
					m_macTxTrace (p);
					confirmParams.m_status = IEEE_802_15_4_SUCCESS;
		
					if (!m_mcpsDataConfirmCallback.IsNull ())
					{
						m_mcpsDataConfirmCallback (confirmParams);
					}
					if(m_associationStatus == ASSOCIATED)
					{		
						
						TxQueueElement *txQElement = new TxQueueElement;
						txQElement->txQPkt = p->Copy();
						if(m_txQueue.size() < 1)
						{
							double currentTime = Simulator::Now().GetSeconds();
							double beaconReceiveTime = MilliSeconds(m_macBeaconTxTime).GetSeconds();
							double SuperframeTime = currentTime - beaconReceiveTime;
							double oneSlotTime = CalculateSymbolToTime(GetOneSlotTime()).GetSeconds();
							if((int)(SuperframeTime / oneSlotTime) <=  m_FinalCAPSlot)
							{
								m_txPkt = p->Copy();
								m_txQueue.push_front(txQElement);
								
								Ptr<UniformRandomVariable> uniformVar = CreateObject<UniformRandomVariable> ();
  							uniformVar->SetAttribute ("Min", DoubleValue (0.0));
  							uniformVar->SetAttribute ("Max", DoubleValue (0.001));
								Time randomStart = NanoSeconds (uniformVar->GetValue());
								Simulator::Schedule(randomStart, &LrWpanCsmaCa::Start, m_csmaCa);
							}
							else
							{
								m_txPkt = p->Copy();
								m_txQueue.push_front(txQElement);
							}
						}
						else
						{
					  		m_txQueue.push_back(txQElement);
						}
						
					}
					//end Ubines
					////////////////////////////////////////////
        }
      else
        {
          NS_LOG_ERROR (this << "Incorrect TxOptions bit 1 not 0/1");
          confirmParams.m_status = IEEE_802_15_4_INVALID_PARAMETER;
          if (!m_mcpsDataConfirmCallback.IsNull ())
            {
              m_mcpsDataConfirmCallback (confirmParams);
            }
          return;
        }
    }
  else
    {
      if (b1 != 0)
        {
          NS_LOG_ERROR (this << "for non-beacon-enables PAN, bit 1 should always be set to 0");
          confirmParams.m_status = IEEE_802_15_4_INVALID_PARAMETER;
          if (!m_mcpsDataConfirmCallback.IsNull ())
            {
              m_mcpsDataConfirmCallback (confirmParams);
            }
          return;
        }
      else
				{
					////////////////////////////////////////////////////////
					//added by Ubines 20131227
					//send packet in unslotted csma-ca
					//std::cout << m_PANCoordinator << " : " << "aaa" << std::endl;
					//macHdr.SetDstAddrFields (params.m_dstPanId, Mac16Address ("00:01"));
					confirmParams.m_status = IEEE_802_15_4_SUCCESS;
					if (!m_mcpsDataConfirmCallback.IsNull ())
					{
						m_mcpsDataConfirmCallback (confirmParams);
					}
		
		
					p->AddHeader (macHdr);
					LrWpanMacTrailer macTrailer;
					p->AddTrailer (macTrailer);
		
					m_macTxTrace (p);
		
					TxQueueElement *txQElement = new TxQueueElement;
					txQElement->txQMsduHandle = params.m_msduHandle;
					txQElement->txQPkt = p;
		
					if (m_txQueue.empty ())
					{
						m_txPkt = txQElement->txQPkt->Copy ();
						m_txQueue.push_front (txQElement); // the first packet in the queue is currently worked on
						Simulator::ScheduleNow (&LrWpanCsmaCa::Start, m_csmaCa);
					}
					else
					{
						// if queue is not empty put this packet at the end of Tx queue
						m_txQueue.push_back (txQElement);
					}
					//end Ubines
					/////////////////////////////////////////////////////
				}
    }

  if (b2 == TX_OPTION_INDIRECT)
    {
      //TODO :indirect tx
    }
  else if (b2 == 0)
    {
      //TODO :direct tx
    }
  else
    {
      NS_LOG_ERROR (this << "Incorrect TxOptions bit 2 not 0/1");
      confirmParams.m_status = IEEE_802_15_4_INVALID_PARAMETER;
      if (!m_mcpsDataConfirmCallback.IsNull ())
        {
          m_mcpsDataConfirmCallback (confirmParams);
        }
      return;
    }

  p->AddHeader (macHdr);

  LrWpanMacTrailer macTrailer;
  // Calculate FCS if the global attribute ChecksumEnable is set.
  if (Node::ChecksumEnabled ())
    {
      macTrailer.EnableFcs (true);
      macTrailer.SetFcs (p);
    }
  p->AddTrailer (macTrailer);

  m_macTxEnqueueTrace (p);

  TxQueueElement *txQElement = new TxQueueElement;
  txQElement->txQMsduHandle = params.m_msduHandle;
  txQElement->txQPkt = p;
  m_txQueue.push_back (txQElement);

  CheckQueue ();
}

void
LrWpanMac::CheckQueue ()
{
  NS_LOG_FUNCTION (this);
  //std::cout << m_PANCoordinator << " : " << "CheckQueue" << std::endl;
  // Pull a packet from the queue and start sending, if we are not already sending.
  if (m_lrWpanMacState == MAC_IDLE && !m_txQueue.empty () && m_txPkt == 0 && !m_setMacState.IsRunning ())
    {
      TxQueueElement *txQElement = m_txQueue.front ();
      m_txPkt = txQElement->txQPkt;
      m_setMacState = Simulator::ScheduleNow (&LrWpanMac::SetLrWpanMacState, this, MAC_CSMA);
    }
}

void
LrWpanMac::SetCsmaCa (Ptr<LrWpanCsmaCa> csmaCa)
{
	//std::cout << m_PANCoordinator << " : " << "SetCsmaCa" << std::endl;
  m_csmaCa = csmaCa;
}

void
LrWpanMac::SetPhy (Ptr<LrWpanPhy> phy)
{
	//std::cout << m_PANCoordinator << " : " << "SetPhy" << std::endl;
  m_phy = phy;
}

Ptr<LrWpanPhy>
LrWpanMac::GetPhy (void)
{
  return m_phy;
}

void
LrWpanMac::SetMcpsDataIndicationCallback (McpsDataIndicationCallback c)
{
	//std::cout << m_PANCoordinator << " : " << "SetMcpsDataIndicationCallback" << std::endl;
  m_mcpsDataIndicationCallback = c;
}

void
LrWpanMac::SetMcpsDataConfirmCallback (McpsDataConfirmCallback c)
{
	//std::cout << m_PANCoordinator << " : " << "SetMcpsDataConfirmCallback" << std::endl;
  m_mcpsDataConfirmCallback = c;
}

void
LrWpanMac::PdDataIndication (uint32_t psduLength, Ptr<Packet> p, uint8_t lqi)
{
	std::cout << m_PANCoordinator << " : " << "PdDataIndication" << std::endl;
	std::cout << m_PANCoordinator << " : " << "p->GetSize() : " << p->GetSize() << std::endl;
  NS_ASSERT (m_lrWpanMacState == MAC_IDLE || m_lrWpanMacState == MAC_ACK_PENDING || m_lrWpanMacState == MAC_CSMA);

  NS_LOG_FUNCTION (this << psduLength << p << lqi);

  bool acceptFrame;

  // from sec 7.5.6.2 Reception and rejection, Std802.15.4-2006
  // level 1 filtering, test FCS field and reject if frame fails
  // level 2 filtering if promiscuous mode pass frame to higher layer otherwise perform level 3 filtering
  // level 3 filtering accept frame
  // if Frame type and version is not reserved, and
  // if there is a dstPanId then dstPanId=m_macPanId or broadcastPanI, and
  // if there is a shortDstAddr then shortDstAddr =shortMacAddr or broadcastAddr, and
  // if beacon frame then srcPanId = m_macPanId
  // if only srcAddr field in Data or Command frame,accept frame if srcPanId=m_macPanId

  Ptr<Packet> originalPkt = p->Copy (); // because we will strip headers

  m_promiscSnifferTrace (originalPkt);

  m_macPromiscRxTrace (originalPkt);
  // XXX no rejection tracing (to macRxDropTrace) being performed below

  LrWpanMacTrailer receivedMacTrailer;
  p->RemoveTrailer (receivedMacTrailer);
  if (Node::ChecksumEnabled ())
    {
      receivedMacTrailer.EnableFcs (true);
    }

  // level 1 filtering
  if (!receivedMacTrailer.CheckFcs (p))
    {
      m_macRxDropTrace (originalPkt);
      std::cout << m_PANCoordinator << " : " << "PKT : drop"<< std::endl;
    }
  else
    {
      LrWpanMacHeader receivedMacHdr;
      p->RemoveHeader (receivedMacHdr);
      
      /////////////////////////////////////////////////
			//modified by Ubines 20131101
			//removing command frame header 
			LrWpanMacCommandFrame receivedCommandHdr (1);
			if (receivedMacHdr.GetType () == LrWpanMacHeader::LRWPAN_MAC_COMMAND)
			{
				p->RemoveHeader (receivedCommandHdr);
			}

      McpsDataIndicationParams params;
      params.m_dsn = receivedMacHdr.GetSeqNum ();
      params.m_mpduLinkQuality = lqi;
      params.m_srcPanId = receivedMacHdr.GetSrcPanId ();
      params.m_srcAddrMode = receivedMacHdr.GetSrcAddrMode ();
      // TODO: Add field for EXT_ADDR source address.
      if (params.m_srcAddrMode == SHORT_ADDR)
        {
          params.m_srcAddr = receivedMacHdr.GetShortSrcAddr ();
        }
      params.m_dstPanId = receivedMacHdr.GetDstPanId ();
      params.m_dstAddrMode = receivedMacHdr.GetDstAddrMode ();
      // TODO: Add field for EXT_ADDR destination address.
      if (params.m_dstAddrMode == SHORT_ADDR)
        {
          params.m_dstAddr = receivedMacHdr.GetShortDstAddr ();
        }

      NS_LOG_DEBUG ("Packet from " << params.m_srcAddr << " to " << params.m_dstAddr);

      if (m_macPromiscuousMode)
        {
          //level 2 filtering
          std::cout << m_PANCoordinator << " : " << "F2 promiscuous mode"<< std::endl;
          if (!m_mcpsDataIndicationCallback.IsNull ())
            {
              NS_LOG_DEBUG ("promiscuous mode, forwarding up");
              m_mcpsDataIndicationCallback (params, p);
            }
          else
            {
              NS_LOG_ERROR (this << " Data Indication Callback not initialised");
            }
        }
      else
        {
          //level 3 frame filtering
          acceptFrame = (receivedMacHdr.GetType () != LrWpanMacHeader::LRWPAN_MAC_RESERVED);

          if (acceptFrame)
            {
              acceptFrame = (receivedMacHdr.GetFrameVer () <= 1);
            }

          if (acceptFrame
              && (receivedMacHdr.GetDstAddrMode () > 1))
            {
              acceptFrame = receivedMacHdr.GetDstPanId () == m_macPanId
                || receivedMacHdr.GetDstPanId () == 0xffff;
            }

          if (acceptFrame
              && (receivedMacHdr.GetDstAddrMode () == 2))
            {
              acceptFrame = receivedMacHdr.GetShortDstAddr () == m_shortAddress
                || receivedMacHdr.GetShortDstAddr () == Mac16Address ("ff:ff");        // check for broadcast addrs
            }

          if (acceptFrame
              && (receivedMacHdr.GetDstAddrMode () == 3))
            {
              acceptFrame = (receivedMacHdr.GetExtDstAddr () == m_selfExt);
            }

          if (acceptFrame
              && (receivedMacHdr.GetType () == LrWpanMacHeader::LRWPAN_MAC_BEACON))
            {
            	std::cout << m_PANCoordinator << " : " << "Oh I received beacon!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
              if (m_macPanId == 0xffff)
                {
                  // TODO: Accept only if the frame version field is valid
                  acceptFrame = true;
                }
              else
                {
                  acceptFrame = receivedMacHdr.GetSrcPanId () == m_macPanId;
                }
            }

          if (acceptFrame
              && ((receivedMacHdr.GetType () == LrWpanMacHeader::LRWPAN_MAC_DATA)
                  || (receivedMacHdr.GetType () == LrWpanMacHeader::LRWPAN_MAC_COMMAND))
              && (receivedMacHdr.GetSrcAddrMode () > 1))
            {
              acceptFrame = receivedMacHdr.GetSrcPanId () == m_macPanId; // \todo need to check if PAN coord             
            }
					//end Ubines	
					///////////////////////////////////////////////
          if (acceptFrame)
            {
              m_macRxTrace (originalPkt);
              
              if(receivedMacHdr.IsBeacon ()){
              	//////////////////////////////////////////////////////////////////////
									// modified by Ubines 20131126
									// procedure about received beacon
									IsSendingGTSRequest = false;
									m_csmaCa->SetSlottedCsmaCa();
									receivedBeaconProcess(receivedMacHdr, p);
									NotReceivedBeaconNum = 0;
              }
              // \todo: What should we do if we receive a frame while waiting for an ACK?
              //        Especially if this frame has the ACK request bit set, should we reply with an ACK, possibly missing the pending ACK?

              // If the received frame is a frame with the ACK request bit set, we immediately send back an ACK.
              // If we are currently waiting for a pending ACK, we assume the ACK was lost and trigger a retransmission after sending the ACK.
              if ((receivedMacHdr.IsData () || receivedMacHdr.IsCommand ()) && receivedMacHdr.IsAckReq ()
                  && !(receivedMacHdr.GetDstAddrMode () == SHORT_ADDR && receivedMacHdr.GetShortDstAddr () == "ff:ff"))
                {
                  // If this is a data or mac command frame, which is not a broadcast,
                  // with ack req set, generate and send an ack frame.
                  // If there is a CSMA medium access in progress we cancel the medium access
                  // for sending the ACK frame. A new transmission attempt will be started
                  // after the ACK was send.
                  if (m_lrWpanMacState == MAC_ACK_PENDING)
                    {
                      m_ackWaitTimeout.Cancel ();
                      PrepareRetransmission ();
                    }
                  else if (m_lrWpanMacState == MAC_CSMA)
                    {
                      // \todo: If we receive a packet while doing CSMA/CA, should  we drop the packet because of channel busy,
                      //        or should we restart CSMA/CA for the packet after sending the ACK?
                      // Currently we simply restart CSMA/CA after sending the ACK.
                      m_csmaCa->Cancel ();
                    }
                  // Cancel any pending MAC state change, ACKs have higher priority.
                  ///////////////////////////////////////////////////////////////////////////////
									//added by Ubines, 20131101
									//command frame process
								
									if (receivedCommandHdr.GetCommandFrameIdentifier() == LrWpanMacCommandFrame::LRWPAN_MAC_ASSOCIATION_REQUEST && m_PANCoordinator == true)
									{
										std::cout << m_PANCoordinator << " : " << "Oh i received Association Request!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
										MlmeAssociateResponseParams param = updateDeviceDescriptorWithResponseParam(receivedMacHdr);
					
										if (param.m_AssocShortAddress == "00:00")
										{
											return;
										}
					
										if(receivedMacHdr.IsAckReq() == true)
										{
											m_setMacState.Cancel ();
		                  ChangeMacState (MAC_IDLE);
		                  m_setMacState = Simulator::ScheduleNow (&LrWpanMac::SendAck, this, receivedMacHdr.GetSeqNum ());
										}
										Simulator::ScheduleNow (&LrWpanMac::MlmeAssociateResponse, this, param, receivedMacHdr);
										
										return;
									} 
									if (receivedCommandHdr.GetCommandFrameIdentifier() == LrWpanMacCommandFrame::LRWPAN_MAC_ASSOCIATION_RESPONSE)
									{
										std::cout << m_PANCoordinator << " : " << "Oh i received Association Response!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
										if (receivedCommandHdr.GetAssociationStatus() == ASSOCIATED)
										{
										
											if(receivedMacHdr.IsAckReq() == true)
											{
												m_setMacState.Cancel ();
	                  		ChangeMacState (MAC_IDLE);
	                 		 	m_setMacState = Simulator::ScheduleNow (&LrWpanMac::SendAck, this, receivedMacHdr.GetSeqNum ());
											}
											
											m_associationStatus = ASSOCIATED;
											m_shortAddress = receivedCommandHdr.GetMacShortAddress();
											return;
										}
									}
									if (receivedCommandHdr.GetCommandFrameIdentifier() == LrWpanMacCommandFrame::LRWPAN_MAC_DISASSOCIATION_NOTIFICATION)
									{
										std::cout<<"received LRWPAN_MAC_DISASSOCIATION_NOTIFICATION" << std::endl;
									}
									if (receivedCommandHdr.GetCommandFrameIdentifier() == LrWpanMacCommandFrame::LRWPAN_MAC_DATA_REQUEST)
									{
										std::cout<<"received LRWPAN_MAC_DATA_REQUEST" << std::endl;
									}
									if (receivedCommandHdr.GetCommandFrameIdentifier() == LrWpanMacCommandFrame::LRWPAN_MAC_PANID_CONFLICT_NOTIFICATION)
									{
										std::cout<<"received LRWPAN_MAC_PANID_CONFLICT_NOTIFICATION" << std::endl;
									}
									if (receivedCommandHdr.GetCommandFrameIdentifier() == LrWpanMacCommandFrame::LRWPAN_MAC_ORPHAN_NOTIFICATION)
									{
										std::cout<<"received LRWPAN_MAC_ORPHAN_NOTIFICATION" << std::endl;
									}
									if (receivedCommandHdr.GetCommandFrameIdentifier() == LrWpanMacCommandFrame::LRWPAN_MAC_BEACON_REQEUST)
									{
										std::cout<<"received LRWPAN_MAC_BEACON_REQEUST" << std::endl;
									}
									if (receivedCommandHdr.GetCommandFrameIdentifier() == LrWpanMacCommandFrame::LRWPAN_MAC_COORDINATOR_REALIGNMENT)
									{
										std::cout<<"received LRWPAN_MAC_COORDINATOR_REALIGNMENT" << std::endl;
									}
									if (receivedCommandHdr.GetCommandFrameIdentifier() == LrWpanMacCommandFrame::LRWPAN_MAC_GTS_REQUEST)
									{
										std::cout << m_PANCoordinator << " : " << "GTS REQUEST"<< std::endl;
										//////////////////////////////////////////////
										//modified by Ubines 20131121
										//procedure function call about received GTS request
										if(m_PANCoordinator == true)
										{
											///////////////////////////////////////////////////////
											//modified by Ubines 20131106
											//check whether acknowledgment frame is necessary or not
											if(receivedMacHdr.IsAckReq() == true)
											{
												m_setMacState.Cancel ();
			                  ChangeMacState (MAC_IDLE);
			                  m_setMacState = Simulator::ScheduleNow (&LrWpanMac::SendAck, this, receivedMacHdr.GetSeqNum ());
											}
											receivedGTSRequest(receivedMacHdr, receivedCommandHdr);
										}
										return;
										//end Ubines
										//////////////////////////////////////////////
									}
								
                  m_setMacState.Cancel ();
                  ChangeMacState (MAC_IDLE);
                  m_setMacState = Simulator::ScheduleNow (&LrWpanMac::SendAck, this, receivedMacHdr.GetSeqNum ());
                }

              if (receivedMacHdr.IsData () && !m_mcpsDataIndicationCallback.IsNull ())
                {
                  // If it is a data frame, push it up the stack.
                  NS_LOG_DEBUG ("PdDataIndication():  Packet is for me; forwarding up");
                  m_mcpsDataIndicationCallback (params, p);
                }
              else if (receivedMacHdr.IsAcknowledgment () && m_txPkt && m_lrWpanMacState == MAC_ACK_PENDING)
                {
                	std::cout << m_PANCoordinator << " : " << "MAC ACKNOWLEDGMENT"<< std::endl;
                  LrWpanMacHeader macHdr;
                  m_txPkt->PeekHeader (macHdr);
                  if (receivedMacHdr.GetSeqNum () == macHdr.GetSeqNum ())
                    {
                      m_macTxOkTrace (m_txPkt);
                      // If it is an ACK with the expected sequence number, finish the transmission
                      // and notify the upper layer.
                      m_ackWaitTimeout.Cancel ();
                      if (!m_mcpsDataConfirmCallback.IsNull ())
                        {
                          TxQueueElement *txQElement = m_txQueue.front ();
                          McpsDataConfirmParams confirmParams;
                          confirmParams.m_msduHandle = txQElement->txQMsduHandle;
                          confirmParams.m_status = IEEE_802_15_4_SUCCESS;
                          m_mcpsDataConfirmCallback (confirmParams);
                        }
                      RemoveFirstTxQElement ();
                      m_setMacState.Cancel ();
                      m_setMacState = Simulator::ScheduleNow (&LrWpanMac::SetLrWpanMacState, this, MAC_IDLE);
                    }
                  else
                    {
                      // If it is an ACK with an unexpected sequence number, mark the current transmission as failed and start a retransmit. (cf 7.5.6.4.3)
                      m_ackWaitTimeout.Cancel ();
                      if (!PrepareRetransmission ())
                        {
                          m_setMacState.Cancel ();
                          m_setMacState = Simulator::ScheduleNow (&LrWpanMac::SetLrWpanMacState, this, MAC_IDLE);
                        }
                      else
                        {
                          m_setMacState.Cancel ();
                          m_setMacState = Simulator::ScheduleNow (&LrWpanMac::SetLrWpanMacState, this, MAC_CSMA);
                        }
                    }
						
                }
            }
          else
            {
              m_macRxDropTrace (originalPkt);
            }
        }
    }
}

void
LrWpanMac::SendAck (uint8_t seqno)
{
  NS_LOG_FUNCTION (this << static_cast<uint32_t> (seqno));
	std::cout << m_PANCoordinator << " : " << "send Ack" << std::endl;
  NS_ASSERT (m_lrWpanMacState == MAC_IDLE);

  // Generate a corresponding ACK Frame.
  LrWpanMacHeader macHdr (LrWpanMacHeader::LRWPAN_MAC_ACKNOWLEDGMENT, seqno);
  LrWpanMacTrailer macTrailer;
  Ptr<Packet> ackPacket = Create<Packet> (0);
  ackPacket->AddHeader (macHdr);
  // Calculate FCS if the global attribute ChecksumEnable is set.
  if (Node::ChecksumEnabled ())
    {
      macTrailer.EnableFcs (true);
      macTrailer.SetFcs (ackPacket);
    }
  ackPacket->AddTrailer (macTrailer);

  // Enqueue the ACK packet for further processing
  // when the transmitter is activated.
  m_txPkt = ackPacket;

  // Switch transceiver to TX mode. Proceed sending the Ack on confirm.
  ChangeMacState (MAC_SENDING);
  m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_TX_ON);
}

void
LrWpanMac::RemoveFirstTxQElement ()
{
	std::cout << m_PANCoordinator << " : " << "RemoveFirstTxQElement" << std::endl;
  TxQueueElement *txQElement = m_txQueue.front ();
  Ptr<const Packet> p = txQElement->txQPkt;
  m_numCsmacaRetry += m_csmaCa->GetNB () + 1;

	
  Ptr<Packet> pkt = p->Copy ();
  LrWpanMacHeader hdr;
  pkt->RemoveHeader (hdr);
  if (hdr.GetShortDstAddr () != Mac16Address ("ff:ff"))
    {
      m_sentPktTrace (p, m_retransmission + 1, m_numCsmacaRetry);
    }

  txQElement->txQPkt = 0;
  delete txQElement;
  m_txQueue.pop_front ();
  m_txPkt = 0;
  m_retransmission = 0;
  m_numCsmacaRetry = 0;
  m_macTxDequeueTrace (p);

}

void
LrWpanMac::AckWaitTimeout (void)
{
  NS_LOG_FUNCTION (this);
//std::cout << m_PANCoordinator << " : " << "AckWaitTimeout" << std::endl;
  // TODO: If we are a PAN coordinator and this was an indirect transmission,
  //       we will not initiate a retransmission. Instead we wait for the data
  //       being extracted after a new data request command.
  if (!PrepareRetransmission ())
    {
      SetLrWpanMacState (MAC_IDLE);
    }
  else
    {
      SetLrWpanMacState (MAC_CSMA);
    }
}

bool
LrWpanMac::PrepareRetransmission (void)
{
	std::cout << m_PANCoordinator << " : " << "PrepareRetransmission" << std::endl;
  NS_LOG_FUNCTION (this);

  if (m_retransmission >= m_macMaxFrameRetries)
    {
      // Maximum number of retransmissions has been reached.
      // remove the copy of the packet that was just sent
      TxQueueElement *txQElement = m_txQueue.front ();
      m_macTxDropTrace (txQElement->txQPkt);
      if (!m_mcpsDataConfirmCallback.IsNull ())
        {
          McpsDataConfirmParams confirmParams;
          confirmParams.m_msduHandle = txQElement->txQMsduHandle;
          confirmParams.m_status = IEEE_802_15_4_NO_ACK;
          m_mcpsDataConfirmCallback (confirmParams);
        }
      RemoveFirstTxQElement ();
      return false;
    }
  else
    {
      m_retransmission++;
      m_numCsmacaRetry += m_csmaCa->GetNB () + 1;
      // Start next CCA process for this packet.
      return true;
    }
}

void
LrWpanMac::PdDataConfirm (LrWpanPhyEnumeration status)
{
	std::cout << m_PANCoordinator << " : " << "PdDataConfirm" << std::endl;
  NS_ASSERT (m_lrWpanMacState == MAC_SENDING);

  NS_LOG_FUNCTION (this << status << m_txQueue.size ());

  LrWpanMacHeader macHdr;
  m_txPkt->PeekHeader (macHdr);
  if (status == IEEE_802_15_4_PHY_SUCCESS)
    {
      if (!macHdr.IsAcknowledgment ())
        {
          // We have just send a regular data packet, check if we have to wait
          // for an ACK.
          if (macHdr.IsAckReq ())
            {
              // wait for the ack or the next retransmission timeout
              // start retransmission timer
              Time waitTime = MicroSeconds (GetMacAckWaitDuration () * 1000 * 1000 / m_phy->GetDataOrSymbolRate (false));
              NS_ASSERT (m_ackWaitTimeout.IsExpired ());
              m_ackWaitTimeout = Simulator::Schedule (waitTime, &LrWpanMac::AckWaitTimeout, this);
              m_setMacState.Cancel ();
              m_setMacState = Simulator::ScheduleNow (&LrWpanMac::SetLrWpanMacState, this, MAC_ACK_PENDING);
              return;
            }
          //add Ubines
          else if(macHdr.IsBeacon ()){
          		std::cout << m_PANCoordinator << " : " << "oh it is beacon" << std::endl;
          }
          //end Ubines
          else
            {
              m_macTxOkTrace (m_txPkt);
              // remove the copy of the packet that was just sent
              if (!m_mcpsDataConfirmCallback.IsNull ())
                {
                  McpsDataConfirmParams confirmParams;
                  NS_ASSERT_MSG (m_txQueue.size () > 0, "TxQsize = 0");
                  TxQueueElement *txQElement = m_txQueue.front ();
                  confirmParams.m_msduHandle = txQElement->txQMsduHandle;
                  confirmParams.m_status = IEEE_802_15_4_SUCCESS;
                  m_mcpsDataConfirmCallback (confirmParams);
                }
              RemoveFirstTxQElement ();
            }
        }
      else
        {
          // We have send an ACK. Clear the packet buffer.
          m_txPkt = 0;
        }
    }
  else if (status == IEEE_802_15_4_PHY_UNSPECIFIED)
    {
			std::cout << m_PANCoordinator << " : " << "phy unspecified " << std::endl;
      if (!macHdr.IsAcknowledgment ())
        {
          NS_ASSERT_MSG (m_txQueue.size () > 0, "TxQsize = 0");
          TxQueueElement *txQElement = m_txQueue.front ();
          m_macTxDropTrace (txQElement->txQPkt);
          if (!m_mcpsDataConfirmCallback.IsNull ())
            {
              McpsDataConfirmParams confirmParams;
              confirmParams.m_msduHandle = txQElement->txQMsduHandle;
              confirmParams.m_status = IEEE_802_15_4_FRAME_TOO_LONG;
              m_mcpsDataConfirmCallback (confirmParams);
            }
          RemoveFirstTxQElement ();
        }
      else
        {
          NS_LOG_ERROR ("Unable to send ACK");
        }
    }
  else
    {
      // Something went really wrong. The PHY is not in the correct state for
      // data transmission.
      NS_FATAL_ERROR ("Transmission attempt failed with PHY status " << status);
    }

  m_setMacState.Cancel ();
  m_setMacState = Simulator::ScheduleNow (&LrWpanMac::SetLrWpanMacState, this, MAC_IDLE);
}

void
LrWpanMac::PlmeCcaConfirm (LrWpanPhyEnumeration status)
{
	std::cout << m_PANCoordinator << " : " << "PlmeCcaConfirm" << std::endl;
  NS_LOG_FUNCTION (this << status);
  // Direct this call through the csmaCa object
  m_csmaCa->PlmeCcaConfirm (status);
}

void
LrWpanMac::PlmeEdConfirm (LrWpanPhyEnumeration status, uint8_t energyLevel)
{
	std::cout << m_PANCoordinator << " : " << "PlmeEdConfirm" << std::endl;
  NS_LOG_FUNCTION (this << status << energyLevel);

}

void
LrWpanMac::PlmeGetAttributeConfirm (LrWpanPhyEnumeration status,
                                    LrWpanPibAttributeIdentifier id,
                                    LrWpanPhyPibAttributes* attribute)
{
	std::cout << m_PANCoordinator << " : " << "PlmeGetAttributeConfirm" << std::endl;
  NS_LOG_FUNCTION (this << status << id << attribute);
}

void
LrWpanMac::PlmeSetTRXStateConfirm (LrWpanPhyEnumeration status)
{
	//std::cout << m_PANCoordinator << " : " << "PlmeSetTRXStateConfirm" << std::endl;
  NS_LOG_FUNCTION (this << status);

  if (m_lrWpanMacState == MAC_SENDING && (status == IEEE_802_15_4_PHY_TX_ON || status == IEEE_802_15_4_PHY_SUCCESS))
    {
      NS_ASSERT (m_txPkt);

      // Start sending if we are in state SENDING and the PHY transmitter was enabled.
      m_promiscSnifferTrace (m_txPkt);
      m_snifferTrace (m_txPkt);
      m_macTxTrace (m_txPkt);
      m_phy->PdDataRequest (m_txPkt->GetSize (), m_txPkt);
    }
  else if (m_lrWpanMacState == MAC_CSMA && (status == IEEE_802_15_4_PHY_RX_ON || status == IEEE_802_15_4_PHY_SUCCESS))
    {
      // Start the CSMA algorithm as soon as the receiver is enabled.
      m_csmaCa->Start ();
    }
  else if (m_lrWpanMacState == MAC_IDLE)
    {
      NS_ASSERT (status == IEEE_802_15_4_PHY_RX_ON || status == IEEE_802_15_4_PHY_SUCCESS || status == IEEE_802_15_4_PHY_TRX_OFF);
      // Do nothing special when going idle.
    }
  else if (m_lrWpanMacState == MAC_ACK_PENDING)
    {
      NS_ASSERT (status == IEEE_802_15_4_PHY_RX_ON || status == IEEE_802_15_4_PHY_SUCCESS);
    }
  else
    {
      // TODO: What to do when we receive an error?
      // If we want to transmit a packet, but switching the transceiver on results
      // in an error, we have to recover somehow (and start sending again).
      NS_FATAL_ERROR ("Error changing transceiver state");
    }
}

void
LrWpanMac::PlmeSetAttributeConfirm (LrWpanPhyEnumeration status,
                                    LrWpanPibAttributeIdentifier id)
{
	std::cout << m_PANCoordinator << " : " << "PlmeSetAttributeConfirm" << std::endl;
  NS_LOG_FUNCTION (this << status << id);
}

void
LrWpanMac::SetLrWpanMacState (LrWpanMacState macState)
{
	//std::cout << m_PANCoordinator << " : " << "SetLrWpanMacState" << std::endl;
  NS_LOG_FUNCTION (this << "mac state = " << macState);

  McpsDataConfirmParams confirmParams;

  if (macState == MAC_IDLE)
    {
      ChangeMacState (MAC_IDLE);

      if (m_macRxOnWhenIdle)
        {
          m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_RX_ON);
        }
      else
        {
          m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_TRX_OFF);
        }

      CheckQueue ();
    }
  else if (macState == MAC_ACK_PENDING)
    {
      ChangeMacState (MAC_ACK_PENDING);
      m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_RX_ON);
    }
  else if (macState == MAC_CSMA)
    {
      NS_ASSERT (m_lrWpanMacState == MAC_IDLE || m_lrWpanMacState == MAC_ACK_PENDING);

      ChangeMacState (MAC_CSMA);
      m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_RX_ON);
    }
  else if (m_lrWpanMacState == MAC_CSMA && macState == CHANNEL_IDLE)
    {
      // Channel is idle, set transmitter to TX_ON
      ChangeMacState (MAC_SENDING);
      m_phy->PlmeSetTRXStateRequest (IEEE_802_15_4_PHY_TX_ON);
    }
  else if (m_lrWpanMacState == MAC_CSMA && macState == CHANNEL_ACCESS_FAILURE)
    {
      NS_ASSERT (m_txPkt);

      // cannot find a clear channel, drop the current packet.
      NS_LOG_DEBUG ( this << " cannot find clear channel");
      confirmParams.m_msduHandle = m_txQueue.front ()->txQMsduHandle;
      confirmParams.m_status = IEEE_802_15_4_CHANNEL_ACCESS_FAILURE;
      m_macTxDropTrace (m_txPkt);
      if (!m_mcpsDataConfirmCallback.IsNull ())
        {
          m_mcpsDataConfirmCallback (confirmParams);
        }
      // remove the copy of the packet that was just sent
      RemoveFirstTxQElement ();

      ChangeMacState (MAC_IDLE);
    }
}

LrWpanAssociationStatus
LrWpanMac::GetAssociationStatus (void) const
{
	//std::cout << m_PANCoordinator << " : " << "GetAssociationStatus" << std::endl;
  return m_associationStatus;
}

void
LrWpanMac::SetAssociationStatus (LrWpanAssociationStatus status)
{
	std::cout << m_PANCoordinator << " : " << "SetAssociationStatus" << std::endl;
  m_associationStatus = status;
}

uint16_t
LrWpanMac::GetPanId (void) const
{
	//std::cout << m_PANCoordinator << " : " << "GetPanId" << std::endl;
  return m_macPanId;
}

void
LrWpanMac::SetPanId (uint16_t panId)
{
	std::cout << m_PANCoordinator << " : " << "SetPanId" << std::endl;
  m_macPanId = panId;
}

void
LrWpanMac::ChangeMacState (LrWpanMacState newState)
{
	//std::cout << m_PANCoordinator << " : " << "ChangeMacState" << std::endl;
  NS_LOG_LOGIC (this << " change lrwpan mac state from "
                     << m_lrWpanMacState << " to "
                     << newState);
  m_macStateLogger (m_lrWpanMacState, newState);
  m_lrWpanMacState = newState;
}

uint64_t
LrWpanMac::GetMacAckWaitDuration (void) const
{
	//std::cout << m_PANCoordinator << " : " << "GetMacAckWaitDuration" << std::endl;
  return m_csmaCa->GetUnitBackoffPeriod () + m_phy->aTurnaroundTime + m_phy->GetPhySHRDuration ()
         + ceil (6 * m_phy->GetPhySymbolsPerOctet ());
}

uint8_t
LrWpanMac::GetMacMaxFrameRetries (void) const
{
	//std::cout << m_PANCoordinator << " : " << "GetMacMaxFrameRetries" << std::endl;
  return m_macMaxFrameRetries;
}

void
LrWpanMac::SetMacMaxFrameRetries (uint8_t retries)
{
	//std::cout << m_PANCoordinator << " : " << "SetMacMaxFrameRetries" << std::endl;
  m_macMaxFrameRetries = retries;
}

} // namespace ns3
