// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include <helper/MenuWndHook.h>
#include <helper/mybuffer.h>
#include "httpsvr/HTTPServer.h"
#include "MemFlash.h"
#include "../controls.extend/propgrid/SPropertyGrid.h"
#include "../controls.extend/SFlyWnd.h"
#include "uianimation/UiAnimationWnd.h"

#if defined(_DEBUG) && !defined(_WIN64)
// #include <vld.h>//ʹ��Vitural Leaker Detector������ڴ�й©�����Դ�http://vld.codeplex.com/ ����
#endif

#include "MainDlg.h"

#define RES_TYPE 0   //���ļ��м�����Դ
// #define RES_TYPE 1   //��PE��Դ�м���UI��Դ
// #define RES_TYPE 2   //��zip���м�����Դ

#include "../components/resprovider-zip/zipresprovider-param.h"

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif


int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
    //����Ҫ����OleInitialize����ʼ�����л���
    HRESULT hRes = OleInitialize(NULL);
    SASSERT(SUCCEEDED(hRes));

    int nRet = 0; 

    SComMgr *pComMgr = new SComMgr;
    
    //�����������·���޸ĵ�demo���ڵ�Ŀ¼
    TCHAR szCurrentDir[MAX_PATH]={0};
    GetModuleFileName( NULL, szCurrentDir, sizeof(szCurrentDir) );
    LPTSTR lpInsertPos = _tcsrchr( szCurrentDir, _T('\\') );
    _tcscpy(lpInsertPos,_T("\\..\\demo"));
    SetCurrentDirectory(szCurrentDir);

    {
        //����һ����SOUIϵͳ��ʹ�õ���COM���
        //CAutoRefPtr��һ��SOUIϵͳ��ʹ�õ�����ָ����
        CAutoRefPtr<IImgDecoderFactory> pImgDecoderFactory; //ͼƬ����������imagedecoder-wid.dllģ���ṩ
        CAutoRefPtr<IRenderFactory> pRenderFactory;         //UI��Ⱦģ�飬��render-gdi.dll����render-skia.dll�ṩ
        CAutoRefPtr<ITranslatorMgr> trans;                  //�����Է���ģ�飬��translator.dll�ṩ
        CAutoRefPtr<IScriptModule> pScriptLua;              //lua�ű�ģ�飬��scriptmodule-lua.dll�ṩ

        BOOL bLoaded=FALSE;
        int nType=MessageBox(GetActiveWindow(),_T("ѡ����Ⱦ���ͣ�\n[yes]: Skia\n[no]:GDI\n[cancel]:Quit"),_T("select a render"),MB_ICONQUESTION|MB_YESNOCANCEL);
        if(nType == IDCANCEL) return -1;
        //�Ӹ��������ʾ���������������
        
        if(nType == IDYES)
            bLoaded = pComMgr->CreateRender_Skia((IObjRef**)&pRenderFactory);
        else
            bLoaded = pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),nType==IDYES?_T("render_skia"):_T("render_gdi"));
        bLoaded=pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));
        bLoaded=pComMgr->CreateTranslator((IObjRef**)&trans);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("translator"));

        //Ϊ��Ⱦģ����������Ҫ���õ�ͼƬ����ģ��
        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
        //����һ��Ψһ��SApplication����SApplication��������Ӧ�ó������Դ
        SApplication *theApp=new SApplication(pRenderFactory,hInstance);

        //����һ�˸���Դ�ṩ����,SOUIϵͳ��ʵ����3����Դ���ط�ʽ���ֱ��Ǵ��ļ����أ���EXE����Դ���ؼ���ZIPѹ��������
        CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)//���ļ�����
        CreateResProvider(RES_FILE,(IObjRef**)&pResProvider);
        if(!pResProvider->Init((LPARAM)_T("uires"),0))
        {
            SASSERT(0);
            return 1;
        }
#elif (RES_TYPE==1)//��EXE��Դ����
        CreateResProvider(RES_PE,(IObjRef**)&pResProvider);
        pResProvider->Init((WPARAM)hInstance,0);
#elif (RES_TYPE==2)//��ZIP������
        bLoaded=pComMgr->CreateResProvider_ZIP((IObjRef**)&pResProvider);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("resprovider_zip"));

        ZIPRES_PARAM param;
        param.ZipFile(pRenderFactory, _T("uires.zip"),"souizip");
        bLoaded = pResProvider->Init((WPARAM)&param,0);
        SASSERT(bLoaded);
#endif
        //��������IResProvider����SApplication����
        theApp->AddResProvider(pResProvider);

        //����һ��http����������������Դ�м���flash
        CMemFlash   memFlash;

        CHTTPServer flashSvr(&memFlash);
        flashSvr.Start(CMemFlash::HomeDir(),"",82,0);

        if(trans)
        {//�������Է����
            theApp->SetTranslator(trans);
            pugi::xml_document xmlLang;
            if(theApp->LoadXmlDocment(xmlLang,_T("lang_cn"),_T("translator")))
            {
                CAutoRefPtr<ITranslator> langCN;
                trans->CreateTranslator(&langCN);
                langCN->Load(&xmlLang.child(L"language"),1);//1=LD_XML
                trans->InstallTranslator(langCN);
            }
        }
#ifdef DLL_CORE
        //����LUA�ű�ģ�飬ע�⣬�ű�ģ��ֻ����SOUI�ں�����DLL��ʽ����ʱ����ʹ�á�
        bLoaded=pComMgr->CreateScrpit_Lua((IObjRef**)&pScriptLua);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("scirpt_lua"));
        
        if(pScriptLua)
        {
            theApp->SetScriptModule(pScriptLua);
            size_t sz=pResProvider->GetRawBufferSize(_T("script"),_T("lua_test"));
            if(sz)
            {
                CMyBuffer<char> lua;
                lua.Allocate(sz);
                pResProvider->GetRawBuffer(_T("script"),_T("lua_test"),lua,sz);
                pScriptLua->executeScriptBuffer(lua,sz);
            }
        }
#endif//DLL_CORE

        //��SApplicationϵͳ��ע�����ⲿ��չ�Ŀؼ���SkinObj��
        SWkeLoader wkeLoader;
        if(wkeLoader.Init(_T("wke.dll")))        
        {
            theApp->RegisterWndFactory(TplSWindowFactory<SWkeWebkit>());//ע��WKE�����
        }
        theApp->RegisterWndFactory(TplSWindowFactory<SGifPlayer>());//ע��GIFPlayer
        theApp->RegisterSkinFactory(TplSkinFactory<SSkinGif>());//ע��SkinGif
        theApp->RegisterSkinFactory(TplSkinFactory<SSkinAPNG>());//ע��SkinGif

        theApp->RegisterWndFactory(TplSWindowFactory<SIPAddressCtrl>());//ע��IP�ؼ�
        theApp->RegisterWndFactory(TplSWindowFactory<SPropertyGrid>());//ע�����Ա�ؼ�
        theApp->RegisterWndFactory(TplSWindowFactory<SUiAnimationWnd>());//ע�ᶯ���ؼ�
        theApp->RegisterWndFactory(TplSWindowFactory<SFlyWnd>());//ע����ж����ؼ�
        SSkinGif::Gdiplus_Startup();

        //����ϵͳ��Դ
        HMODULE hSysResource=LoadLibrary(SYS_NAMED_RESOURCE);
        if(hSysResource)
        {
            CAutoRefPtr<IResProvider> sysSesProvider;
            CreateResProvider(RES_PE,(IObjRef**)&sysSesProvider);
            sysSesProvider->Init((WPARAM)hSysResource,0);
            theApp->LoadSystemNamedResource(sysSesProvider);
        }
        //����hook���Ʋ˵��ı߿�
        CMenuWndHook::InstallHook(hInstance,L"_skin.sys.menu.border");
        
        //����ȫ����Դ����XML
        theApp->Init(_T("xml_init")); 

        {
            //��������ʾʹ��SOUI����Ӧ�ó��򴰿�,Ϊ�˱��洰�ڶ�������������������󣬰���������һ�㡣
            CMainDlg dlgMain;  
            dlgMain.Create(GetActiveWindow(),0,0,800,600);
            dlgMain.GetNative()->SendMessage(WM_INITDIALOG);
            dlgMain.CenterWindow();
            dlgMain.ShowWindow(SW_SHOWNORMAL);
            nRet=theApp->Run(dlgMain.m_hWnd);
        }

        //Ӧ�ó����˳�
        delete theApp; 
        
        flashSvr.Shutdown();

        //ж�ز˵��߿����hook
        CMenuWndHook::UnInstallHook();
        
        SSkinGif::Gdiplus_Shutdown();
    }
    delete pComMgr;
    OleUninitialize();

    return nRet;
}


