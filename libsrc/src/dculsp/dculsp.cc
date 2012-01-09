#include "dculsp.h"

DICOMUpperLayerServiceRequestor::DICOMUpperLayerServiceRequestor(void);
{
	state=1;
}

virtual
DICOMUpperLayerServiceRequestor::~DICOMUpperLayerServiceRequestor()
{
}

void
DICOMUpperLayerServiceRequestor::T_Connect_Confirm(void)
{
	Assert(state==4);

	// used saved stuff from A_Associate_Request to build PDU

	// this is only for TCP ... what would OSI do about PDU's ??

	build the PDU

	T_Data_Request(PDU);
}

void
DICOMUpperLayerServiceRequestor::A_Associate_Request(
	const char *acn,
	const char *callingae,
	const char *calledae,
	UserInformationItem &uitem,
	PresentationAddress &callingpa,
	PresentationAddress &calledpa,
	PresentationContextDefinitionList &pcdl)
{
	Assert(state==1);

	// save stuff for T_Connect_Confirm callback to establish association

	applicationcontextname=acn;
	callingapplicationentitytitle=callingae;
	calledapplicationentitytitle=calledae;
	userinformationitem=&uitem;
	callingpresentationaddress=&callingpa;
	calledpresentationaddress=&calledpa;
	presentationcontextdefinitionlist=&pcdl;

	Assert(applicationcontextname && *applicationcontextname);
	Assert(callingapplicationentitytitle && *callingapplicationentitytitle);
	Assert(calledapplicationentitytitle && *calledapplicationentitytitle);

	T_Connect_Request(callingpresentationaddress,calledpresentationaddress);
}

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

class DICOMUpperLayerServiceAcceptor {
public:
	DICOMUpperLayerServiceAcceptor(void);

virtual
DICOMUpperLayerServiceAcceptor::~DICOMUpperLayerServiceRequestor()
{
	if (connection) delete connection;
}

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

