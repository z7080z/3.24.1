/* Command Frame 20131017
 * Author: Ubines
 */

#ifndef LRWPANCOMMANDFRAME_H
#define LRWPANCOMMANDFRAME_H

#include <ns3/object.h>
#include <ns3/mac16-address.h>

namespace ns3{

	typedef enum
	{
	  ASSOCIATED = 0,
	  PAN_AT_CAPACITY = 1,
	  PAN_ACCESS_DENIED = 2,
	  ASSOCIATED_WITHOUT_ADDRESS = 0xfe,
	  DISASSOCIATED = 0xff
	} LrWpanAssociationStatus;


class LrWpanMacCommandFrame : public Header
{
	public:
		LrWpanMacCommandFrame(void);
		LrWpanMacCommandFrame(int);
		~LrWpanMacCommandFrame();

		typedef enum{
			//NOT_USE = 0,
			LRWPAN_MAC_ASSOCIATION_REQUEST = 1,
			LRWPAN_MAC_ASSOCIATION_RESPONSE = 2,
			LRWPAN_MAC_DISASSOCIATION_NOTIFICATION = 3,
			LRWPAN_MAC_DATA_REQUEST = 4,
			LRWPAN_MAC_PANID_CONFLICT_NOTIFICATION = 5,
			LRWPAN_MAC_ORPHAN_NOTIFICATION = 6,
			LRWPAN_MAC_BEACON_REQEUST = 7,
			LRWPAN_MAC_COORDINATOR_REALIGNMENT = 8,
			LRWPAN_MAC_GTS_REQUEST = 9,
			LRWPAN_MAC_RESERVED = 10,
			/////////////////////////////////////////
			//[TMCTP] modified by Ubines, 20140108
			//addition of MAC command frame for TMCTP
			LRWPAN_MAC_DBS_REQUEST = 21,
			LRWPAN_MAC_DBS_RESPONSE = 22
			/////////////////////////////////////////

		}CommandFrameIdentifier;
				
		typedef enum{
			//RESERVED = 0,
			COORDINATOR_LEAVE_PAN = 1, //The coordinator wishes the device to leave the PAN
			DEVICE_LEAVE_PAN = 2
			//RESERVED = 0x03~0x7f,
			//RESERVED FOR MAC PRIMITIVE ENUMERATION VALUES = 0x80~0xff,
		} DisassocationStatusField;

		void MakeAssociationRequestFrame(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
		void MakeAssociationResponseFrame(Mac16Address, LrWpanAssociationStatus);
		void MakeDisassociationNotificationFrame(DisassocationStatusField);
		void MakeDataRequestFrame(void);
		void MakePanIDConflictNotificationFrame(void);
		void MakeOrphanNotificationFrame(void);
		void MakeBeaconRequestFrame(void);
		void MakeCoordinatorRealignmentFrame(uint16_t, Mac16Address, uint8_t, uint8_t);
		void MakeGTSRequestFrame(uint8_t, uint8_t, uint8_t);
	
		///////////////////////////////////////////////////////////////
		//[TMCTP] added by Ubines, 20140108
		void MakeDBSRequestFrame(Mac16Address, uint8_t, uint8_t, uint8_t);
		void MakeDBSResponseFrame(Mac16Address, uint8_t, uint8_t, uint8_t, uint32_t, uint8_t, uint8_t);
		//end Ubines
		///////////////////////////////////////////////////////////////
		void SetCommandFrameIdentifier(CommandFrameIdentifier);
		CommandFrameIdentifier GetCommandFrameIdentifier(void) const;
		void SetDeviceType(uint8_t);
		uint8_t GetDeviceType(void) const;
		void SetPowerSource(uint8_t);
		uint8_t GetPowerSource(void) const;
		void SetReceiverOnWhenIdle(uint8_t);
		uint8_t GetReceiverOnWhenIdle(void) const;
		void SetSecurityCapability(uint8_t);
		uint8_t GetSecurityCapability(void) const;
		void SetAllocateAddress(uint8_t);
		uint8_t GetAllocateAddress(void) const;
		void SetMacShortAddress(Mac16Address);
		Mac16Address GetMacShortAddress(void) const;
		void SetAssociationStatus(LrWpanAssociationStatus);
		uint8_t GetAssociationStatus(void) const;
		void SetDisassociationReason(DisassocationStatusField);
		uint8_t GetDisassociationReason(void) const;
		void SetPanIdentifier(uint16_t);
		uint16_t GetPanIdentifier(void) const;
		void SetCoordinatorShortAddress(Mac16Address);
		Mac16Address GetCoordinatorShortAddress(void) const;
		void SetChannelNumber(uint8_t);
		uint8_t GetChannelNumber(void) const;
		void SetChannelPage(uint8_t);
		uint8_t GetChannelPage(void) const;
		void SetGTSCharacteristicsField(uint8_t);
		uint8_t GetGTSCharacteristicsField(void) const;
		void SetCapabilityInformationField(uint8_t);
		uint8_t GetCapabilityInformationField(void) const;
		void SetGTSLength(uint8_t);
		uint8_t GetGTSLength(void) const;
		void SetGTSDirection(uint8_t);
		uint8_t GetGTSDirection(void) const;
		void SetCharacteristicType(uint8_t);
		uint8_t GetCharacteristicType(void) const;
		uint32_t GetSerializedSize (void) const;
		void Serialize (Buffer::Iterator start) const;
		uint32_t Deserialize (Buffer::Iterator start);

		std::string GetName (void) const;
		TypeId GetTypeId (void) const;
		TypeId GetInstanceTypeId (void) const;
		void PrintFrameControl (std::ostream &os) const;
		void Print (std::ostream &os) const;

		/////////////////////////////////////////////
		//[TMCTP] added by Ubines, 20140108
		void SetRequesterShortAddr(Mac16Address);
		Mac16Address GetRequesterShortAddr(void) const;
		void SetDBSLength(uint8_t);
		uint8_t GetDBSLength(void) const;
		void SetCharacteristicsType(uint8_t);
		uint8_t GetCharacteristicsType(void) const;
		void SetNumberoftheDescendant(uint8_t);
		uint8_t GetNumberoftheDescendant(void) const;
		
		void SetAllocatedDBSStartingSlot(uint8_t);
		uint8_t GetAllocatedDBSStartingSlot(void) const;
		void SetAllocatedDBSLength(uint8_t);
		uint8_t GetAllocatedDBSLength(void) const;
		void SetAllocatedPHYChannelNumber(uint8_t);
		uint8_t GetAllocatedPHYChannelNumber(void) const;
		void SetStartBandEdge(uint32_t);
		uint8_t GetStartBandEdge_1(void) const;
		uint8_t GetStartBandEdge_2(void) const;
		uint8_t GetStartBandEdge_3(void) const;
		void SetStartingPHYChannelID(uint8_t);
		uint8_t GetStartingPHYChannelID(void) const;
		void SetEndingPHYChannelID(uint8_t);
		uint8_t GetEndingPHYChannelID(void) const;
	
		void SetDBSRequestInformation(uint8_t);
		uint8_t GetDBSRequestInformation(void) const;
		//end Ubines
		////////////////////////////////////////////
		
	private:
		/* MAC payload(size : variable)  */
		/* MAC command frame indentifier (size : 1 octet) */
		CommandFrameIdentifier m_CommandFrameIdentifier; // octet 1 (enum commandframeidentifier)

		
		/* Association resquest command */
		/* Capability information field(size : 1 octet) */
		uint8_t m_CapabilityInfo;
		//uint8_t m_Reserved;			//0bit
		uint8_t m_DeviceType;			// 1bit
		uint8_t m_PowerSource;			// 2bit
		uint8_t m_ReceiverOnWhenIdle;	// 3bit
		//uint8_t m_Reserved;			// 4-5bit
		uint8_t m_SecurityCapability;	// 6bit
		uint8_t m_AllocateAddr;			// 7bit
		

		/* Association response command */
		Mac16Address m_Short_Addr; 		/* Short Address(size : 2 octet) */
		uint8_t m_AssocStatus; // (enum AssociationStatusField) 		/* Association Status(size : 1 octet) */


		/* Disassociation notification command */
		uint8_t m_DisassocReason; // (enum DisassociationStatusField) /* Disassociation reason(size : 1 octet) */

		
		/* Data Request Command */
		/* Pan ID conflict notification command */
		/* Orphan notification command */
		/* Beacon request command  */
		//no-payload


		/* Coordinator realignment command */
		uint16_t m_PanID; //PAN Identifier(size : 2 octet)
		Mac16Address m_CoordinatorShortAddr; // Coordinator short address(size : 2 octet)
		uint8_t m_ChannelNum; // Channel number(size : 1 octet)
		//Mac16Address m_Short_Addr; // Short Address (size : 2 octet)
		uint8_t m_ChannelPage; // Channel page (size : 0/1 octet)
		
		
		/* Channel Page field */
		//uint8_t m_GtsCharacter; // GTS characteristrics (size : 1 octet)
		uint8_t m_GtsLength;	  // 0~3bit
		uint8_t m_GtsDirection;   // 4bit
		uint8_t m_CharacterType;  // 5bit
		//reserved				  //6~7bit


		///////////////////////////////////////////////////
		//[TMCTP] added by Ubines, 20140108
		/* TMCTP Command frmae */
		/* DBS request command */
		Mac16Address m_RequesterShortAddr;
		uint8_t m_DBSLength;
		uint8_t m_CharacteristicsType;
		uint8_t m_NumberoftheDescendant;

		/* DBS response command */
		uint8_t m_AllocatedDBSStartingSlot;
		uint8_t m_AllocatedDBSLength;
		uint8_t m_AllocatedPHYChannelNumber;
		uint32_t m_StartBandEdge;
		uint8_t m_StartBandEdge_1;
		uint8_t m_StartBandEdge_2;
		uint8_t m_StartBandEdge_3;		
		uint8_t m_StartingPHYChannelID;
		uint8_t m_EndingPHYChannelID;
		//end Ubines
		//////////////////////////////////////////////////


	};
}
#endif