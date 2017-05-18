// OEHPC++ConsoleDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "json.hpp"
#include <string>
#include "OEHPC++ConsoleDemo.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <afx.h>
#include <afxwin.h>
#include <afxinet.h>
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using json = nlohmann::json;
// The one and only application object

CWinApp theApp;

using namespace rapidjson;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
        else
        {
			//Ugly Console Prompt Logic for POC while I get everything built around...		
			std::string orderID;
			std::string transactionType;
			std::string chargeType;
			std::string entryMode;
			std::cout << "Enter an OrderID\n";
			std::cin >> orderID;
			std::cout << "Credit Card (Y/N)?\n";
			std::cin >> transactionType;
			std::cout << "EMV or KEYED?";
			std::cin >> entryMode;

			if (transactionType == "Y")
			{
				transactionType = "CREDIT_CARD";
				chargeType = "SALE";
			}
			else
			{
				transactionType = "DEBIT_CARD";
				chargeType = "PURCHASE";
			}
			if (entryMode == "EMV")
			{
				entryMode = "EMV";
			}
			else
			{
				entryMode == "AUTO";
			}	

			CInternetSession session;
			CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");

			std::string preformedString = "account_token=C9CBE35FCE67540F328FE4FC8758AF6DCECC24954FB2C4FFE4A24F2B81D95FEA9953BC5CF45601D078&transaction_type=" + transactionType + "&charge_type=" + chargeType + "&charge_total=1&duplicate_check=NO_CHECK&manage_payer_data=true&order_id=" + orderID + "&entry_mode=" + entryMode;

			CString strFormData(preformedString.c_str());
			CHttpConnection* pConnection = session.GetHttpConnection(_T("ws.test.paygateway.com"), 443, _T(""), _T(""));
			CHttpFile* pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST,
				_T("/HostPayService/v1/hostpay/transactions"), NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);

			USES_CONVERSION;

			try
			{
				BOOL result = pFile->SendRequest(strHeaders, (LPVOID)(W2A(strFormData)), strFormData.GetLength());
			}
			catch (CInternetException *e)
			{
				e->ReportError();;
				return -1;
			}

			DWORD dwRet;
			pFile->QueryInfoStatusCode(dwRet);

			if (dwRet != HTTP_STATUS_OK)
			{
				return -1;
			}
			int len = pFile->GetLength();
			char buf[20000];
			int numread;
			CString filepath;
			CString strFile = L"response.txt";
			CString strResponse = _T("");
			filepath.Format(L".\\%s", strFile);
			CFile myfile(filepath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
			while ((numread = pFile->Read(buf, sizeof(buf) - 1)) > 0)
			{
				buf[numread] = '\0';
				strFile += buf;
				strResponse = buf;
				myfile.Write(buf, numread);
			}
			const wchar_t* JSON = strResponse;

			CT2CA pszConvertedAnsiString(strResponse);
			std::string rawResponseString(pszConvertedAnsiString); //RawResponseString should be the exact response, to be fed into a JSON parser.
			//Begin Json Parsing, probably not cleanest, but get sthe work done, Uses RapidJson external Library to do the gruntwork
			Document d;
			d.Parse(rawResponseString.c_str());

			Value& au = d["actionUrl"];
			Value& ssp = d["sealedSetupParameters"];
			std::string actionUrl = au.GetString();
			std::string sealedSetupParameters = ssp.GetString();
			std::string urlToRender = actionUrl + sealedSetupParameters;

			//Launching in External Web Browser, May implement full Integrated Web Browser support... When I can
			std::string launchIE("start iexplore.exe " + urlToRender);
			std::system(launchIE.c_str());


			myfile.Close();
			session.Close();
			pFile->Close();
			delete pFile;
			return 0;
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }
    return nRetCode;
}
