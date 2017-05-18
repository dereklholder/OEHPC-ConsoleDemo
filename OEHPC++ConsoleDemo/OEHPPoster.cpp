// SimplePost.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <afx.h>
#include <afxwin.h>
#include <afxinet.h>
#include <afxpriv.h>

CWinApp theApp;

int _tmain(int argc, _TCHAR* argv[])
{
       if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
            return 1;
       CInternetSession session;
       CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");

       CString strFormData = _T("account_token=D551ABF1CE6C590A308CECFE8351A762C9C92D904FB7CAFEECA44E2380DA5FE89D54BD50B3F7E6433B&transaction_type=CREDIT_CARD&protocol_version=12&charge_type=SALE&credit_card_number=4242424242424242&expire_month=12&expire_year=2013&charge_total=0.01&transaction_condition_code=6&manage_payer_data=true&order_id=1");
       CHttpConnection* pConnection = session.GetHttpConnection(_T("etrans.paygateway.com"), 443, _T(""), _T(""));
       CHttpFile* pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, 
              _T("/TransactionManager"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

       USES_CONVERSION;

       try
       {
              BOOL result = pFile->SendRequest(strHeaders, (LPVOID)(W2A(strFormData)), strFormData.GetLength());
       }
       catch(CInternetException *e)
       {
              e->ReportError();;
              return -1;
       }

       DWORD dwRet;
       pFile->QueryInfoStatusCode(dwRet);

       if(dwRet != HTTP_STATUS_OK)
       {
              return -1;
       }
       int len = pFile->GetLength();
       char buf[2000];
       int numread;
       CString filepath;
       CString strFile = L"response.txt";
       filepath.Format(L".\\%s", strFile);
       CFile myfile( filepath, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
       while ((numread = pFile->Read(buf,sizeof(buf)-1)) > 0)
       {
              buf[numread] = '\0';
              strFile += buf;
              myfile.Write(buf, numread);
       }
       myfile.Close();
       session.Close();
       pFile->Close(); 
       delete pFile;
       return 0;
}
