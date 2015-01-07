/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       Swnd.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI����DUI����ģ��
*/

#pragma once
#include "SWindowMgr.h"
#include "SwndContainer-i.h"

#include "helper/STimerEx.h"
#include "helper/SwndMsgCracker.h"

#include "event/EventSubscriber.h"
#include "event/events.h"
#include "event/EventSet.h"
#include "SwndLayout.h"
#include "res.mgr/SStylePool.h"
#include "res.mgr/SSkinPool.h"

#include <OCIdl.h>

#define SC_WANTARROWS     0x0001      /* Control wants arrow keys         */
#define SC_WANTTAB        0x0002      /* Control wants tab keys           */
#define SC_WANTRETURN     0x0004      /* Control wants return keys        */
#define SC_WANTCHARS      0x0008      /* Want WM_CHAR messages            */
#define SC_WANTALLKEYS    0xFFFF      /* Control wants all keys           */
#define SC_WANTSYSKEY     0x80000000    /* System Key */

#define ICWND_FIRST    ((SWindow*)-1)   /*�Ӵ��ڲ����ڿ�ͷ*/
#define ICWND_LAST    NULL              /*�Ӵ��ڲ�����ĩβ*/

namespace SOUI
{

    /////////////////////////////////////////////////////////////////////////
    enum {NormalShow=0,ParentShow=1};    //�ṩWM_SHOWWINDOW��Ϣʶ���Ǹ�������ʾ����Ҫ��ʾ������
    enum {NormalEnable=0,ParentEnable=1};    //�ṩWM_ENABLE��Ϣʶ���Ǹ����ڿ��û���ֱ�Ӳ�����ǰ����

    
    #define UM_UPDATESWND (WM_USER+2000)    //���͵��������ڵ�����ˢ�·Ǳ�����ϴ��ڵ��Զ�����Ϣ, wParam:SWND

    class STimerID
    {
    public:
        DWORD    Swnd:24;        //���ھ��,������ھ������24λ��Χ������ʹ�����ַ�ʽ���ö�ʱ��
        DWORD    uTimerID:7;        //��ʱ��ID��һ���������֧��128����ʱ����
        DWORD    bSwndTimer:1;    //����ͨ�ö�ʱ���ı�־����־Ϊ1ʱ����ʾ�ö�ʱ��ΪSWND��ʱ��

        STimerID(SWND hWnd,char id)
        {
            SASSERT(hWnd<0x00FFFFFF && id>=0);
            bSwndTimer=1;
            Swnd=hWnd;
            uTimerID=id;
        }
        STimerID(DWORD dwID)
        {
            memcpy(this,&dwID,sizeof(DWORD));
        }
        operator DWORD &() const
        {
            return *(DWORD*)this;
        }
    };


    class SOUI_EXP SPainter
    {
    public:
        SPainter(): crOld(CR_INVALID)
        {
        }

        CAutoRefPtr<IFont> pOldPen;
        COLORREF          crOld;
    };

    class SOUI_EXP SMsgHandleState
    {
    public:
        SMsgHandleState():m_bMsgHandled(FALSE)
        {

        }

        BOOL IsMsgHandled() const
        {
            return m_bMsgHandled;
        }

        void SetMsgHandled(BOOL bHandled)
        {
            m_bMsgHandled = bHandled;
        }

        BOOL m_bMsgHandled;
    };

    //////////////////////////////////////////////////////////////////////////
    // SWindow
    //////////////////////////////////////////////////////////////////////////

    typedef enum tagGW_CODE
    {
        GSW_FIRSTCHILD=0,
        GSW_LASTCHILD,
        GSW_PREVSIBLING,
        GSW_NEXTSIBLING,
        GSW_PARENT,
        GSW_OWNER,
    } GW_CODE;

    typedef struct SWNDMSG
    {
        UINT uMsg;
        WPARAM wParam;
        LPARAM lParam;
    } *PSWNDMSG;

    struct SwndToolTipInfo
    {
        SWND    swnd;       //ӵ��tooltip�Ĵ���
        DWORD   dwCookie;   //tooltip�ڴ����ڵ�ID����Ӧһ�����ڲ�ͬ������ʾ��ͬtip�������һ����Բ��ṩ
        CRect   rcTarget;   //tooltip��Ӧ��
        SStringT strTip;    //top�ַ���
    };

    /**
    * @class     SWindow
    * @brief     SOUI���ڻ��� 
    * 
    * Describe   SOUI���ڻ���,ʵ�ִ��ڵĻ����ӿ�
    */
    class SOUI_EXP SWindow : public SObject
        , public SMsgHandleState
        , public TObjRefImpl2<IObjRef,SWindow>
    {
        SOUI_CLASS_NAME(SWindow, L"window")
        friend class SwndLayoutBuilder;
        friend class SWindowRepos;
        friend class SHostWnd;
        friend class SwndContainerImpl;
    public:
        SWindow();

        virtual ~SWindow();

    public://SWindow״̬��ط���
        /**
        * GetSwnd
        * @brief    ��ô��ھ��
        * @return   SWND 
        *
        * Describe  
        */
        SWND GetSwnd();
        SWindow *GetWindow(int uCode);    

        /**
        * GetWindowText
        * @brief    ��ô����ı�
        * @return   SStringT 
        *
        * Describe  
        */
        virtual SStringT GetWindowText();

        /**
        * SetWindowText
        * @brief    ���ô����ı�
        * @param    LPCTSTR lpszText --  �����ı�
        * @return   void 
        *
        * Describe  
        */
        virtual void SetWindowText(LPCTSTR lpszText);

        /**
         * GetEventSet
         * @brief    ��ȡSEventSet����
         * @return   SEventSet * SEventSet����ָ��
         *
         * Describe  
         */
        SEventSet * GetEventSet(){return &m_evtSet;}

        /**
         * GetStyle
         * @brief    ��ȡstyle����
         * @return   SwndStyle& style����
         *
         * Describe  
         */
        SwndStyle& GetStyle();

        BOOL IsChecked();
        void SetCheck(BOOL bCheck);

        BOOL IsDisabled(BOOL bCheckParent = FALSE);
        void EnableWindow( BOOL bEnable,BOOL bUpdate=FALSE);

        BOOL IsVisible(BOOL bCheckParent = FALSE);
        void SetVisible(BOOL bVisible,BOOL bUpdate=FALSE);

        LPCWSTR GetName(){return m_strName;}
        void SetName(LPCWSTR pszName){m_strName=pszName;}

        int GetID(){return m_nID;}
        void SetID(int nID){m_nID=nID;}

        void SetOwner(SWindow *pOwner);
        SWindow *GetOwner();

        DWORD GetState(void);
        DWORD ModifyState(DWORD dwStateAdd, DWORD dwStateRemove,BOOL bUpdate=FALSE);

        ISwndContainer *GetContainer();
        void SetContainer(ISwndContainer *pContainer);

        /**
        * GetUserData
        * @brief    ��userdata
        * @return   ULONG_PTR 
        *
        * Describe  
        */
        ULONG_PTR GetUserData();
        /**
        * SetUserData
        * @brief    ����userdata
        * @param    ULONG_PTR uData --  ԭ����userdata
        * @return   ULONG_PTR 
        *
        * Describe  
        */
        ULONG_PTR SetUserData(ULONG_PTR uData);

        /**
        * GetTextAlign
        * @brief    ����ı��Ķ����־
        * @return   UINT 
        *
        * Describe  ����ı��Ķ����־
        */
        UINT GetTextAlign();    

        /**
        * GetWindowRect
        * @brief    ��ô����������е�λ��
        * @param    [out] LPRECT prect --  ���ھ���
        * @return   void 
        *
        * Describe  
        */    
        void GetWindowRect(LPRECT prect);

        /**
        * GetClientRect
        * @brief    ��ô��ڵĿͻ���
        * @param    [out] LPRECT pRect --  ���ھ���
        * @return   void 
        *
        * Describe  
        */
        virtual void GetClientRect(LPRECT pRect);
        
    public://�������ṹ��ط���
        /**
         * GetParent
         * @brief    ��ø�����
         * @return   SWindow * ������ָ��
         *
         * Describe  
         */
        SWindow *GetParent();

        /**
         * GetTopLevelParent
         * @brief    ��ö��㴰��
         * @return   SWindow * ���㴰��ָ��
         *
         * Describe  
         */
        SWindow * GetTopLevelParent();
        
        SWindow * GetRoot(){return GetTopLevelParent();}
        
        /**
         * GetChildrenCount
         * @brief    ����Ӵ�������
         * @return   UINT �Ӵ������� 
         *
         * Describe  
         */
        UINT GetChildrenCount();

        /**
         * InsertChild
         * @brief    �ڴ������в���һ���Ӵ���
         * @param    SWindow * pNewChild --  �Ӵ��ڶ���
         * @param    SWindow * pInsertAfter --  ����λ��
         * @return   void 
         *
         * Describe  һ������UI��ʼ����ʱ�򴴽�������Ĵ��ڲ����Զ����벼������
         */
        void InsertChild(SWindow *pNewChild,SWindow *pInsertAfter=ICWND_LAST);

        /**
         * RemoveChild
         * @brief    �Ӵ��������Ƴ�һ���Ӵ��ڶ���
         * @param    SWindow * pChild --  �Ӵ��ڶ���
         * @return   BOOL 
         *
         * Describe  �Ӵ��ڲ����Զ��ͷ�
         */
        BOOL RemoveChild(SWindow *pChild);
        
        /**
         * DestroyChild
         * @brief    ����һ���Ӵ���
         * @param    SWindow * pChild --  �Ӵ��ڶ���
         * @return   BOOL 
         *
         * Describe  �ȵ���RemoveChild���ٵ���pChild->Release���ͷ��Ӵ��ڶ���
         */
        BOOL DestroyChild(SWindow *pChild);

        /**
         * BringWindowToTop
         * @brief    ����ǰ���ڵ�Z-order�ᵽͬ���Ӵ�����ǰ
         * @return   void 
         *
         * Describe  
         */
        void BringWindowToTop();

        /**
        * FindChildByID
        * @brief    ͨ��ID���Ҷ�Ӧ���Ӵ���
        * @param    int nID --  ����ID
        * @param    int nDeep --  �������,-1�������޶�
        * @return   SWindow* 
        *
        * Describe  
        */
        SWindow* FindChildByID(int nID, int nDeep =-1);

        /**
        * FindChildByID2
        * @brief    FindChildByID��ģ���֧࣬������ת��
        * @param    int nID --  ����ID
        * @param    int nDeep --  �������,-1�������޶�
        * @return   T* 
        *
        * Describe  
        */
        template<class T>
        T* FindChildByID2(int nID, int nDeep =-1)
        {
            SWindow *pTarget = FindChildByID(nID,nDeep);
            if(!pTarget || !pTarget->IsClass(T::GetClassName()))
            {
                SASSERT_FMTW(FALSE,L"FindChildByID2 Failed, no window of class [%s] with id of [%d] was found within [%d] levels",T::GetClassName(),nID,nDeep);
                return NULL;
            }
            return (T*)pTarget;
        }

        /**
        * FindChildByName
        * @brief    ͨ�����ֲ����Ӵ���
        * @param    LPCWSTR pszName --  ����name����
        * @param    int nDeep --  �������,-1�������޶�
        * @return   SWindow* 
        *
        * Describe  
        */
        SWindow* FindChildByName(LPCWSTR pszName , int nDeep =-1);

        SWindow* FindChildByName(LPCSTR pszName , int nDeep =-1)
        {
            return FindChildByName(S_CA2W(pszName),nDeep);
        }

        /**
        * FindChildByName2
        * @brief    FindChildByName��ģ���֧࣬������ת��
        * @param    LPCWSTR pszName --  ����name����
        * @param    int nDeep --  �������,-1�������޶�
        * @return   T* 
        *
        * Describe  
        */
        template<class T>
        T* FindChildByName2(LPCWSTR pszName, int nDeep =-1)
        {
            SWindow *pTarget = FindChildByName(pszName,nDeep);
            if(!pTarget || !pTarget->IsClass(T::GetClassName()))
            {
                SASSERT_FMTW(FALSE,L"FindChildByName2 Failed, no window of class [%s] with name of [%s] was found within [%d] levels",T::GetClassName(),pszName,nDeep);
                return NULL;
            }
            return (T*)pTarget;
        }

        template<class T>
        T* FindChildByName2(LPCSTR pszName, int nDeep =-1)
        {
            return FindChildByName2<T>(S_CA2W(pszName),nDeep);
        }

        /**
        * CreateChildren
        * @brief    ��XML�����Ӵ���
        * @param    LPCWSTR pszXml --  �Ϸ���utf16����XML�ַ���
        * @return   SWindow * �����ɹ��ĵ����һ������
        *
        * Describe  
        */
        SWindow *CreateChildren(LPCWSTR pszXml);
        
    public://������Ϣ��ط���
        /**
        * SSendMessage
        * @brief    ��SWND������������Ϣ
        * @param    UINT Msg --  ��Ϣ����
        * @param    WPARAM wParam --  ����1
        * @param    LPARAM lParam --  ����2
        * @param [out] BOOL * pbMsgHandled -- ��Ϣ�����־ 
        * @return   LRESULT ��Ϣ����״̬����������Ϣ����
        *
        * Describe  
        */
        LRESULT SSendMessage(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0,BOOL *pbMsgHandled=NULL);

        /**
        * SSendMessage
        * @brief    ��SWND������������Ϣ
        * @param    MSG * pMsg --  ��Ϣ�ṹ��,
        * @param [out] BOOL * pbMsgHandled -- ��Ϣ�����־ 
        * @return   LRESULT ��Ϣ����״̬����������Ϣ����
        *
        * Describe  pMsg�У�ֻ��message,wparam,lparam��ֵ,һ�������Ҫ�����Ӵ��ڴ�����Ϣ
        */
        LRESULT SDispatchMessage(MSG * pMsg,BOOL *pbMsgHandled=NULL);

        /**
        * GetCurMsg
        * @brief    ��õ�ǰ���ڴ������Ϣ
        * @return   PSWNDMSG 
        *
        * Describe  
        */
        PSWNDMSG GetCurMsg()
        {
            return m_pCurMsg;
        }
        
        BOOL IsMsgTransparent();

    public://�ٿ�SWindow�ķ���

        void SetFocus();
        void KillFocus();
        void Invalidate();
        void InvalidateRect(LPCRECT lprect);
        void InvalidateRect(const CRect& rect);
        void LockUpdate();
        void UnlockUpdate();
        BOOL IsUpdateLocked();

        /**
        * Move
        * @brief    �������ƶ���ָ��λ��
        * @param    LPCRECT prect --  
        * @return   void 
        *
        * Describe  �ƶ��󣬴��ڵĲ��ֱ�־�Զ���ΪPos_Float
        */
        void Move(LPCRECT prect);

        /**
        * Move
        * @brief    �������ƶ���ָ��λ��
        * @param    int x --  left
        * @param    int y --  top
        * @param    int cx --  width
        * @param    int cy --  height
        * @return   void 
        *
        * Describe 
        * @see     Move(LPRECT prect)
        */
        void Move(int x,int y, int cx=-1,int cy=-1);



        /**
        * SetTimer
        * @brief    ���ô��ڶ�ʱ��������һ��IDΪ0-127��SWND��ʱ��
        * @param    char id --  ��ʱ��ID
        * @param    UINT uElapse --  ��ʱ(MS)
        * @return   BOOL 
        *
        * Describe  �ο�::SetTimer
        */
        BOOL SetTimer(char id,UINT uElapse);

        /**
        * KillTimer
        * @brief    ɾ��һ��SWND��ʱ��
        * @param    char id --  ��ʱ��ID
        * @return   void 
        *
        * Describe  
        */
        void KillTimer(char id);

        /**
        * SetTimer2
        * @brief    ���ú�����ʱ����ģ��һ�����ݴ��ڶ�ʱ��
        * @param    UINT_PTR id --  ��ʱ��ID
        * @param    UINT uElapse --  ��ʱ(MS)
        * @return   BOOL 
        *
        * Describe  ����SetTimerֻ֧��0-127�Ķ�ʱ��ID��SetTimer2�ṩ��������timerid
        *           �ܹ�ʹ��SetTimerʱ��������SetTimer2����KillʱЧ�ʻ�Ƚϵ�
        */
        BOOL SetTimer2(UINT_PTR id,UINT uElapse);

        /**
        * KillTimer2
        * @brief    ɾ��һ��SetTimer2���õĶ�ʱ��
        * @param    UINT_PTR id --  SetTimer2���õĶ�ʱ��ID
        * @return   void 
        *
        * Describe  ��Ҫö�ٶ�ʱ���б�
        */
        void KillTimer2(UINT_PTR id);

        /**
        * AnimateWindow
        * @brief    ���ڶ���Ч��
        * @param    DWORD dwTime --  ִ��ʱ��
        * @param    DWORD dwFlags --  ִ��ģʽ
        * @return   BOOL 
        *
        * Describe  
        */
        BOOL AnimateWindow(DWORD dwTime,DWORD dwFlags);
        
        SWND GetCapture();
        SWND SetCapture();
        BOOL ReleaseCapture();

    public:// Virtual functions

        /**
        * IsSiblingsAutoGroupped
        * @brief    ͬ�ര���Զ������־
        * @return   BOOL 
        *
        * Describe  ��Ҫ�Ǹ�RadioButton�õ�
        */
        virtual BOOL IsSiblingsAutoGroupped(){return FALSE;}

        /**
        * GetSelectedSiblingInGroup
        * @brief    �����һ��group��ѡ��״̬�Ĵ���
        * @return   SWindow * 
        *
        * Describe  ����group�еĴ���ʱ����NULL
        */
        virtual SWindow * GetSelectedSiblingInGroup(){return NULL;}

        /**
        * GetSelectedChildInGroup
        * @brief    ��ȡ��ѡ��״̬���Ӵ���
        * @return   SWindow * -- ѡ��״̬����
        * Describe  
        */    
        virtual SWindow * GetSelectedChildInGroup();

        /**
        * OnSetCaretValidateRect
        * @brief    ���ù����ʾ��
        * @param    LPCRECT lpRect --  �����ʾ��
        * @return   void 
        *
        * Describe  
        */
        virtual void OnSetCaretValidateRect(LPCRECT lpRect);

        // Set current cursor, when hover
        virtual BOOL OnSetCursor(const CPoint &pt);

        /**
        * OnUpdateToolTip
        * @brief    ����tooltip
        * @param    const CPoint & pt --  ���Ե�
        * @param [out]  SwndToolTipInfo & tipInfo -- tip��Ϣ 
        * @return   BOOL -- TRUE:����tooltip��FALSE:������tooltip
        *
        * Describe  
        */
        virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo);

        virtual void OnStateChanging(DWORD dwOldState,DWORD dwNewState) {}
        virtual void OnStateChanged(DWORD dwOldState,DWORD dwNewState) {}

        virtual BOOL InitFromXml(pugi::xml_node xmlNode);
        virtual BOOL CreateChildren(pugi::xml_node xmlNode);

        virtual SStringW tr(const SStringW &strSrc);

        virtual SWND SwndFromPoint(CPoint ptHitTest, BOOL bOnlyText);

        virtual BOOL FireEvent(EventArgs &evt);

        virtual UINT OnGetDlgCode();

        virtual BOOL IsFocusable();

        virtual BOOL OnNcHitTest(CPoint pt);

        virtual BOOL IsClipClient()
        {
            return m_bClipClient;
        }

        /**
        * UpdateChildrenPosition
        * @brief    �����Ӵ���λ��
        * @return   void 
        *
        * Describe  
        */
        virtual void UpdateChildrenPosition();

        /**
        * OnRelayout
        * @brief    ����λ�÷����仯
        * @param    const CRect & rcOld --  ԭλ��
        * @param    const CRect & rcNew --  ��λ��
        * @return   void 
        *
        * Describe  ����λ�÷����仯,���´��ڼ������Ӵ���λ��
        */
        virtual void OnRelayout(const CRect &rcOld, const CRect & rcNew);
        
        /**
        * GetChildrenLayoutRect
        * @brief    ����Ӵ��ڵĲ��ֿռ�
        * @return   CRect 
        *
        * Describe  
        */
        virtual CRect GetChildrenLayoutRect();

        /**
         * GetLayout
         * @brief    ��õ�ǰ���ڵĲ��ֶ���
         * @return   const SwndPosition * -- ���ֶ���ָ��
         * Describe  
         */    
        virtual const SwndLayout * GetLayout() const;

        /**
        * GetDesiredSize
        * @brief    ��û��ָ�����ڴ�Сʱ��ͨ����Ƥ�����㴰�ڵ�������С
        * @param    LPRECT pRcContainer --  ����λ��
        * @return   CSize 
        *
        * Describe  
        */
        virtual CSize GetDesiredSize(LPCRECT pRcContainer);

        /**
         * NeedRedrawWhenStateChange
         * @brief    ����״̬�ı�ʱ�ؼ��Ƿ��ػ�
         * @return   BOOL -- TRUE���ػ�
         *
         * Describe  
         */
        virtual BOOL NeedRedrawWhenStateChange();

        /**
         * GetTextRect
         * @brief    ���㴰�����ı�����ʾλ��
         * @param  [out]  LPRECT pRect --  �����ı�����ʾλ��
         * @return   void 
         *
         * Describe  
         */
        virtual void GetTextRect(LPRECT pRect);
        
        /**
         * DrawText
         * @brief    �����ı�����
         * @param    IRenderTarget * pRT --  ��ȾRT
         * @param    LPCTSTR pszBuf --  �ı�
         * @param    int cchText --  �ı�����
         * @param    LPRECT pRect --  ������Ⱦ��Χ
         * @param    UINT uFormat --  ��ʽ
         * @return   void 
         *
         * Describe  �ӿؼ����ظ÷�����������������Ϊ
         */
        virtual void DrawText(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat);
        
        /**
         * DrawFocus
         * @brief    ���ƿؼ��Ľ���״̬
         * @param    IRenderTarget * pRT --  ��ȾRT
         * @return   void 
         *
         * Describe  Ĭ�ϻ���һ�����߿�
         */
        virtual void DrawFocus(IRenderTarget *pRT);
        
        /**
        * BeforePaint
        * @brief    ΪRT׼���õ�ǰ���ڵĻ�ͼ����
        * @param    IRenderTarget * pRT --  
        * @param    SPainter & painter --  
        * @return   void 
        *
        * Describe  
        */
        virtual void BeforePaint(IRenderTarget *pRT, SPainter &painter);

        /**
        * AfterPaint
        * @brief    �ָ���BeforePaint���õ�RT״̬
        * @param    IRenderTarget * pRT --  
        * @param    SPainter & painter --  
        * @return   void 
        *
        * Describe  
        */
        virtual void AfterPaint(IRenderTarget *pRT, SPainter &painter);
        
    public://render��ط���
        /**
        * RedrawRegion
        * @brief    �����ڼ��Ӵ������ݻ��Ƶ�RenderTarget
        * @param    IRenderTarget * pRT --  ��ȾĿ��RT
        * @param    IRegion * pRgn --  ��Ⱦ����ΪNULLʱ��Ⱦ��������
        * @return   void 
        *
        * Describe  
        */
        void RedrawRegion(IRenderTarget *pRT, IRegion *pRgn);

        /**
        * GetRenderTarget
        * @brief    ��ȡһ����SWND��������Ӧ���ڴ�DC
        * @param    const LPRECT pRc --  RT��Χ
        * @param    DWORD gdcFlags --  ͬOLEDCFLAGS
        * @param    BOOL bClientDC --  ������client����
        * @return   IRenderTarget * 
        *
        * Describe  ʹ��ReleaseRenderTarget�ͷ�
        */
        IRenderTarget * GetRenderTarget(const LPRECT pRc=NULL,DWORD gdcFlags=0,BOOL bClientDC=TRUE);


        /**
        * ReleaseRenderTarget
        * @brief    
        * @param    IRenderTarget * pRT --  �ͷ���GetRenderTarget��ȡ��RT
        * @return   void 
        *
        * Describe  
        */
        void ReleaseRenderTarget(IRenderTarget *pRT);

        /**
        * PaintBackground
        * @brief    �����ڵı�������
        * @param    IRenderTarget * pRT --  Ŀ��RT
        * @param    LPRECT pRc --  Ŀ��λ��
        * @return   void 
        *
        * Describe  Ŀ��λ�ñ����ڴ���λ����
        */
        void PaintBackground(IRenderTarget *pRT,LPRECT pRc);

        /**
        * PaintForeground
        * @brief    �����ڵ�ǰ������
        * @param    IRenderTarget * pRT --  Ŀ��RT
        * @param    LPRECT pRc --  Ŀ��λ��
        * @return   void 
        *
        * Describe  Ŀ��λ�ñ����ڴ���λ����,��������ǰ���ڵ��Ӵ���
        */
        void PaintForeground(IRenderTarget *pRT,LPRECT pRc);

        /**
        * BeforePaintEx
        * @brief    ΪDC׼���õ�ǰ���ڵĻ�ͼ����,�Ӷ��㴰�ڿ�ʼ����
        * @param    IRenderTarget * pRT --  ��ȾRT
        * @return   void 
        *
        * Describe  һ��Ӧ�ú�CreateRanderTarget���ʹ��
        */
        void BeforePaintEx(IRenderTarget *pRT);

    public:

        /**
        * FireCommand
        * @brief    ����ڵ�EVT_CMD�¼�
        * @return   BOOL-- true:EVT_CMD�¼�������
        *
        * Describe  
        */
        BOOL FireCommand();

        /**
        * FireCtxMenu
        * @brief    �����ݲ˵��¼�
        * @param    CPoint pt --  �����λ��
        * @return   BOOL -- true:�ⲿ�����˿�ݲ˵��¼�
        *
        * Describe  
        */
        BOOL FireCtxMenu(CPoint pt);
        
    protected://cache��ط���

        /**
        * IsCacheDirty
        * @brief    ��ѯCache��Dirty��־
        * @return   bool -- true��ʾCache�Ѿ�Dirty
        * Describe  
        */    
        bool IsCacheDirty() const  {return IsDrawToCache()&&m_bDirty;}

        /**
        * MarkCacheDirty
        * @brief    ���Cache��Dirty��־
        * @param    bool bDirty --  Dirty��־
        * @return   void
        * Describe  
        */    
        void MarkCacheDirty(bool bDirty) {m_bDirty = bDirty;}

        /**
        * IsDrawToCache
        * @brief    �鿴��ǰ�Ƿ��ǰѴ������ݻ��Ƶ�cache��
        * @return   bool -- true���Ƶ�cache�ϡ�
        * Describe  
        */    
        bool IsDrawToCache() const;

        /**
        * GetCachedRenderTarget
        * @brief    ���Cache�������ݵ�RenderTarget
        * @return   IRenderTarget * -- Cache�������ݵ�RenderTarget
        * Describe  
        */    
        IRenderTarget * GetCachedRenderTarget();

        /**
         * IsLayeredWindow
         * @brief    ȷ����Ⱦʱ�Ӵ��ڵ������ǲ�����Ⱦ����ǰ���ڵĻ�����
         * @return   BOOL -- TREU:�Ӵ��ڵ���������Ⱦ��this�Ļ���RT��
         * Describe  
         */    
        BOOL IsLayeredWindow();

    protected://helper functions

        void _Update();
        
    
        /**
         * _GetCurrentRenderContainer
         * @brief    ��õ�ǰ������������Ⱦ����������
         * @return   SWindow * -- ��Ⱦ����������
         * Describe  
         */    
        SWindow * _GetCurrentLayeredWindow();

        /**
        * _GetRenderTarget
        * @brief    ��ȡһ����SWND��������Ӧ���ڴ�DC
        * @param  [in,out]  CRect & rcGetRT --  RT��Χ,����������Ч������
        * @param    DWORD gdcFlags --  ͬOLEDCFLAGS
        * @param    BOOL bClientDC --  ������client����
        * @return   IRenderTarget * 
        *
        * Describe  ʹ��ReleaseRenderTarget�ͷ�
        */
        IRenderTarget * _GetRenderTarget(CRect & rcGetRT,DWORD gdcFlags,UINT uMinFrgndZorder,IRegion *pRgn);


        /**
        * _ReleaseRenderTarget
        * @brief    
        * @param    IRenderTarget * pRT --  �ͷ���GetRenderTarget��ȡ��RT
        * @return   void 
        *
        * Describe  
        */
        void _ReleaseRenderTarget(IRenderTarget *pRT);

        //���������ݻ��Ƶ�RenderTarget��
        void _PaintWindowClient(IRenderTarget *pRT);
        void _PaintWindowNonClient(IRenderTarget *pRT);
        void _PaintRegion(IRenderTarget *pRT, IRegion *pRgn,UINT iZorderBegin,UINT iZorderEnd);
        void _PaintRegion2(IRenderTarget *pRT, IRegion *pRgn,UINT iZorderBegin,UINT iZorderEnd);

        void DrawDefFocusRect(IRenderTarget *pRT,CRect rc);
        void DrawAniStep(CRect rcFore,CRect rcBack,IRenderTarget *pRTFore,IRenderTarget * pRTBack,CPoint ptAnchor);
        void DrawAniStep( CRect rcWnd,IRenderTarget *pRTFore,IRenderTarget * pRTBack,BYTE byAlpha);
        
        void UpdateCacheMode();
        
        void TestMainThread();

    protected:// Message Handler

        /**
        * SwndProc
        * @brief    Ĭ�ϵ���Ϣ������
        * @param    UINT uMsg --  ��Ϣ����
        * @param    WPARAM wParam --  ����1
        * @param    LPARAM lParam --  ����2
        * @param    LRESULT & lResult --  ��Ϣ����ֵ
        * @return   BOOL �Ƿ񱻴���
        *
        * Describe  ����Ϣӳ�����û�д������Ϣ����ú�������
        */
        virtual BOOL SwndProc(UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT & lResult)
        {
            return FALSE;
        }

        int OnCreate(LPVOID);

        void OnSize(UINT nType, CSize size);

        void OnDestroy();

        BOOL OnEraseBkgnd(IRenderTarget *pRT);

        void OnPaint(IRenderTarget *pRT);

        void OnNcPaint(IRenderTarget *pRT);

        void OnShowWindow(BOOL bShow, UINT nStatus);

        void OnEnable(BOOL bEnable,UINT nStatus);

        void OnLButtonDown(UINT nFlags,CPoint pt);

        void OnLButtonUp(UINT nFlags,CPoint pt);

        void OnRButtonDown(UINT nFlags, CPoint point);

        void OnMouseMove(UINT nFlags,CPoint pt) {}

        void OnMouseHover(WPARAM wParam, CPoint ptPos);

        void OnMouseLeave();

        BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

        void OnSetFocus();
        void OnKillFocus();
        
        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
            MSG_WM_NCPAINT_EX(OnNcPaint)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_SIZE(OnSize)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_SHOWWINDOW(OnShowWindow)
            MSG_WM_ENABLE_EX(OnEnable)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_RBUTTONDOWN(OnRButtonDown)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MSG_WM_MOUSEHOVER(OnMouseHover)
            MSG_WM_MOUSELEAVE(OnMouseLeave)
            MSG_WM_MOUSEWHEEL(OnMouseWheel)
            MSG_WM_SETFOCUS_EX(OnSetFocus)
            MSG_WM_KILLFOCUS_EX(OnKillFocus)
        WND_MSG_MAP_END_BASE()  //��Ϣ���������ഫ�ݣ�����ʹ��WND_MSG_MAP_END_BASE������WND_MSG_MAP_END

    protected:
        // ���Դ�����
        HRESULT OnAttrPos(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrOffset(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrPos2type(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrVisible(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrEnable(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrDisplay(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrCache(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrAlpha(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrSkin(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrClass(const SStringW& strValue, BOOL bLoading);
        HRESULT OnAttrTrackMouseEvent(const SStringW& strValue, BOOL bLoading);

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"id",m_nID,FALSE)
            ATTR_STRINGW(L"name",m_strName,FALSE)
            ATTR_CUSTOM(L"skin", OnAttrSkin)        //ֱ�ӻ��Ƥ������
            ATTR_SKIN(L"ncskin", m_pNcSkin, TRUE)   //ֱ�ӻ��Ƥ������
            ATTR_CUSTOM(L"class", OnAttrClass)      //���style
            ATTR_INT(L"data", m_uData, 0 )
            ATTR_CUSTOM(L"enable", OnAttrEnable)
            ATTR_CUSTOM(L"visible", OnAttrVisible)
            ATTR_CUSTOM(L"show", OnAttrVisible)
            ATTR_CUSTOM(L"display", OnAttrDisplay)
            ATTR_CUSTOM(L"pos", OnAttrPos)
            ATTR_CUSTOM(L"offset", OnAttrOffset)
            ATTR_CUSTOM(L"pos2type", OnAttrPos2type)
            ATTR_CUSTOM(L"cache", OnAttrCache)
            ATTR_CUSTOM(L"alpha",OnAttrAlpha)
            ATTR_CUSTOM(L"trackMouseEvent",OnAttrTrackMouseEvent)
            ATTR_I18NSTRT(L"tip", m_strToolTipText, FALSE)  //ʹ�����԰�����
            ATTR_INT(L"msgTransparent", m_bMsgTransparent, FALSE)
            ATTR_INT(L"maxWidth",m_nMaxWidth,FALSE)
            ATTR_INT(L"clipClient",m_bClipClient,FALSE)
            ATTR_INT(L"focusable",m_bFocusable,FALSE)
            ATTR_CHAIN(m_style)                     //֧�ֶ�style�е����Զ���
        SOUI_ATTRS_END()
        
    protected:
        SWND                m_swnd;             /**< ���ھ�� */
        CRect               m_rcWindow;         /**< �����������е�λ�� */
        BOOL                m_bFloat;           /**< ����λ�ù̶������ı�־ */

        ISwndContainer *    m_pContainer;       /**< �������� */
        SEventSet           m_evtSet;           /**< �����¼����� */

        SWindow *           m_pOwner;           /**< ����Owner���¼��ַ�ʱ������¼�����Owner���� */
        SWindow *           m_pParent;          /**< ������ */
        SWindow *           m_pFirstChild;      /**< ��һ�Ӵ��� */
        SWindow *           m_pLastChild;       /**< ��󴰿� */
        SWindow *           m_pNextSibling;     /**< ǰһ�ֵܴ��� */
        SWindow *           m_pPrevSibling;     /**< ��һ�ֵܴ��� */
        UINT                m_nChildrenCount;   /**< �Ӵ������� */

        SWNDMSG *           m_pCurMsg;          /**< ��ǰ���ڴ���Ĵ�����Ϣ */

        SwndStyle           m_style;            /**< ����Style����һ�鴰������ */
        SStringT            m_strText;          /**< �������� */
        SStringT            m_strToolTipText;   /**< ����ToolTip */
        SStringW            m_strName;          /**< �������� */
        int                 m_nID;              /**< ����ID */
        UINT                m_uZorder;          /**< ����Zorder */

        DWORD               m_dwState;          /**< ��������Ⱦ�����е�״̬ */
        DWORD               m_bVisible:1;       /**< ���ڿɼ�״̬ */
        DWORD               m_bDisplay:1;       /**< ��������ʱ�Ƿ�ռλ����ռλʱ�������²��� */
        DWORD               m_bDisable:1;       /**< ���ڽ���״״̬ */
        DWORD               m_bClipClient:1;    /**< ���ڻ���ʱ��clip�ͻ�������ı�־,����clip�������Ӽ�������ֻ�ڻ��ƿ����߳��ͻ���ʱ������*/
        DWORD               m_bMsgTransparent:1;/**< ������Ϣ��־ TRUE-��������Ϣ */
        DWORD               m_bFocusable:1;     /**< ���ڿɻ�ý����־ */
        DWORD               m_bUpdateLocked:1;  /**< ��ʱ�������£������󣬲�����������Invalidate */
        DWORD               m_bCacheDraw:1;     /**< ֧�ִ������ݵ�Cache��־ */
        DWORD               m_bDirty:1;         /**< ���洰�����־ */

        ISkinObj *          m_pBgSkin;          /**< ����skin */
        ISkinObj *          m_pNcSkin;          /**< �ǿͻ���skin */
        ULONG_PTR           m_uData;            /**< ���ڵ�����λ,����ͨ��GetUserData��� */

        SwndLayout        m_layout;             /**< ���ֶ��� */
        int                 m_nMaxWidth;        /**< �Զ������Сʱ�����ڵ������ */

        CAutoRefPtr<IRenderTarget> m_cachedRT;  /**< ���洰�ڻ��Ƶ�RT */
        
        typedef struct GETRTDATA
        {
            CRect rcRT;             /**< GETRT���õ���Ч��Χ */
            DWORD gdcFlags;         /**< GETRT���Ʊ�־λ */
            UINT  uMinFrgndZorder;  /**< GETRTʱǰ����ʼ��zorder */
            CAutoRefPtr<IRegion> rgn;/**< ����һ����rcRT��Ӧ��IRegion���� */
        } * PGETRTDATA;
        
        PGETRTDATA m_pGetRTData;
        
        CAutoRefPtr<IRegion>    m_invalidRegion;/**< �Ǳ�����ϴ��ڵ������� */
#ifdef _DEBUG
        DWORD               m_nMainThreadId;    /**< �������߳�ID */
#endif
    };
}//namespace SOUI