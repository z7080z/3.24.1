/* Beacon Frame 
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


#ifndef LRWPAN_BEACON
#define LRWPAN_BEACON

/* JW fix header
Header files which located in same directory wrap with ""
and Header files which located in different directory wrap with <> and add ns3/ prefix
*/
#include <stdint.h>
#include <ns3/header.h>
#include <ns3/nstime.h>
#include <ns3/mac16-address.h>
#include <ns3/mac64-address.h>
//end JW

namespace ns3 {

  struct GTSList{
	  Mac16Address m_GTSDevShortAddress;
	  uint8_t m_GTSStartSlot;
	  uint8_t m_GTSLength;
  };

class LrWpanBeacon : public Header
{

public:
 
  enum AddrModeType
  {
    NOADDR = 0,
    RESADDR = 1,
    SHORTADDR = 2,
    EXTADDR = 3
  };

	LrWpanBeacon (void);
	LrWpanBeacon (uint8_t);

  ~LrWpanBeacon (void);

	/* Get Superframe Specification */
	uint16_t GetSuperframeSpec (void) const;
	uint8_t GetSupSpecBeaconOrder (void) const;
	uint8_t GetSupSpecSuperOrder (void) const;
	uint8_t GetSupSpecFinalCAPSlot (void) const;
	uint8_t GetSupSpecBLE (void) const;
	uint8_t GetSupSpecReserved (void) const;
	uint8_t GetSupSpecPANC (void) const;
	uint8_t GetSupSpecAssocPermit (void) const; 
	
	/* Get GTS Specification */
	uint8_t GetGTSSpec (void) const;
	uint8_t GetGTSDesCount (void) const;
	uint8_t GetGTSSpecReserved (void) const;
	uint8_t GetGTSPermit (void) const;
	
	/* Get GTS Directions */
	uint8_t GetGTSDirec (void) const;
	uint8_t GetGTSdirecMask (void) const;
	uint8_t GetGTSdirecRes (void) const;
	
	/* Get GTS List */	
	uint8_t GetGTSList (uint8_t, uint8_t) const;
	Mac16Address GetDevShortAdd (void) const;
	uint8_t GetGTSStartSlot (void) const;
	uint8_t GetGTSLength (void) const;
			
	/* Get Pending Address Specification */
	uint8_t GetPendingAddSpec (void) const;
	uint8_t GetNumofShortAddPending (void) const; 
	uint8_t GetPenAddReserved1 (void) const;
	uint8_t GetNumofExtenAddPending (void) const;
	uint8_t GetPenAddreserved2 (void) const;
	
	/* Get Address List */
	Mac16Address GetShortAddressList (void) const;
	Mac64Address GetExtenAddressList (void) const;

	/* Set Superframe Specification */
	void SetSuperframeSpec (uint16_t superframeSpec);
	void SetSupSpecBeaconOrder (uint8_t beaconOrder);
	void SetSupSpecSuperOrder (uint8_t superframeOrder);
	void SetSupSpecFinalCAPSlot (uint8_t finalCAPSlot);
	void SetSupSpecBLE (uint8_t BLE);
	void SetSupSpecReserved (uint8_t reserved);
	void SetSupSpecPANC (uint8_t PANC);
	void SetSupSpecAssocPermit (uint8_t assocPermit);
		
	/* Set GTS Specification */
	void SetGTSSpec (uint8_t gtsSpec);
	void SetGTSDesCount (uint8_t gtsDesCnt);
	void SetGTSSpecReserved (uint8_t reserved);
	void SetGTSPermit (uint8_t gtsPermit);
	
	/* Set GTS Directions */
	void SetGTSDirec (uint8_t gtsDirec);
	void SetGTSdirecMask (uint8_t directMast);
	void SetGTSdirecRes (uint8_t reserved);
	
	/* Set GTS List */	
	void SetGTSList (uint8_t gtsList);
	void SetDevShortAdd (Mac16Address devShortAdd);
	void SetGTSStartSlot (uint8_t gtsStartSlot);
	void SetGTSLength (uint8_t gtsLength);
			
	/* Set Pending Address Specification */
	void SetPendingAddSpec (uint8_t penAddSpec);
	void SetNumofShortAddPending (uint8_t numofshortaddpend); 
	void SetPenAddReserved1 (uint8_t reserved);
	void SetNumofExtenAddPending (uint8_t numofextenaddpend);
	void SetPenAddreserved2 (uint8_t reserved);
	
	/* Set Address List */
	void SetShortAddressList (Mac16Address shortaddlist);
	void SetExtenAddressList (Mac64Address extenaddlist);
	
	/* Set GTS list*/ 
	void CreateGTSList(std::vector<GTSList> );
		
	std::string GetName (void) const;
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	void PrintFrameControl (std::ostream &os) const;
	void Print (std::ostream &os) const;
	uint32_t GetSerializedSize (void) const;
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	std::vector<GTSList> GetGTSList2(void);

private:
 
	/* Superframe Specification 2 Octets */
	uint8_t m_supspecBeaconOrder;					// Bit 0-3
	uint8_t m_supspecSuperframeOrder;				// Bit 4-7
	uint8_t m_supspecFinalCAPSlot;					// Bit 8-11
	uint8_t m_supspecBLE;							// Bit 12
	uint8_t m_supspecReserved;						// Bit 13
	uint8_t m_supspecPANC;							// Bit 14
	uint8_t m_supspecAssocPermit;					// Bit 15
	
	/* GTS fields variable Octets */
	/* GTS Specification 1 Octets */
	uint8_t m_GTSDesCount;							// Bit 0-2
	uint8_t m_GTSSpecReserved;						// Bit 3-6
	uint8_t m_GTSPermit;							// Bit 7
	
	/* GTS Directions 0 or 1 Octets */
	uint8_t m_GTSDirecMask;							// Bit 0-6								
	uint8_t m_GTSDirecReserved;						// Bit 7
	
	/* GTS List variable Octets */
	std::vector <GTSList> m_GTSList;
	
	Mac16Address m_GTSDevShortAddress;				// Bit 0-15
	uint8_t m_GTSStartSlot;							// Bit 16-19
	uint8_t m_GTSLength;							// Bit 20-23
	
	/* Pending address fields variable Octets */
	/* Pending address specification 1 Octets */
	uint8_t m_NumofShortAddPending;					// Bit 0-2
	uint8_t m_PenAddSpecReserved_1;					// Bit 3
	uint8_t m_NumofExtAddPending;					// Bit 4-6
	uint8_t m_PenAddSpecReserved_2;					// Bit 7
	
	/* Address List */								//the maximum num of addresses is seven
	Mac16Address m_ShortAddressList[7]; 
    Mac64Address m_ExtenAddressList[7];  
	
}; //LrWpanBeacon
}; // namespace ns-3
#endif /* LRWPAN_MAC_BEACON_H */