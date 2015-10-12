/* Beacon Frame 20131017
 * Author: Ubines
 */

/*
 * the following classes implements the 802.15.4 Beacon Header
 *    -------------------------------------------------------------------------------------------
 *    Beacon          : Frame Control, Sequence Number, Address Fields+, Auxiliary Security Header++.
 *
 *    + - The Address fields in Beacon frame is made up of the Source PAN Id and address only and size
 *        is  4 or 8 octets whereas the other frames may contain the Destination PAN Id and address as
 *        well. (see specs).
 *    ++ - These fields are optional and of variable size
 */
 
/* JW fix header 
Header files which located in same directory wrap with "" 
and Header files which located in different directory wrap with <> and add ns3/ prefix
*/
#include "lr-wpan-beacon-header.h"
#include <ns3/mac16-address.h>
#include <ns3/mac64-address.h>
#include <ns3/address-utils.h>
//end JW


namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LrWpanBeacon);

// TODO: Test Compressed PAN Id, Security Enabled, different size Key


LrWpanBeacon::LrWpanBeacon (void)
{

}

LrWpanBeacon::LrWpanBeacon (uint8_t beaconOrder)
{
	SetSupSpecBeaconOrder(beaconOrder);
	SetSupSpecSuperOrder(15);
	SetSupSpecFinalCAPSlot(6);
	SetSupSpecBLE(0);
	SetSupSpecReserved(0);
	SetSupSpecPANC(1);
	SetSupSpecAssocPermit(0);
	
	SetGTSDesCount(0);
	SetGTSSpecReserved(0);
	SetGTSPermit(0);
	SetGTSdirecMask(0);
	SetGTSdirecRes(0);

	SetNumofShortAddPending(0);
	SetPenAddReserved1(0);
	SetNumofExtenAddPending(0);
	SetPenAddreserved2(0);

}

LrWpanBeacon::~LrWpanBeacon (void)
{
}


uint16_t
LrWpanBeacon::GetSuperframeSpec  (void) const
{
	uint16_t val = 0;
	
	val = m_supspecBeaconOrder & (0x0f);             			// Bit 0-3
  val |= (m_supspecSuperframeOrder << 4) & (0x0f << 4);			// Bit 4-7
  val |= (m_supspecFinalCAPSlot << 8) & (0x0f << 8);   		 	// Bit 8-11
  val |= (m_supspecBLE << 12) & (0x01 << 12);          			// Bit 12
  val |= (m_supspecReserved << 13) & (0x01 << 13);     			// Bit 13
  val |= (m_supspecPANC << 14) & (0x01 << 14);         			// Bit 14
  val |= (m_supspecAssocPermit << 15) & (0x01 << 15);  			// Bit 15
	return val;
}

uint8_t
LrWpanBeacon::GetSupSpecBeaconOrder (void) const
{
  return (m_supspecBeaconOrder);
}

uint8_t
LrWpanBeacon::GetSupSpecSuperOrder  (void) const
{
  return (m_supspecSuperframeOrder);
}

uint8_t
LrWpanBeacon::GetSupSpecFinalCAPSlot   (void) const
{
  return (m_supspecFinalCAPSlot);
}

uint8_t
LrWpanBeacon::GetSupSpecBLE (void) const
{
  return (m_supspecBLE);
}

uint8_t
LrWpanBeacon::GetSupSpecReserved (void) const
{
  return (m_supspecReserved);
}

uint8_t
LrWpanBeacon::GetSupSpecPANC  (void) const
{
  return (m_supspecPANC);
}

uint8_t
LrWpanBeacon::GetSupSpecAssocPermit   (void) const
{
  return (m_supspecAssocPermit);
}


uint8_t
LrWpanBeacon::GetGTSSpec (void) const
{
  uint8_t val = 0;

  val = m_GTSDesCount & (0x7);						// Bit 0-2
  val |= (m_GTSSpecReserved << 3) & (0xf << 3);		// Bit 3-6
  val |= (m_GTSPermit << 7) & (0x1 << 7);			// Bit 7

  return(val);
}


uint8_t
LrWpanBeacon::GetGTSDesCount    (void) const
{
  return (m_GTSDesCount);
}

uint8_t
LrWpanBeacon::GetGTSSpecReserved (void) const
{
  return (m_GTSSpecReserved);
}

uint8_t
LrWpanBeacon::GetGTSPermit (void) const
{
  return (m_GTSPermit);
}


uint8_t
LrWpanBeacon::GetGTSDirec (void) const
{
  uint8_t val = 0;

  val = m_GTSDirecMask & (0x3f);              		// Bit 0-6
  val |= (m_GTSDirecReserved << 7) & (0x1 << 7);	// Bit 7

  return(val);
}


uint8_t
LrWpanBeacon::GetGTSdirecMask (void) const
{
  return (m_GTSDirecMask);
}

uint8_t
LrWpanBeacon::GetGTSdirecRes (void) const
{
  return (m_GTSDirecReserved);
}

uint8_t
LrWpanBeacon::GetGTSList (uint8_t m_GTSStartSlot1, uint8_t m_GTSLength1) const
{
  uint8_t val = 0;

  val = m_GTSStartSlot1 & (0xf);				// Bit 16-19
  val |= (m_GTSLength1 << 4) & (0xf << 4);		// Bit 20-23

  return(val);
}


Mac16Address
LrWpanBeacon::GetDevShortAdd (void) const
{
  return (m_GTSDevShortAddress);
}

uint8_t
LrWpanBeacon::GetGTSStartSlot (void) const
{
  return (m_GTSStartSlot);
}

uint8_t
LrWpanBeacon::GetGTSLength (void) const
{
  return (m_GTSLength);
}

uint8_t
LrWpanBeacon::GetPendingAddSpec (void) const
{
  uint8_t val = 0;

  val = m_NumofShortAddPending & (0x7);					// Bit 0-2
  val |= (m_PenAddSpecReserved_1 << 3) & (0x1 << 3);	// Bit 3
  val |= (m_NumofExtAddPending << 4) & (0x7 << 4);		// Bit 4-6
	val |= (m_PenAddSpecReserved_2 << 7) & (0x1 << 7);  // Bit 7
	
  return(val);
}


uint8_t
LrWpanBeacon::GetNumofShortAddPending (void) const
{
  return (m_NumofShortAddPending);
}

uint8_t
LrWpanBeacon::GetPenAddReserved1 (void) const
{
  return (m_PenAddSpecReserved_1);
}

uint8_t
LrWpanBeacon::GetNumofExtenAddPending (void) const
{
  return (m_NumofExtAddPending);
}

uint8_t
LrWpanBeacon::GetPenAddreserved2 (void) const
{ 
  return (m_PenAddSpecReserved_2);
}

/*
Mac16Address
LrWpanBeacon::GetShortAddressList (void)
{
  return (m_ShortAddressList);
}

Mac64Address
LrWpanBeacon::GetExtenAddressList (void)
{
  return (m_ExtenAddressList);
}
*/

void
LrWpanBeacon::SetSuperframeSpec  (uint16_t superframeSpec)
{
  m_supspecBeaconOrder = (superframeSpec) & (0x0f);             // Bit 0-3
  m_supspecSuperframeOrder = (superframeSpec >> 4) & (0x0f);    // Bit 4-7
  m_supspecFinalCAPSlot = (superframeSpec >> 8) & (0x0f);		// Bit 8-11
  m_supspecBLE = (superframeSpec >> 12) & (0x01);				// Bit 12
  m_supspecReserved = (superframeSpec >> 13) & (0x01);			// Bit 13
  m_supspecPANC = (superframeSpec >> 14) & (0x01);				// Bit 14
  m_supspecAssocPermit = (superframeSpec >> 15) & (0x01);		// Bit 15
}

void
LrWpanBeacon::SetSupSpecBeaconOrder (uint8_t beaconOrder)
{
  m_supspecBeaconOrder = beaconOrder;
}

void
LrWpanBeacon::SetSupSpecSuperOrder (uint8_t superframeOrder)
{
  m_supspecSuperframeOrder = superframeOrder;
}

void
LrWpanBeacon::SetSupSpecFinalCAPSlot (uint8_t finalCAPSlot)
{
  m_supspecFinalCAPSlot = finalCAPSlot;
}

void
LrWpanBeacon::SetSupSpecBLE (uint8_t BLE)
{
  m_supspecBLE = BLE;
}

void
LrWpanBeacon::SetSupSpecReserved (uint8_t reserved)
{
  m_supspecReserved = reserved;
}

void
LrWpanBeacon::SetSupSpecPANC (uint8_t PANC)
{
  m_supspecPANC = PANC;
}

void
LrWpanBeacon::SetSupSpecAssocPermit (uint8_t assocPermit)
{
  m_supspecAssocPermit = assocPermit;
}

void
LrWpanBeacon::SetGTSSpec (uint8_t gtsSpec)
{
  m_GTSDesCount = (gtsSpec) & (0x07);             // Bit 0-2
  m_GTSSpecReserved = (gtsSpec >> 3) & (0x0f);    // Bit 3-6
  m_GTSPermit = (gtsSpec >> 7) & (0x01);		  // Bit 7
}

void
LrWpanBeacon::SetGTSDesCount (uint8_t gtsDesCnt)
{
  m_GTSDesCount = gtsDesCnt;
}

void
LrWpanBeacon::SetGTSSpecReserved  (uint8_t reserved)
{
  m_GTSSpecReserved = reserved;
}

void
LrWpanBeacon::SetGTSPermit (uint8_t gtsPermit)
{
  m_GTSPermit = gtsPermit;
}

void
LrWpanBeacon::SetGTSDirec (uint8_t gtsDirec)
{
  m_GTSDirecMask = (gtsDirec) & (0x3f);             // Bit 0-6
  m_GTSDirecReserved = (gtsDirec >> 7) & (0x01);    // Bit 7
}

void
LrWpanBeacon::SetGTSdirecMask (uint8_t directMast)
{
  m_GTSDirecMask = directMast;
}

void
LrWpanBeacon::SetGTSdirecRes (uint8_t reserved)
{
  m_GTSDirecReserved = reserved;
}

void
LrWpanBeacon::SetGTSList (uint8_t gtsList)
{
	m_GTSStartSlot = (gtsList) & (0x0f);
	m_GTSLength = (gtsList >> 4) & (0x0f);
}

void
LrWpanBeacon::SetDevShortAdd (Mac16Address devShortAdd)
{
  m_GTSDevShortAddress = devShortAdd;
}

void
LrWpanBeacon::SetGTSStartSlot (uint8_t gtsStartSlot)
{
  m_GTSStartSlot = gtsStartSlot;
}

void
LrWpanBeacon::SetGTSLength (uint8_t gtsLength)
{
  m_GTSLength = gtsLength;
}

void
LrWpanBeacon::SetPendingAddSpec (uint8_t penAddSpec)
{
  m_NumofShortAddPending = (penAddSpec) & (0x07);         // Bit 0-2
  m_PenAddSpecReserved_1 = (penAddSpec >> 3) & (0x01);    // Bit 3
  m_NumofExtAddPending = (penAddSpec >> 4) & (0x07);	  // Bit 4-6  
  m_PenAddSpecReserved_2 = (penAddSpec >> 7) & (0x01);	  // Bit 7 
}

void
LrWpanBeacon::SetNumofShortAddPending (uint8_t numofshortaddpend)
{
  m_NumofShortAddPending = numofshortaddpend;
}

void
LrWpanBeacon::SetPenAddReserved1 (uint8_t reserved)
{
  m_PenAddSpecReserved_1 = reserved;
}

void
LrWpanBeacon::SetNumofExtenAddPending (uint8_t numofextenaddpend)
{
  m_NumofExtAddPending = numofextenaddpend;
}

void
LrWpanBeacon::SetPenAddreserved2  (uint8_t reserved)
{
  m_PenAddSpecReserved_2 = reserved;
}

std::string
LrWpanBeacon::GetName (void) const
{
  return "LrWpan BEACON Header";
}


TypeId
LrWpanBeacon::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LrWpanBeacon")
    .SetParent<Header> ()
    .AddConstructor<LrWpanBeacon> ();
  return tid;
}


TypeId
LrWpanBeacon::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}


void
LrWpanBeacon::PrintFrameControl (std::ostream &os) const
{
  os << "LrWpanBeacon : PrintFrameControl" ;
}

void
LrWpanBeacon::Print (std::ostream &os) const
{
  os << "LrWpanBeacon : Print" ;
}

uint32_t
LrWpanBeacon::GetSerializedSize (void) const
{

    uint32_t size = 2; //Superframe Specification
	
	size += 1; //GTS Specification
	size += 1; //GTS Direction

	if(GetGTSDesCount() == 0)
	{
		size += 3;
	}
	else
	{
		size += GetGTSDesCount() * 3; //GTS List
	}

	size += 1; //Pending Address Specification
	
	return (size);
}


void
LrWpanBeacon::Serialize (Buffer::Iterator start) const
{
	Buffer::Iterator i = start;
  		
	uint16_t superframeSpec = GetSuperframeSpec ();
	i.WriteHtolsbU16 (superframeSpec);
	
	i.WriteU8 (GetGTSSpec());
	i.WriteU8 (GetGTSDirec());

	if(GetGTSDesCount() == 0)
	{
			WriteTo (i, GetDevShortAdd()); //Device Short Address in GTS List 	
			i.WriteU8 (GetGTSList(0,0));
	}
	else
	{
		for(uint8_t j = 0 ; j < GetGTSDesCount() ; j++)
		{
			WriteTo (i, m_GTSList[j].m_GTSDevShortAddress); //Device Short Address in GTS List 	
			i.WriteU8 (GetGTSList(m_GTSList[j].m_GTSStartSlot, m_GTSList[j].m_GTSLength));
		}
	}
       
    i.WriteU8 (GetPendingAddSpec());
}


uint32_t
LrWpanBeacon::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;
	uint16_t superframeSpec  = i.ReadLsbtohU16 ();
	SetSuperframeSpec(superframeSpec);
  		
	SetGTSSpec (i.ReadU8 ());
	SetGTSDirec (i.ReadU8 ());

	if(GetGTSDesCount() == 0)
	{
		ReadFrom (i, m_GTSDevShortAddress );
		SetGTSList (i.ReadU8());
	}
	else
	{
	  
		for(uint8_t j = 0 ; j < GetGTSDesCount() ; j++)
		{			
			ReadFrom (i, m_GTSDevShortAddress );
			SetGTSList (i.ReadU8 ());

			GTSList gtslist;
			gtslist.m_GTSDevShortAddress = GetDevShortAdd();
			gtslist.m_GTSStartSlot = GetGTSStartSlot();
			gtslist.m_GTSLength = GetGTSLength();

			m_GTSList.push_back(gtslist);				
		}
	}

	SetPendingAddSpec (i.ReadU8 ());

	return i.GetDistanceFrom (start);
}

void
LrWpanBeacon::CreateGTSList(std::vector<GTSList> list){

	if(list.size() == 0){
		SetGTSDesCount(m_GTSList.size());
		return ;
	}

	for(std::vector<GTSList>::iterator IterPos = list.begin(); IterPos != list.end(); ++IterPos )
	{
		GTSList gtsDescriptorList;
		gtsDescriptorList.m_GTSDevShortAddress = IterPos->m_GTSDevShortAddress;
		gtsDescriptorList.m_GTSStartSlot = IterPos->m_GTSStartSlot;
		gtsDescriptorList.m_GTSLength = IterPos->m_GTSLength;

		m_GTSList.push_back (gtsDescriptorList);
	}

	SetGTSDesCount(m_GTSList.size());
}	

std::vector<GTSList>
LrWpanBeacon::GetGTSList2(void){
	return m_GTSList;
}

} //namespace ns3


