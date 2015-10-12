/* Command Frame 20131017
 * Author: Ubines
 */

#include <ns3/lr-wpan-mac.h>
#include "lr-wpan-mac-header.h"
#include "lr-wpan-command-frame.h"
#include <ns3/address-utils.h>

namespace ns3 {
LrWpanMacCommandFrame::LrWpanMacCommandFrame(){
	
}
LrWpanMacCommandFrame::LrWpanMacCommandFrame(int a){
	a = a+1;
}
LrWpanMacCommandFrame::~LrWpanMacCommandFrame(){
}

void
LrWpanMacCommandFrame::MakeAssociationRequestFrame(uint8_t DeviceType,
												uint8_t PowerSource, uint8_t ReceiverOnWhenIdle,
												uint8_t SecurityCapability, uint8_t AllocateAddr){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_ASSOCIATION_REQUEST;
	SetCommandFrameIdentifier(commandFrameID);
	SetDeviceType(DeviceType);
	SetPowerSource(PowerSource);
	SetReceiverOnWhenIdle(ReceiverOnWhenIdle);
	SetSecurityCapability(SecurityCapability);
	SetAllocateAddress(AllocateAddr);
}

void
LrWpanMacCommandFrame::MakeAssociationResponseFrame(Mac16Address ShortAddr,
															LrWpanAssociationStatus AssocStatus){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_ASSOCIATION_RESPONSE;
	SetCommandFrameIdentifier(commandFrameID);
	SetMacShortAddress(ShortAddr);
	SetAssociationStatus(AssocStatus);
}

void
LrWpanMacCommandFrame::MakeDisassociationNotificationFrame(DisassocationStatusField DisassocReason){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_DISASSOCIATION_NOTIFICATION;
	SetCommandFrameIdentifier(commandFrameID);
	SetDisassociationReason(DisassocReason);
}

void
LrWpanMacCommandFrame::MakeDataRequestFrame(){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_DATA_REQUEST;
	SetCommandFrameIdentifier(commandFrameID);
}

void
LrWpanMacCommandFrame::MakePanIDConflictNotificationFrame(){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_PANID_CONFLICT_NOTIFICATION;
	SetCommandFrameIdentifier(commandFrameID);
}

void
LrWpanMacCommandFrame::MakeOrphanNotificationFrame(){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_ORPHAN_NOTIFICATION;
	SetCommandFrameIdentifier(commandFrameID);
}

void
LrWpanMacCommandFrame::MakeBeaconRequestFrame(){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_BEACON_REQEUST;
	SetCommandFrameIdentifier(commandFrameID);

}

void
LrWpanMacCommandFrame::MakeCoordinatorRealignmentFrame(uint16_t PanID,
														Mac16Address CoordinatorShortAddr,
														uint8_t ChannelNum, uint8_t ChannelPage){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_COORDINATOR_REALIGNMENT;
	SetCommandFrameIdentifier(commandFrameID);
	SetPanIdentifier(PanID);
	SetCoordinatorShortAddress(CoordinatorShortAddr);
	SetChannelNumber(ChannelNum);
	SetChannelPage(ChannelPage);
}

void
LrWpanMacCommandFrame::MakeGTSRequestFrame(uint8_t GTSLength, uint8_t GTSDirection,
														uint8_t CharacterType){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_GTS_REQUEST;
	SetCommandFrameIdentifier(commandFrameID);
	SetGTSLength(GTSLength);
	SetGTSDirection(GTSDirection);
	SetCharacteristicType(CharacterType);
}

////////////////////////////////////////////////////////////
//[TMCTP] added by Ubines, 20140108
void
LrWpanMacCommandFrame::MakeDBSRequestFrame(Mac16Address RequesterShortAddr, uint8_t DBSLength,
											uint8_t CharacteristicsType, uint8_t NumberoftheDescendant){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_DBS_REQUEST;

	SetCommandFrameIdentifier(commandFrameID);
	SetRequesterShortAddr(RequesterShortAddr);
	SetDBSLength(DBSLength);
	SetCharacteristicsType(CharacteristicsType);
	SetNumberoftheDescendant(NumberoftheDescendant);
}

void
LrWpanMacCommandFrame::MakeDBSResponseFrame(Mac16Address RequesterShortAddr, uint8_t AllocatedDBSStartingSlot,
											uint8_t AllocatedDBSLength, uint8_t AllocatedPHYChannelNumber,
											uint32_t StartBandEdge, uint8_t StartingPHYChannelID, uint8_t EndingPHYChannelID){
	CommandFrameIdentifier commandFrameID = LRWPAN_MAC_DBS_RESPONSE;
	SetCommandFrameIdentifier(commandFrameID);
	SetRequesterShortAddr(RequesterShortAddr);
	SetAllocatedDBSStartingSlot(AllocatedDBSStartingSlot);
	SetAllocatedDBSLength(AllocatedDBSLength);
	SetAllocatedPHYChannelNumber(AllocatedPHYChannelNumber);
	SetStartBandEdge(StartBandEdge);
	SetStartingPHYChannelID(StartingPHYChannelID);
	SetEndingPHYChannelID(EndingPHYChannelID);
}
//end Ubines
///////////////////////////////////////////////////////////
void
LrWpanMacCommandFrame::SetCommandFrameIdentifier(CommandFrameIdentifier CommandFrameID){
	m_CommandFrameIdentifier = CommandFrameID;
}

LrWpanMacCommandFrame::CommandFrameIdentifier
LrWpanMacCommandFrame::GetCommandFrameIdentifier(void) const{
	return m_CommandFrameIdentifier;
}

void
LrWpanMacCommandFrame::SetDeviceType(uint8_t DeviceType){
	m_DeviceType = DeviceType;
}

uint8_t
LrWpanMacCommandFrame::GetDeviceType(void) const{
	return m_DeviceType;
}

void
LrWpanMacCommandFrame::SetPowerSource(uint8_t PowerSource){
	m_PowerSource = PowerSource;
}

uint8_t
LrWpanMacCommandFrame::GetPowerSource(void) const{
	return m_PowerSource;
}

void
LrWpanMacCommandFrame::SetReceiverOnWhenIdle(uint8_t ReceiverOnWhenIdle){
	m_ReceiverOnWhenIdle = ReceiverOnWhenIdle;
}

uint8_t
LrWpanMacCommandFrame::GetReceiverOnWhenIdle(void) const{
	return m_ReceiverOnWhenIdle;
}

void
LrWpanMacCommandFrame::SetSecurityCapability(uint8_t SecurityCapability){
	m_SecurityCapability = SecurityCapability;
}

uint8_t
LrWpanMacCommandFrame::GetSecurityCapability(void) const{
	return m_SecurityCapability;
}

void
LrWpanMacCommandFrame::SetAllocateAddress(uint8_t AllocateAddr){
	m_AllocateAddr = AllocateAddr;
}

uint8_t
LrWpanMacCommandFrame::GetAllocateAddress(void) const{
	return m_AllocateAddr;
}

void
LrWpanMacCommandFrame::SetMacShortAddress(Mac16Address ShortAddr){
	m_Short_Addr = ShortAddr;
}

Mac16Address
LrWpanMacCommandFrame::GetMacShortAddress(void) const{
	return m_Short_Addr;
}

void
LrWpanMacCommandFrame::SetAssociationStatus(LrWpanAssociationStatus AssocStatus){
	m_AssocStatus = AssocStatus;
}

uint8_t
LrWpanMacCommandFrame::GetAssociationStatus(void) const{
	return m_AssocStatus;
}

void
LrWpanMacCommandFrame::SetDisassociationReason(DisassocationStatusField DisassocReason){
	m_DisassocReason= DisassocReason;
}

uint8_t
LrWpanMacCommandFrame::GetDisassociationReason(void) const{
	return m_DisassocReason;
}

void
LrWpanMacCommandFrame::SetPanIdentifier(uint16_t PanID){
	m_PanID = PanID;
}

uint16_t
LrWpanMacCommandFrame::GetPanIdentifier(void) const{
	return m_PanID;
}

void
LrWpanMacCommandFrame::SetCoordinatorShortAddress(Mac16Address CoordinatorShortAddr){
	m_CoordinatorShortAddr = CoordinatorShortAddr;
}

Mac16Address
LrWpanMacCommandFrame::GetCoordinatorShortAddress(void) const{
	return m_CoordinatorShortAddr;
}

void
LrWpanMacCommandFrame::SetChannelNumber(uint8_t ChannelNum){
	m_ChannelNum = ChannelNum;
}

uint8_t
LrWpanMacCommandFrame::GetChannelNumber(void) const{
	return m_ChannelNum;
}

void
LrWpanMacCommandFrame::SetChannelPage(uint8_t ChannelPage){
	m_ChannelPage = ChannelPage;
}

uint8_t
LrWpanMacCommandFrame::GetChannelPage(void) const{
	return m_ChannelPage;
}

void
LrWpanMacCommandFrame::SetGTSLength(uint8_t GTSLength){
	m_GtsLength = GTSLength;
}

uint8_t
LrWpanMacCommandFrame::GetGTSLength(void) const{
	return m_GtsLength;
}

void
LrWpanMacCommandFrame::SetGTSDirection(uint8_t GTSDirection){
	m_GtsDirection = GTSDirection;
}

uint8_t
LrWpanMacCommandFrame::GetGTSDirection(void) const{
	return m_GtsDirection;
}

void
LrWpanMacCommandFrame::SetCharacteristicType(uint8_t CharacterType){
	m_CharacterType = CharacterType;
}

uint8_t
LrWpanMacCommandFrame::GetCharacteristicType(void) const{
	return m_CharacterType;
}

/////////////////////////////////////////////////////////////
//[TMCTP] added by Ubines, 20140108

void
LrWpanMacCommandFrame::SetRequesterShortAddr(Mac16Address RequesterShortAddr){
	m_RequesterShortAddr = RequesterShortAddr;
}

Mac16Address
LrWpanMacCommandFrame::GetRequesterShortAddr(void) const{
	return m_RequesterShortAddr;
}

void
LrWpanMacCommandFrame::SetDBSLength(uint8_t DBSLength){
	m_DBSLength = DBSLength;
}

uint8_t
LrWpanMacCommandFrame::GetDBSLength(void) const{
	return m_DBSLength;
}

void
LrWpanMacCommandFrame::SetCharacteristicsType(uint8_t CharacteristicsType){
	m_CharacteristicsType = CharacteristicsType;
}

uint8_t
LrWpanMacCommandFrame::GetCharacteristicsType(void) const{
	return m_CharacteristicsType;
}

void
LrWpanMacCommandFrame::SetNumberoftheDescendant(uint8_t NumberoftheDescendant){
	m_NumberoftheDescendant = NumberoftheDescendant;
}

uint8_t
LrWpanMacCommandFrame::GetNumberoftheDescendant(void) const{
	return m_NumberoftheDescendant;
}

void
LrWpanMacCommandFrame::SetAllocatedDBSStartingSlot(uint8_t AllocatedDBSStartingSlot){
	m_AllocatedDBSStartingSlot = AllocatedDBSStartingSlot;
}

uint8_t
LrWpanMacCommandFrame::GetAllocatedDBSStartingSlot(void) const{
	return m_AllocatedDBSStartingSlot;
}

void
LrWpanMacCommandFrame::SetAllocatedDBSLength(uint8_t AllocatedDBSLength){
	m_AllocatedDBSLength = AllocatedDBSLength;
}

uint8_t
LrWpanMacCommandFrame::GetAllocatedDBSLength(void) const{
	return m_AllocatedDBSLength;
}

void
LrWpanMacCommandFrame::SetAllocatedPHYChannelNumber(uint8_t AllocatedPHYChannelNumber){
	m_AllocatedPHYChannelNumber = AllocatedPHYChannelNumber;
}

uint8_t
LrWpanMacCommandFrame::GetAllocatedPHYChannelNumber(void) const{
	return m_AllocatedPHYChannelNumber;
}

void
LrWpanMacCommandFrame::SetStartBandEdge(uint32_t StartBandEdge){
	m_StartBandEdge = StartBandEdge;
	
	m_StartBandEdge_1 = (StartBandEdge >> 16) & 0x0F;
	m_StartBandEdge_2 = (StartBandEdge >> 8) & 0x0F;
	m_StartBandEdge_3 = (StartBandEdge) & 0x0F;
}

void
LrWpanMacCommandFrame::SetStartingPHYChannelID(uint8_t StartingPHYChannelID){
	m_StartingPHYChannelID = StartingPHYChannelID;
}

uint8_t
LrWpanMacCommandFrame::GetStartingPHYChannelID(void) const{
	return m_StartingPHYChannelID;
}

void
LrWpanMacCommandFrame::SetEndingPHYChannelID(uint8_t EndingPHYChannelID){
	m_EndingPHYChannelID = EndingPHYChannelID;
}

uint8_t
LrWpanMacCommandFrame::GetEndingPHYChannelID(void) const{
	return m_EndingPHYChannelID;
}


//end Ubines
/////////////////////////////////////////////////////////////

uint8_t
LrWpanMacCommandFrame::GetGTSCharacteristicsField(void) const{
	uint8_t val = 0;
	val |=  GetGTSLength() & 0xF;							// 0~3 Bit 
	val |= (GetGTSDirection() << 4) & (0x1 << 4);			// Bit 4
	val |= (GetCharacteristicType() << 5) & (0x1 << 5);     // Bit 5
	return val;
}

void
LrWpanMacCommandFrame::SetGTSCharacteristicsField(uint8_t GtsCharacteristicsField){
	uint8_t val = GtsCharacteristicsField;
	SetGTSLength((val) & 0xF);
	SetGTSDirection((val >> 4) & 0x1);
	SetCharacteristicType((val >> 5) & 0x1);
}

uint8_t
LrWpanMacCommandFrame::GetCapabilityInformationField(void) const{
	uint8_t temp = 0;
	temp |= (GetDeviceType() << 1) & (0x1 << 1);			// Bit 1
	temp |= (GetPowerSource() << 2) & (0x1 << 2);			// Bit 2
	temp |= (GetReceiverOnWhenIdle() << 3) & (0x1 << 3);    // Bit 3
	temp |= (GetSecurityCapability() << 6) & (0x1 << 6);    // Bit 6
	temp |= (GetAllocateAddress() << 7) & (0x1 << 7);		// Bit 7
	const uint8_t val = temp;

	return val;
}

void
LrWpanMacCommandFrame::SetCapabilityInformationField(uint8_t CapabilityInfo){
	uint8_t val = CapabilityInfo;
	SetDeviceType((val >> 1) & 0x1);
	SetPowerSource((val >> 2) & 0x1);
	SetReceiverOnWhenIdle((val >> 3) & 0x1);
	SetSecurityCapability((val >> 6) & 0x1);
	SetAllocateAddress((val >> 7) & 0x1);
}

//////////////////////////////////////////////////////////
//[TMCTP] added by Ubines, 20140108
uint8_t
LrWpanMacCommandFrame::GetDBSRequestInformation(void) const{
	uint8_t temp = 0;
	temp |= GetDBSLength() & 0xf;			// Bit 0-3
	temp |= (GetCharacteristicsType() << 7) & (0x1 << 7);    // Bit 7
	const uint8_t val = temp;

	return val;
}

void
LrWpanMacCommandFrame::SetDBSRequestInformation(uint8_t DBSrequestinformation){
	uint8_t val = DBSrequestinformation;
	SetDBSLength(val & 0xf);
	SetCharacteristicsType((val >> 7) & 0x1);
}

//end Ubines
/////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
//[TMCTP] added by Ubines, 20140108
uint8_t
LrWpanMacCommandFrame::GetStartBandEdge_1(void) const{
	return m_StartBandEdge_1;
}

uint8_t
LrWpanMacCommandFrame::GetStartBandEdge_2(void) const{
	return m_StartBandEdge_2;
}

uint8_t
LrWpanMacCommandFrame::GetStartBandEdge_3(void) const{
	return m_StartBandEdge_3;
}
//end Ubines
/////////////////////////////////////////////////////////


std::string
LrWpanMacCommandFrame::GetName (void) const
{
  return "LrWpan Command Frame Header";
}


TypeId
LrWpanMacCommandFrame::GetTypeId (void) const
{
   static TypeId tid = TypeId ("ns3::LrWpanCommandFrame")
    .SetParent<Header> ()
    .AddConstructor<LrWpanMacCommandFrame> ();
  return tid;
}


TypeId
LrWpanMacCommandFrame::GetInstanceTypeId (void) const
{
  return GetTypeId();
}


void
LrWpanMacCommandFrame::PrintFrameControl (std::ostream &os) const
{
  os << "LrWpanCommandFrame : PrintFrameControl" ;
}

void
LrWpanMacCommandFrame::Print (std::ostream &os) const
{
  os << "LrWpanCommandFrame : Print" ;
}

uint32_t
LrWpanMacCommandFrame::GetSerializedSize(void) const{
	switch(GetCommandFrameIdentifier()){
		case LRWPAN_MAC_ASSOCIATION_REQUEST:
			return 2; //1:command frame identifier , 1:capability information
		case LRWPAN_MAC_ASSOCIATION_RESPONSE:
			return 1+2+1; //1:command frame identifier , 2:short address, 1:association status
		case LRWPAN_MAC_DISASSOCIATION_NOTIFICATION:
			return 1+1; //1:command frame identifier , 1:disassociation reason
		case LRWPAN_MAC_DATA_REQUEST:
			return 1; //1:command frame identifier
		case LRWPAN_MAC_PANID_CONFLICT_NOTIFICATION:
			return 1; //1:command frame identifier
		case LRWPAN_MAC_ORPHAN_NOTIFICATION:
			return 1; //1:command frame identifier
		case LRWPAN_MAC_BEACON_REQEUST:
			return 1; //1:command frame identifier
		case LRWPAN_MAC_COORDINATOR_REALIGNMENT:
			return 1+2+2+1+2+1; //1:command frame identifier , 2:PAN identifier , 2: coordinator short addr , 1:channel number, 2:short address, 1:channel page
		case LRWPAN_MAC_GTS_REQUEST:
			return 1+1; // 1:comand frame identifier, 1:GTS characteristics
		case LRWPAN_MAC_DBS_REQUEST:
			return 1+4; //1:comand frame identifier, 4:DBS Request Information
		case LRWPAN_MAC_DBS_RESPONSE:
			return 1+10; //1:comand frame identifier, 10:DBS Response Information
		case LRWPAN_MAC_RESERVED:
			break;
	}
	return 0;
}

void
LrWpanMacCommandFrame::Serialize (Buffer::Iterator start) const{
	Buffer::Iterator i = start;
	
  	switch (GetCommandFrameIdentifier())
    	{
		case LRWPAN_MAC_ASSOCIATION_REQUEST:
			i.WriteU8(1);
			i.WriteU8(GetCapabilityInformationField());
  		  	break;
		
  		case LRWPAN_MAC_ASSOCIATION_RESPONSE:
			i.WriteU8(2);
			WriteTo(i, GetMacShortAddress());
			i.WriteU8(GetAssociationStatus());
  		  	break;
		
   		case LRWPAN_MAC_DISASSOCIATION_NOTIFICATION:
			i.WriteU8(3);
			i.WriteU8(GetDisassociationReason());
  		  	break;
		
		case LRWPAN_MAC_DATA_REQUEST:
			i.WriteU8(4);
   		 	break;
		
		case LRWPAN_MAC_PANID_CONFLICT_NOTIFICATION:
			i.WriteU8(5);
  		  	break;

		case LRWPAN_MAC_ORPHAN_NOTIFICATION:
			i.WriteU8(6);
   		 	break;

		case LRWPAN_MAC_BEACON_REQEUST:
			i.WriteU8(7);
			break;
		
		case LRWPAN_MAC_COORDINATOR_REALIGNMENT:
			i.WriteU8(8);
			i.WriteU16(GetPanIdentifier());
			WriteTo(i, GetCoordinatorShortAddress());
			i.WriteU8(GetChannelNumber());
			WriteTo(i, GetMacShortAddress());
			i.WriteU8(GetChannelPage());
			break;
		
		case LRWPAN_MAC_GTS_REQUEST:
			i.WriteU8(9);
			i.WriteU8(GetGTSCharacteristicsField());
			break;

		case LRWPAN_MAC_RESERVED:
			i.WriteU8(10);
			break;

		case LRWPAN_MAC_DBS_REQUEST:
			i.WriteU8(21);
			WriteTo(i, GetRequesterShortAddr());
			i.WriteU8(GetDBSRequestInformation());
			i.WriteU8(GetNumberoftheDescendant());
			break;

		case LRWPAN_MAC_DBS_RESPONSE:
			i.WriteU8(22);
			WriteTo(i, GetRequesterShortAddr());
			i.WriteU8(GetAllocatedDBSStartingSlot());
			i.WriteU8(GetAllocatedDBSLength());
			i.WriteU8(GetAllocatedPHYChannelNumber());
			i.WriteU8(GetStartBandEdge_1());
			i.WriteU8(GetStartBandEdge_2());
			i.WriteU8(GetStartBandEdge_3());
			i.WriteU8(GetStartingPHYChannelID());
			i.WriteU8(GetEndingPHYChannelID());
			break;
	}
}

uint32_t
LrWpanMacCommandFrame::Deserialize (Buffer::Iterator start){
	Buffer::Iterator i = start;
	m_CommandFrameIdentifier = (CommandFrameIdentifier)i.ReadU8();
	switch (m_CommandFrameIdentifier)
		{
	   	case LRWPAN_MAC_ASSOCIATION_REQUEST:
			uint8_t CapabilityInfo;
			CapabilityInfo = i.ReadU8();
			SetCapabilityInformationField(CapabilityInfo);
    		break;
		
   		case LRWPAN_MAC_ASSOCIATION_RESPONSE:
			ReadFrom(i, m_Short_Addr);
			m_AssocStatus = i.ReadU8();
	    	break;
		
   	 	case LRWPAN_MAC_DISASSOCIATION_NOTIFICATION:
			m_DisassocReason = i.ReadU8();
    		break;
		
		case LRWPAN_MAC_DATA_REQUEST:
			//no-op
   		 	break;
		
		case LRWPAN_MAC_PANID_CONFLICT_NOTIFICATION:
			//no-op
    		break;

		case LRWPAN_MAC_ORPHAN_NOTIFICATION:
			//no-op
    		break;

		case LRWPAN_MAC_BEACON_REQEUST:
			//no-op
			break;
		
		case LRWPAN_MAC_COORDINATOR_REALIGNMENT:
			m_PanID = i.ReadU16();
			ReadFrom(i, m_CoordinatorShortAddr);
			m_ChannelNum = i.ReadU8();
			ReadFrom(i, m_Short_Addr);
			m_ChannelPage = i.ReadU8();
			break;
		
		case LRWPAN_MAC_GTS_REQUEST:
			uint8_t GTSCharacteristicField;
			GTSCharacteristicField = i.ReadU8();
			SetGTSCharacteristicsField(GTSCharacteristicField);
			break;

		case LRWPAN_MAC_RESERVED:
			break;

		case LRWPAN_MAC_DBS_REQUEST:
			uint8_t DBSrequestinformation;

			ReadFrom(i, m_RequesterShortAddr);
			DBSrequestinformation = i.ReadU8();
			SetDBSRequestInformation(DBSrequestinformation);
			m_NumberoftheDescendant = i.ReadU8();
			break;

		case LRWPAN_MAC_DBS_RESPONSE:
			ReadFrom(i, m_RequesterShortAddr);
			m_AllocatedDBSStartingSlot = i.ReadU8();
			m_AllocatedDBSLength = i.ReadU8();
			m_AllocatedPHYChannelNumber = i.ReadU8();
			m_StartBandEdge_1 = i.ReadU8();
			m_StartBandEdge_2 = i.ReadU8();
			m_StartBandEdge_3 = i.ReadU8();
			m_StartingPHYChannelID = i.ReadU8();
			m_EndingPHYChannelID = i.ReadU8();
			break;
		}
	return i.GetDistanceFrom (start);
}
}
