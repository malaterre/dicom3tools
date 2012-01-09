class PresentationAddress
{
public:

	virtual bool isTCP(void)				{ return false; }
	virtual class TcpPresentationAddress *castToTCP(void)	{ Assert(0); return 0; }
};

class TcpPresentationAddress : public PresentationAddress
{
public:

	bool isTCP(void)				{ return true; }
	class TcpPresentationAddress *castToTCP(void)	{return this; }
};

class UserInformationItem
{
};

class PresentationContextDefinitionList
{
};

class PresentationDataValueList
{
};

class TransportLayerServiceRequestor
{
public:
	TransportLayerServiceRequestor(void);
	virtual ~TransportLayerServiceRequestor(void);

	virtual void T_Connect_Request		(PresentationAddress& callingpresentationaddress,
						 PresentationAddress& calledpresentationaddress) 	{ Assert(0); }

	virtual void T_Connect_Confirm		(void) 							= 0;

	virtual void T_Data_Request(PDU)	{ Assert(0); }
	virtual void T_Data_Indication(PDU)	{ Assert(0); }
};

class TransportLayerServiceAcceptor
{
	TransportLayerServiceAcceptor(void);
	virtual ~TransportLayerServiceAcceptor(void);

	virtual void T_Connect_Indication	(PresentationAddress& callingpresentationaddress,
				 		 PresentationAddress& calledpresentationaddress)	= 0;

	virtual void T_Connect_Response		(void) 						 	{ Assert(0); }

	virtual void T_Data_Request(PDU)	{ Assert(0); }
	virtual void T_Data_Indication(PDU)	{ Assert(0); }
};

class TcpTransportLayerServiceRequestor : public virtual TransportLayerServiceRequestor
{
public:
	TcpTransportLayerServiceRequestor(void);
	virtual ~TcpTransportLayerServiceRequestor(void);

	        void T_Connect_Request		(PresentationAddress& callingpresentationaddress,
						 PresentationAddress& calledpresentationaddress);

	virtual void T_Data_Request(PDU)	{ Assert(0); }
	virtual void T_Data_Indication(PDU)	{ Assert(0); }
};

class TcpTransportLayerServiceAcceptor : public virtual TransportLayerServiceAcceptor
{
	TcpTransportLayerServiceAcceptor(void);
	virtual ~TcpTransportLayerServiceAcceptor(void);

	        void T_Connect_Response		(void);

	virtual void T_Data_Request(PDU)	{ Assert(0); }
	virtual void T_Data_Indication(PDU)	{ Assert(0); }
};

//
//		       DICOM UL
//		       Service
//		       Provider
//
//	  request    |         |  indication
//	-----------> | Message | ---------->
//	             |         |
//	<----------- | Message | <----------
//	confirmation |         |  response
//	            SAP       SAP
//	
//	 Requestor		  Acceptor

// the pure virtual methods are callbacks to be supplied
// by the using class derived from the base class


class DICOMUpperLayerServiceRequestor : public virtual TransportLayerServiceRequestor {
private:
	int state;

	const char *applicationcontextname;
	const char *callingapplicationentitytitle;
	const char *calledapplicationentitytitle;
	UserInformationItem *userinformationitem;
	PresentationAddress *callingpresentationaddress;
	PresentationAddress *calledpresentationaddress;
	PresentationContextDefinitionList *presentationcontextdefinitionlist;

	       void T_Confirm	(void);
public:
	DICOMUpperLayerServiceRequestor(void);
	virtual ~DICOMUpperLayerServiceRequestor();

	        void A_Associate_Request	(const char *applicationcontextname,		// requestor to provider
						 const char *callingapplicationentitytitle,
						 const char *calledapplicationentitytitle,
						 UserInformationItem &userinformationitem,
						 PresentationAddress &callingpresentationaddress,
						 PresentationAddress &calledpresentationaddress,
						 PresentationContextDefinitionList &presentationcontextdefinitionlist
						);

	virtual void A_Associate_Confirm	(const char *applicationcontextname,		// provider  to requestor
						 const char *callingapplicationentitytitle,
						 const char *calledapplicationentitytitle,
						 UserInformationItem &userinformationitem,
						 Uint16 result,
						 Uint16 resultsource,
						 Uint16 diagnostic,
						 PresentationAddress &callingpresentationaddress,
						 PresentationAddress &calledpresentationaddress,
						 PresentationContextDefinitionList &presentationcontextdefinitionlistresult
						) = 0;

	        void A_Release_Request		(Uint16 reason);				// requestor to provider

	virtual void A_Release_Confirm	(Uint16 reason,Uint16 result) = 0;		// provider  to requestor

	        void A_Abort_Request		(void);						// requestor to provider
 
	virtual void A_P_Abort_Indication	(Uint16 reason) = 0;				// provider  to both

	        void P_Data_Request		(PresentationDataValueList &data);		// requestor to provider
 };

class DICOMUpperLayerServiceAcceptor : public virtual TransportLayerServiceAcceptor {
public:
	DICOMUpperLayerServiceAcceptor(void);
	virtual ~DICOMUpperLayerServiceAcceptor();

	virtual void A_Associate_Indication	(const char *applicationcontextname,		// provider  to acceptor
						 const char *callingapplicationentitytitle,
						 const char *calledapplicationentitytitle,
						 UserInformationItem &userinformationitem,
						 PresentationAddress &callingpresentationaddress,
						 PresentationAddress &calledpresentationaddress,
						 PresentationContextDefinitionList &presentationcontextdefinitionlist
						) = 0;

	        void A_Associate_Response	(const char *applicationcontextname,		// acceptor  to provider
						 const char *callingapplicationentitytitle,
						 const char *calledapplicationentitytitle,
						 UserInformationItem &userinformationitem,
						 Uint16 result,
						 Uint16 diagnostic,
						 PresentationAddress &callingpresentationaddress,
						 PresentationAddress &calledpresentationaddress,
						 PresentationContextDefinitionList &presentationcontextdefinitionlistresult
						);

	virtual void A_Release_Indication	(Uint16 reason) = 0;				// provider  to acceptor 
	        void A_Release_Response		(Uint16 reason,Uint16 result);			// acceptor  to provider

	virtual void A_Abort_Indication		(Uint16 source) = 0;				// provider  to acceptor
 
	virtual void A_P_Abort_Indication	(Uint16 reason) = 0;				// provider  to both

	virtual void P_Data_Indication		(PresentationDataValueList &data) = 0;		// provider  to acceptor
 };

