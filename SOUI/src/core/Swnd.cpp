#include "souistd.h"
#include "core/SWnd.h"
#include "core/SwndLayoutBuilder.h"
#include "helper/color.h"
#include "helper/SplitString.h"

namespace SOUI
{
    /*
    ��Ⱦ�ع���Ҫע������⣺
    1����Ⱦ��֮����Ⱦ���ݵĴ���
    2����Ⱦ��֮���������Եļ̳�
    3������ǰ��ʱ���Ӵ��ں��ֵܴ�����Ҫ���Ƶ���ͬ����Ⱦ���ϣ�
        A���Ӵ���ֱ�ӻ��Ƶ���ǰ��Ļ���RT�ϣ�
        B���ֵܴ����γɵ�ǰ��Ҫ�ڵ�ǰ�㴫�ݵ���һ�����Ƶ���һ���ϡ�
    4��ע��BeforePaintEx���߼��仯
    */

    //////////////////////////////////////////////////////////////////////////
    // SWindow Implement
    //////////////////////////////////////////////////////////////////////////

    SWindow::SWindow()
        : m_swnd(SWindowMgr::NewWindow(this))
        , m_nID(0)
        , m_pContainer(NULL)
        , m_pParent(NULL),m_pFirstChild(NULL),m_pLastChild(NULL),m_pNextSibling(NULL),m_pPrevSibling(NULL)
        , m_nChildrenCount(0)
        , m_uZorder(0)
        , m_dwState(WndState_Normal)
        , m_bMsgTransparent(FALSE)
        , m_bVisible(TRUE)
        , m_bDisplay(TRUE)
        , m_bDisable(FALSE)
        , m_nMaxWidth(-1)
        , m_bUpdateLocked(FALSE)
        , m_bClipClient(FALSE)
        , m_bFocusable(FALSE)
        , m_bCacheDraw(FALSE)
        , m_bDirty(TRUE)
        , m_uData(0)
        , m_pOwner(NULL)
        , m_pCurMsg(NULL)
        , m_pBgSkin(NULL)
        , m_pNcSkin(NULL)
        , m_pGetRTData(NULL)
        , m_bFloat(FALSE)
#ifdef _DEBUG
        , m_nMainThreadId( ::GetCurrentThreadId() ) // ��ʼ��������̲߳�һ�������߳�
#endif
    {
        m_evtSet.addEvent(EVT_MOUSE_HOVER);
        m_evtSet.addEvent(EVT_MOUSE_LEAVE);
        m_evtSet.addEvent(EventCmd::EventID);
        m_evtSet.addEvent(EventCtxMenu::EventID);
        m_evtSet.addEvent(EventSetFocus::EventID);
        m_evtSet.addEvent(EventKillFocus::EventID);
    }

    SWindow::~SWindow()
    {
        SWindowMgr::DestroyWindow(m_swnd);
    }


    // Get align
    UINT SWindow::GetTextAlign()
    {
        return m_style.GetTextAlign() ;
    }


    void SWindow::GetWindowRect(LPRECT prect)
    {
        SASSERT(prect);
        if(m_rcWindow.left == POS_INIT || m_rcWindow.left == POS_WAIT)
        {
            memset(prect,0,sizeof(RECT));
        }else
        {
            memcpy(prect,&m_rcWindow,sizeof(RECT));
        }
    }

    void SWindow::GetClientRect(LPRECT pRect)
    {
        SASSERT(pRect);
        if(m_rcWindow.left == POS_INIT || m_rcWindow.left == POS_WAIT)
        {
            memset(pRect,0,sizeof(RECT));
        }else
        {
            *pRect=m_rcWindow;
            pRect->left+=m_style.m_nMarginX;
            pRect->right-=m_style.m_nMarginX;
            pRect->top+=m_style.m_nMarginY;
            pRect->bottom-=m_style.m_nMarginY;
        }
    }

    SStringT SWindow::GetWindowText()
    {
        return m_strText;
    }

    void SWindow::OnSetCaretValidateRect( LPCRECT lpRect )
    {
        CRect rcClient;
        GetClientRect(&rcClient);
        CRect rcIntersect;
        rcIntersect.IntersectRect(&rcClient,lpRect);
        if(GetParent()) GetParent()->OnSetCaretValidateRect(&rcIntersect);
    }

    BOOL SWindow::OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo)
    {
        if(m_strToolTipText.IsEmpty()) return FALSE;
        tipInfo.swnd = m_swnd;
        tipInfo.dwCookie =0;
        tipInfo.rcTarget = m_rcWindow;
        tipInfo.strTip = m_strToolTipText;
        return TRUE;
    }

    void SWindow::SetWindowText(LPCTSTR lpszText)
    {
        m_strText = lpszText;
        if(IsVisible(TRUE)) Invalidate();
        if (m_layout.IsFitContent(PD_ALL))
        {
            if(GetParent()) GetParent()->UpdateChildrenPosition();
            if(IsVisible(TRUE)) Invalidate();
        }
    }

    void SWindow::TestMainThread()
    {
#ifdef DEBUG
        // ���㿴�����������ʱ���Ҳ��ҵĸ����㣬��������߳���ˢ����
        // ����һ����Σ�յ�����
        DWORD dwCurThreadID = GetCurrentThreadId();

        BOOL bOK = (m_nMainThreadId == dwCurThreadID); // ��ǰ�̺߳͹������ʱ���߳�һ��

        SASSERT(bOK);
#endif
    }


    // Send a message to SWindow
    LRESULT SWindow::SSendMessage(UINT Msg, WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/,BOOL *pbMsgHandled/*=NULL*/)
    {
        LRESULT lResult = 0;

        if ( Msg < WM_USER
            && Msg != WM_DESTROY
            && Msg != WM_CLOSE
            )
        {
            TestMainThread();
        }
        AddRef();
        SWNDMSG msgCur= {Msg,wParam,lParam};
        SWNDMSG *pOldMsg=m_pCurMsg;
        m_pCurMsg=&msgCur;

        BOOL bOldMsgHandle=IsMsgHandled();//������һ����Ϣ�Ĵ���״̬

        SetMsgHandled(FALSE);

        ProcessSwndMessage(Msg, wParam, lParam, lResult);

        if(pbMsgHandled) *pbMsgHandled=IsMsgHandled();

        SetMsgHandled(bOldMsgHandle);//�ָ���һ����Ϣ�Ĵ���״̬

        m_pCurMsg=pOldMsg;
        Release();

        return lResult;
    }

    LRESULT SWindow::SDispatchMessage( MSG * pMsg,BOOL *pbMsgHandled/*=NULL*/ )
    {
        LRESULT lRet = SSendMessage(pMsg->message,pMsg->wParam,pMsg->lParam,pbMsgHandled);
        if(pbMsgHandled && *pbMsgHandled) return lRet;
        SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
        while(pChild)
        {
            pChild->SDispatchMessage(pMsg,pbMsgHandled);
            pChild = pChild->GetWindow(GSW_NEXTSIBLING);
        }
        return lRet;
    }

    // Move SWindow to new place
    //
    void SWindow::Move(LPCRECT prect)
    {
        SASSERT(prect);
        TestMainThread();

        m_bFloat = TRUE;//ʹ��Move�󣬳������Զ����㴰������
        if(m_rcWindow.EqualRect(prect)) return;

        CRect rcOld = m_rcWindow;
        m_rcWindow = prect;

        OnRelayout(rcOld,m_rcWindow);
    }

    void SWindow::Move(int x,int y, int cx/*=-1*/,int cy/*=-1*/)
    {
        if(cx==-1) cx=m_rcWindow.Width();
        if(cy==-1) cy=m_rcWindow.Height();
        CRect rcNew(x,y,x+cx,y+cy);
        Move(&rcNew);
    }

    // Set current cursor, when hover
    BOOL SWindow::OnSetCursor(const CPoint &pt)
    {
        HCURSOR hCursor=GETRESPROVIDER->LoadCursor(m_style.m_strCursor);
        ::SetCursor(hCursor);
        return TRUE;
    }

    // Get SWindow state
    DWORD SWindow::GetState(void)
    {
        return m_dwState;
    }

    // Modify SWindow state
    DWORD SWindow::ModifyState(DWORD dwStateAdd, DWORD dwStateRemove,BOOL bUpdate/*=FALSE*/)
    {
        TestMainThread();

        DWORD dwOldState = m_dwState;

        DWORD dwNewState = m_dwState;
        dwNewState &= ~dwStateRemove;
        dwNewState |= dwStateAdd;

        OnStateChanging(dwOldState,dwNewState);

        m_dwState = dwNewState;

        OnStateChanged(dwOldState,dwNewState);
        if(bUpdate && NeedRedrawWhenStateChange()) InvalidateRect(m_rcWindow);
        return dwOldState;
    }

    ULONG_PTR SWindow::GetUserData()
    {
        return m_uData;
    }

    ULONG_PTR SWindow::SetUserData(ULONG_PTR uData)
    {
        ULONG_PTR uOld=m_uData;
        m_uData=uData;
        return uOld;
    }

    BOOL SWindow::SetTimer(char id,UINT uElapse)
    {
        STimerID timerID(m_swnd,id);
        return ::SetTimer(GetContainer()->GetHostHwnd(),DWORD(timerID),uElapse,NULL);
    }

    void SWindow::KillTimer(char id)
    {
        STimerID timerID(m_swnd,id);
        ::KillTimer(GetContainer()->GetHostHwnd(),DWORD(timerID));
    }


    BOOL SWindow::SetTimer2( UINT_PTR id,UINT uElapse )
    {
        return STimer2::SetTimer(m_swnd,id,uElapse);
    }

    void SWindow::KillTimer2( UINT_PTR id )
    {
        STimer2::KillTimer(m_swnd,id);
    }

    SWND SWindow::GetSwnd()
    {
        return m_swnd;
    }


    SWindow *SWindow::GetParent()
    {
        return m_pParent;
    }


    SWindow * SWindow::GetTopLevelParent()
    {
        SWindow *pParent=this;
        while(pParent->GetParent()) pParent=pParent->GetParent();
        return pParent;
    }


    BOOL SWindow::DestroyChild(SWindow *pChild)
    {
        TestMainThread();
        if(this != pChild->GetParent()) return FALSE;
        pChild->Invalidate();
        pChild->SSendMessage(WM_DESTROY);
        RemoveChild(pChild);
        pChild->Release();
        return TRUE;
    }

    UINT SWindow::GetChildrenCount()
    {
        return m_nChildrenCount;
    }

    void SWindow::InsertChild(SWindow *pNewChild,SWindow *pInsertAfter/*=ICWND_LAST*/)
    {
        TestMainThread();
        pNewChild->SetContainer(GetContainer());
        pNewChild->m_pParent=this;
        pNewChild->m_pPrevSibling=pNewChild->m_pNextSibling=NULL;

        if(pInsertAfter==m_pLastChild) pInsertAfter=ICWND_LAST;

        if(pInsertAfter==ICWND_LAST)
        {
            //insert window at head
            pNewChild->m_pPrevSibling=m_pLastChild;
            if(m_pLastChild) m_pLastChild->m_pNextSibling=pNewChild;
            else m_pFirstChild=pNewChild;
            m_pLastChild=pNewChild;
        }
        else if(pInsertAfter==ICWND_FIRST)
        {
            //insert window at tail
            pNewChild->m_pNextSibling=m_pFirstChild;
            if(m_pFirstChild) m_pFirstChild->m_pPrevSibling=pNewChild;
            else m_pLastChild=pNewChild;
            m_pFirstChild=pNewChild;
        }
        else
        {
            //insert window at middle
            SASSERT(pInsertAfter->m_pParent == this);
            SASSERT(m_pFirstChild && m_pLastChild);
            SWindow *pNext=pInsertAfter->m_pNextSibling;
            SASSERT(pNext);
            pInsertAfter->m_pNextSibling=pNewChild;
            pNewChild->m_pPrevSibling=pInsertAfter;
            pNewChild->m_pNextSibling=pNext;
            pNext->m_pPrevSibling=pNewChild;
        }
        m_nChildrenCount++;
        
        //ֻ�ڲ����¿ؼ�ʱ��Ҫ���zorderʧЧ,ɾ���ؼ�����Ҫ���
        GetContainer()->MarkWndTreeZorderDirty();
    }

    BOOL SWindow::RemoveChild(SWindow *pChild)
    {
        TestMainThread();
        if(this != pChild->GetParent()) return FALSE;
        SWindow *pPrevSib=pChild->m_pPrevSibling;
        SWindow *pNextSib=pChild->m_pNextSibling;
        if(pPrevSib) pPrevSib->m_pNextSibling=pNextSib;
        else m_pFirstChild=pNextSib;
        if(pNextSib) pNextSib->m_pPrevSibling=pPrevSib;
        else m_pLastChild=pPrevSib;
        pChild->m_pParent=NULL;
        m_nChildrenCount--;
        return TRUE;
    }

    BOOL SWindow::IsChecked()
    {
        return WndState_Check == (m_dwState & WndState_Check);
    }

    BOOL SWindow::IsDisabled(BOOL bCheckParent /*= FALSE*/)
    {
        if(bCheckParent) return m_dwState & WndState_Disable;
        else return m_bDisable;
    }

    BOOL SWindow::IsVisible(BOOL bCheckParent /*= FALSE*/)
    {
        if(bCheckParent) return (0 == (m_dwState & WndState_Invisible));
        else return m_bVisible;
    }

    //��ΪNotifyInvalidateRectֻ�д��ڿɼ�ʱ��֪ͨˢ�£������ڴ��ڿɼ�״̬�ı�ǰ��ִ��һ��֪ͨ��
    void SWindow::SetVisible(BOOL bVisible,BOOL bUpdate/*=FALSE*/)
    {
        if(bUpdate) InvalidateRect(m_rcWindow);
        SSendMessage(WM_SHOWWINDOW,bVisible);
        if(bUpdate) InvalidateRect(m_rcWindow);
    }

    void SWindow::EnableWindow( BOOL bEnable,BOOL bUpdate)
    {
        SSendMessage(WM_ENABLE,bEnable);
        if(bUpdate) InvalidateRect(m_rcWindow);
    }

    void SWindow::SetCheck(BOOL bCheck)
    {
        if (bCheck)
            ModifyState(WndState_Check, 0,TRUE);
        else
            ModifyState(0, WndState_Check,TRUE);
    }

    ISwndContainer *SWindow::GetContainer()
    {
        return m_pContainer;
    }

    void SWindow::SetContainer(ISwndContainer *pContainer)
    {
        TestMainThread();
        m_pContainer=pContainer;
        SWindow *pChild=GetWindow(GSW_FIRSTCHILD);
        while(pChild)
        {
            pChild->SetContainer(pContainer);
            pChild=pChild->GetWindow(GSW_NEXTSIBLING);
        }
    }

    void SWindow::SetOwner(SWindow *pOwner)
    {
        m_pOwner=pOwner;
    }

    SWindow *SWindow::GetOwner()
    {
        return m_pOwner;
    }

    BOOL SWindow::IsMsgTransparent()
    {
        return m_bMsgTransparent;
    }

    // add by dummyz@126.com
    SwndStyle& SWindow::GetStyle()
    {
        return m_style;
    }

    //���ù�������㷨�����ؼ�,�����𼶲��� 2014��12��8��
    SWindow* SWindow::FindChildByID(int id, int nDeep/* =-1*/)
    {
        if(id == 0 || nDeep ==0) return NULL;


        SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
        while(pChild)
        {
            if (pChild->GetID() == id)
                return pChild;
            pChild = pChild->GetWindow(GSW_NEXTSIBLING);
        }

        if(nDeep>0) nDeep--;
        if(nDeep==0) return NULL;

        pChild = GetWindow(GSW_FIRSTCHILD);
        while(pChild)
        {
            SWindow *pChildFind=pChild->FindChildByID(id,nDeep);
            if(pChildFind) return pChildFind;
            pChild = pChild->GetWindow(GSW_NEXTSIBLING);
        }

        return NULL;
    }

    SWindow* SWindow::FindChildByName( LPCWSTR pszName , int nDeep)
    {
        if(!pszName || nDeep ==0) return NULL;

        SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
        while(pChild)
        {
            if (pChild->m_strName == pszName)
                return pChild;
            pChild = pChild->GetWindow(GSW_NEXTSIBLING);
        }

        if(nDeep>0) nDeep--;
        if(nDeep==0) return NULL;

        pChild = GetWindow(GSW_FIRSTCHILD);
        while(pChild)
        {
            SWindow *pChildFind=pChild->FindChildByName(pszName,nDeep);
            if(pChildFind) return pChildFind;
            pChild = pChild->GetWindow(GSW_NEXTSIBLING);
        }

        return NULL;
    }

    BOOL SWindow::CreateChildren(pugi::xml_node xmlNode)
    {
        TestMainThread();
        for (pugi::xml_node xmlChild=xmlNode.first_child(); xmlChild; xmlChild=xmlChild.next_sibling())
        {
            if(xmlChild.type() != pugi::node_element) continue;

            if(_wcsicmp(xmlChild.name(),L"include")==0)
            {//�ڴ��ڲ�����֧��include��ǩ
                pugi::xml_document xmlDoc;
                SStringTList strLst;

                if(2 == SplitString(S_CW2T(xmlChild.attribute(L"src").value()),_T(':'),strLst))
                {
                    LOADXML(xmlDoc,strLst[1],strLst[0]);
                }else
                {
                    LOADXML(xmlDoc,strLst[0],RT_LAYOUT);
                }
                if(xmlDoc)
                {
                    CreateChildren(xmlDoc.child(L"include"));
                }else
                {
                    SASSERT(FALSE);
                }

            }else if(!xmlChild.get_userdata())//ͨ��userdata�����һ���ڵ��Ƿ���Ժ���
            {
                SWindow *pChild = SApplication::getSingleton().CreateWindowByName(xmlChild.name());
                if(pChild)
                {
                    InsertChild(pChild);
                    pChild->InitFromXml(xmlChild);
                }
            }
        }
        return TRUE;
    }


    SStringW SWindow::tr( const SStringW &strSrc )
    {
        return TR(strSrc,GetContainer()->GetTranslatorContext());
    }

    // Create SWindow from xml element
    BOOL SWindow::InitFromXml(pugi::xml_node xmlNode)
    {
        TestMainThread();
        SASSERT(m_pContainer);
        if (xmlNode)
        {
            m_strText = S_CW2T(tr(xmlNode.text().get()));   //ʹ�����԰����롣

            if (!m_strText.IsEmpty())
            {
                m_strText.TrimBlank();
                if (!m_strText.IsEmpty()) BUILDSTRING(m_strText);
            }

            m_layout.Clear();

            //��ǲ�����width , height and size����
            xmlNode.attribute(L"width").set_userdata(1);
            xmlNode.attribute(L"height").set_userdata(1);
            xmlNode.attribute(L"size").set_userdata(1);

            SObject::InitFromXml(xmlNode);

            if(!m_bVisible || (m_pParent && !m_pParent->IsVisible(TRUE)))
                ModifyState(WndState_Invisible, 0);

            if (4 != m_layout.nCount)
            {
                m_layout.InitWidth(xmlNode.attribute(L"width").value());
                m_layout.InitHeight(xmlNode.attribute(L"height").value());
                m_layout.InitSizeFromString(xmlNode.attribute(L"size").value());
            }
        }

        if(0!=SSendMessage(WM_CREATE))
        {
            if(m_pParent)    m_pParent->DestroyChild(this);
            return FALSE;
        }
        CreateChildren(xmlNode);
        return TRUE;
    }

    SWindow * SWindow::CreateChildren(LPCWSTR pszXml)
    {
        pugi::xml_document xmlDoc;
        if(!xmlDoc.load_buffer(pszXml,wcslen(pszXml)*sizeof(wchar_t),pugi::parse_default,pugi::encoding_utf16)) return NULL;
        BOOL bLoaded=CreateChildren(xmlDoc);
        if(!bLoaded) return NULL;
        UpdateChildrenPosition();
        return m_pLastChild;
    }

    // Hittest children
    SWND SWindow::SwndFromPoint(CPoint ptHitTest, BOOL bOnlyText)
    {
        if (!m_rcWindow.PtInRect(ptHitTest)) return NULL;

        CRect rcClient;
        GetClientRect(&rcClient);

        if(!rcClient.PtInRect(ptHitTest))
            return m_swnd;    //ֻ�����λ�ڿͻ���ʱ���ż��������Ӵ���

        SWND swndChild = NULL;

        SWindow *pChild=GetWindow(GSW_LASTCHILD);
        while(pChild)
        {
            if (pChild->IsVisible(TRUE) && !pChild->IsMsgTransparent())
            {
                swndChild = pChild->SwndFromPoint(ptHitTest, bOnlyText);

                if (swndChild) return swndChild;
            }

            pChild=pChild->GetWindow(GSW_PREVSIBLING);
        }

        return m_swnd;
    }

    BOOL SWindow::NeedRedrawWhenStateChange()
    {
        if (m_pBgSkin && !m_pBgSkin->IgnoreState())
        {
            return TRUE;
        }
        return m_style.GetStates()>1;
    }

    //�����ǰ�����л��ƻ��棬����������cache���Զ���ģ�Ҳ���������ڶ�����alpha
    void SWindow::_PaintWindowClient(IRenderTarget *pRT)
    {
        if(IsLayeredWindow())
        {
            SASSERT((pRT == m_cachedRT)); 
        }

        if(IsDrawToCache() && !IsLayeredWindow() )
        {
            IRenderTarget *pRTCache=m_cachedRT;
            if(pRTCache)
            {//�ڴ������ڴ�����ʱ�����pRTCache����ΪNULL
                CRect rcWnd=m_rcWindow;
                if(IsCacheDirty())
                {
                    pRTCache->SetViewportOrg(-rcWnd.TopLeft());
                    pRTCache->BitBlt(&rcWnd,pRT,rcWnd.left,rcWnd.top,SRCCOPY);//�Ѹ����ڵ����ݸ��ƹ�����
                    CAutoRefPtr<IFont> oldFont;
                    COLORREF crOld=pRT->GetTextColor();
                    pRTCache->SelectObject(pRT->GetCurrentObject(OT_FONT),(IRenderObj**)&oldFont);
                    pRTCache->SetTextColor(crOld);

                    SSendMessage(WM_ERASEBKGND, (WPARAM)pRTCache);
                    SSendMessage(WM_PAINT, (WPARAM)pRTCache);

                    pRTCache->SelectObject(oldFont);
                    pRTCache->SetTextColor(crOld);

                    MarkCacheDirty(false);
                }
                if(m_style.m_byAlpha == 0xFF)
                    pRT->BitBlt(&rcWnd,pRTCache,rcWnd.left,rcWnd.top,SRCCOPY);
                else
                    pRT->AlphaBlend(&rcWnd,pRTCache,&rcWnd,m_style.m_byAlpha);
            }
        }else
        {
            SSendMessage(WM_ERASEBKGND, (WPARAM)pRT);
            SSendMessage(WM_PAINT, (WPARAM)pRT);
        }
    }

    void SWindow::_PaintWindowNonClient( IRenderTarget *pRT )
    {
        if(IsDrawToCache() && !IsLayeredWindow() )
        {
            IRenderTarget *pRTCache=m_cachedRT;
            if(pRTCache)
            {
                CRect rcWnd;
                GetWindowRect(&rcWnd);
                CRect rcClient;
                GetClientRect(&rcClient);

                SSendMessage(WM_NCPAINT, (WPARAM)pRTCache);
                pRT->PushClipRect(&rcClient,RGN_DIFF);

                if(m_style.m_byAlpha == 0xFF)
                    pRT->BitBlt(&rcWnd,pRTCache,rcWnd.left,rcWnd.top,SRCCOPY);
                else
                    pRT->AlphaBlend(&rcWnd,pRTCache,&rcWnd,m_style.m_byAlpha);
                pRT->PopClip();
            }
        }else
        {
            SSendMessage(WM_NCPAINT, (WPARAM)pRT);
        }
    }
    
    //paint zorder in [iZorderBegin,iZorderEnd) widnows
    void SWindow::_PaintRegion2( IRenderTarget *pRT, IRegion *pRgn,UINT iZorderBegin,UINT iZorderEnd )
    {
        if(!IsVisible(TRUE))  //ֻ���Լ���ȫ�ɼ�������²Ż���
            return;
        
        CRect rcWnd,rcClient;
        GetWindowRect(&rcWnd);
        GetClientRect(&rcClient);
        
        IRenderTarget *pRTBack = pRT;//backup current RT
        
        if(IsLayeredWindow() && pRTBack != GetCachedRenderTarget())
        {//���Ƶ����ڵĻ�����,��Ҫ�̳�ԭRT�Ļ�ͼ����
            //������ƣ����ര�ڱض�Ҫ����Ⱦ��zorder��Χ�ڣ��������ʵ��������
            SASSERT(m_uZorder >= iZorderBegin && m_uZorder < iZorderEnd);

            CAutoRefPtr<IFont> curFont;
            HRESULT hr = pRT->SelectDefaultObject(OT_FONT,(IRenderObj**)&curFont);
            COLORREF crTxt = pRT->GetTextColor();
            
            pRT = GetCachedRenderTarget();
            pRT->ClearRect(&m_rcWindow,0);
            if(SUCCEEDED(hr)) pRT->SelectObject(curFont);
            pRT->SetTextColor(crTxt);
        }
        
        if(IsClipClient())
        {
            pRT->PushClipRect(rcClient);
        }
        if(m_uZorder >= iZorderBegin
            && m_uZorder < iZorderEnd 
            && (!pRgn || pRgn->IsEmpty() || pRgn->RectInRegion(&rcWnd)))
        {//paint client
            _PaintWindowClient(pRT);
        }

        SPainter painter;
        BeforePaint(pRT,painter);

        SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
        while(pChild)
        {
            if(pChild->m_uZorder >= iZorderEnd) break;
            if(pChild->m_uZorder< iZorderBegin)
            {//��������֦��zorder�ǲ����ڻ��Ʒ�Χ��
                SWindow *pNextChild = pChild->GetWindow(GSW_NEXTSIBLING);
                if(pNextChild)
                {
                    if(pNextChild->m_uZorder<=iZorderBegin)
                    {
                        pChild = pNextChild;
                        continue;
                    }
                }else
                {//���һ���ڵ�ʱ�鿴����Ӵ��ڵ�zorder
                    SWindow *pLastChild = pChild;
                    while(pLastChild->GetChildrenCount())
                    {
                        pLastChild = pLastChild->GetWindow(GSW_LASTCHILD);
                    }
                    if(pLastChild->m_uZorder < iZorderBegin)
                    {
                        break;
                    }
                }
            }
            pChild->_PaintRegion2(pRT,pRgn,iZorderBegin,iZorderEnd);
            pChild = pChild->GetWindow(GSW_NEXTSIBLING);
        }
        AfterPaint(pRT,painter);

        if(IsClipClient())
        {
            pRT->PopClip();
        }

        if(m_uZorder >= iZorderBegin
            && m_uZorder < iZorderEnd 
            && (!pRgn ||  pRgn->IsEmpty() ||pRgn->RectInRegion(&rcWnd)) 
            && (rcWnd!=rcClient))
        {//paint nonclient
            _PaintWindowNonClient(pRT);
        }
        
        if(IsLayeredWindow() && pRTBack!=GetCachedRenderTarget())
        {//�����Ƶ����ڵĻ����ϵ�ͼ�񷵻ص���һ��RT
            pRTBack->AlphaBlend(&m_rcWindow,pRT,&m_rcWindow,m_style.m_byAlpha);

            CAutoRefPtr<IFont> curFont;
            HRESULT hr = pRT->SelectDefaultObject(OT_FONT,(IRenderObj**)&curFont);
            
            pRT = pRTBack;
            if(SUCCEEDED(hr)) pRT->SelectObject(curFont);
            
            MarkCacheDirty(true);
        }

    }

    //��ǰ�����еĲ�������zorder,Ϊ�˱�֤���ݽ�����zorder����ȷ��,�������������zorder�ؽ�.
    void SWindow::_PaintRegion(IRenderTarget *pRT, IRegion *pRgn,UINT iZorderBegin,UINT iZorderEnd)
    {
        TestMainThread();
        BeforePaintEx(pRT);
        _PaintRegion2(pRT,pRgn,iZorderBegin,iZorderEnd);
    }

    void SWindow::RedrawRegion(IRenderTarget *pRT, IRegion *pRgn)
    {
        TestMainThread();
        _PaintRegion2(pRT,pRgn,ZORDER_MIN,ZORDER_MAX);
    }

    void SWindow::Invalidate()
    {
        CRect rcClient;
        GetClientRect(&rcClient);
        InvalidateRect(rcClient);
    }

    void SWindow::InvalidateRect(LPCRECT lprect)
    {
        if (lprect)
        {
            CRect rect = *lprect;
            InvalidateRect(rect);
        }else
        {
            InvalidateRect(m_rcWindow);
        }
    }

    void SWindow::InvalidateRect(const CRect& rect)
    {
        TestMainThread();
        MarkCacheDirty(true);
        if(!IsVisible(TRUE) || IsUpdateLocked()) return ;
        //ֻ�ܸ��´�����Ч����
        CRect rcIntersect = rect & m_rcWindow;
        if(rcIntersect.IsRectEmpty()) return;

        if(!m_style.m_bBkgndBlend)
        {//�Ǳ�����ϴ��ڣ�ֱ�ӷ���Ϣ֧��������������ˢ��
            if(!m_invalidRegion)
            {
                GETRENDERFACTORY->CreateRegion(&m_invalidRegion);
            }
            m_invalidRegion->CombineRect(rcIntersect,RGN_OR);
            ::SendMessage(GetContainer()->GetHostHwnd(),UM_UPDATESWND,(WPARAM)m_swnd,0);//����ˢ�´���
        }else
        {
            if(GetParent())
            {
                GetParent()->InvalidateRect(rcIntersect);
            }else
            {
                GetContainer()->OnRedraw(rcIntersect);
            }
        }
    }

    void SWindow::LockUpdate()
    {
        m_bUpdateLocked=TRUE;
    }

    void SWindow::UnlockUpdate()
    {
        m_bUpdateLocked=FALSE;
    }

    BOOL SWindow::IsUpdateLocked()
    {
        return m_bUpdateLocked;
    }

    void SWindow::BringWindowToTop()
    {
        TestMainThread();
        SWindow *pParent=GetParent();
        if(!pParent) return;
        pParent->RemoveChild(this);
        pParent->InsertChild(this);
    }

    BOOL SWindow::FireEvent(EventArgs &evt)
    {
        TestMainThread();
        m_evtSet.FireEvent(evt);
        if(evt.handled != 0) return TRUE;

        if(GetOwner()) return GetOwner()->FireEvent(evt);
        return GetContainer()->OnFireEvent(evt);
    }


    void SWindow::OnRelayout(const CRect &rcOld, const CRect & rcNew)
    {
        SWindow *pParent= GetParent();
        if(pParent)
        {
            pParent->InvalidateRect(rcOld);
            pParent->InvalidateRect(rcNew);
        }else
        {
            InvalidateRect(m_rcWindow);
        }

        SSendMessage(WM_NCCALCSIZE);//����ǿͻ�����С

        CRect rcClient;
        GetClientRect(&rcClient);
        SSendMessage(WM_SIZE,0,MAKELPARAM(rcClient.Width(),rcClient.Height()));

        UpdateChildrenPosition();
    }

    int SWindow::OnCreate( LPVOID )
    {
        if(m_style.m_bTrackMouseEvent)
            GetContainer()->RegisterTrackMouseEvent(m_swnd);
        else
            GetContainer()->UnregisterTrackMouseEvent(m_swnd);

        return 0;
    }

    void SWindow::OnDestroy()
    {
        //destroy children windows
        SWindow *pChild=m_pFirstChild;
        while (pChild)
        {
            SWindow *pNextChild=pChild->m_pNextSibling;
            pChild->SSendMessage(WM_DESTROY);
            pChild->Release();

            pChild=pNextChild;
        }
        m_pFirstChild=m_pLastChild=NULL;
        m_nChildrenCount=0;
    }

    // Draw background default
    BOOL SWindow::OnEraseBkgnd(IRenderTarget *pRT)
    {
        CRect rcClient;
        GetClientRect(&rcClient);
        if (!m_pBgSkin)
        {
            COLORREF crBg = m_style.m_crBg;

            if (CR_INVALID != crBg)
            {
                pRT->FillSolidRect(&rcClient,crBg);
            }
        }
        else
        {
            int nState=0;

            if(GetState()&WndState_Disable)
            {
                nState=3;
            }
            else if(GetState()&WndState_Check || GetState()&WndState_PushDown)
            {
                nState=2;
            }else if(GetState()&WndState_Hover)
            {
                nState=1;
            }
            if(nState>=m_pBgSkin->GetStates()) nState=0;
            m_pBgSkin->Draw(pRT, rcClient, nState); 
        }
        return TRUE;
    }

    void SWindow::BeforePaint(IRenderTarget *pRT, SPainter &painter)
    {
        IFontPtr pFont = m_style.GetTextFont(IIF_STATE4(m_dwState,0,1,2,3));
        if(pFont) 
            pRT->SelectObject(pFont,(IRenderObj**)&painter.pOldPen);

        COLORREF crTxt = m_style.GetTextColor(IIF_STATE4(m_dwState,0,1,2,3));
        if(crTxt != CR_INVALID)
            painter.crOld = pRT->SetTextColor(crTxt);
    }

    void SWindow::BeforePaintEx(IRenderTarget *pRT)
    {
        SWindow *pParent=GetParent();
        if(pParent) pParent->BeforePaintEx(pRT);
        SPainter painter;
        BeforePaint(pRT,painter);
    }

    void SWindow::AfterPaint(IRenderTarget *pRT, SPainter &painter)
    {
        if(painter.pOldPen) pRT->SelectObject(painter.pOldPen);
        if(painter.crOld!=CR_INVALID) pRT->SetTextColor(painter.crOld);
    }

    // Draw inner text default and focus rect
    void SWindow::OnPaint(IRenderTarget *pRT)
    {
        SPainter painter;

        BeforePaint(pRT, painter);

        CRect rcText;
        GetTextRect(rcText);
        DrawText(pRT,m_strText, m_strText.GetLength(), rcText, GetTextAlign());

        //draw focus rect
        if(GetContainer()->SwndGetFocus()==m_swnd)
        {
            DrawFocus(pRT);
        }

        AfterPaint(pRT, painter);
    }

    void SWindow::OnNcPaint(IRenderTarget *pRT)
    {
        if(m_style.m_nMarginX!=0 || m_style.m_nMarginY!=0)
        {
            BOOL bGetRT = (pRT==0);
            if(bGetRT)
            {
                pRT=GetRenderTarget(&m_rcWindow,OLEDC_OFFSCREEN,FALSE);//���Զ�������
                PaintBackground(pRT,&m_rcWindow);
            }
            else
            {
                CRect rcClient;
                GetClientRect(&rcClient);
                pRT->PushClipRect(&rcClient,RGN_DIFF);
            }

            int nState=0;
            if(WndState_Hover & m_dwState) nState=1;
            if(m_pNcSkin)
            {
                if(nState>=m_pNcSkin->GetStates()) nState=0;
                m_pNcSkin->Draw(pRT,m_rcWindow,nState);
            }
            else
            {
                COLORREF crBg = m_style.m_crBorder;
                if (CR_INVALID != crBg)
                {
                     pRT->FillSolidRect(&m_rcWindow,crBg);
                }
            }
            if(bGetRT)
            {
                PaintForeground(pRT,&m_rcWindow);
                ReleaseRenderTarget(pRT);
            }
            else
            {
                pRT->PopClip();
            }
        }
    }

    CSize SWindow::GetDesiredSize(LPCRECT pRcContainer)
    {
        SASSERT(m_layout.IsFitContent(PD_ALL));


        int nTestDrawMode = GetTextAlign() & ~(DT_CENTER | DT_RIGHT | DT_VCENTER | DT_BOTTOM);

        CRect rcTest (0,0,0x7FFF,0x7FFF);
        if(m_nMaxWidth!=-1)
        {
            rcTest.right=m_nMaxWidth;
            nTestDrawMode|=DT_WORDBREAK;
        }

        CAutoRefPtr<IRenderTarget> pRT;
        GETRENDERFACTORY->CreateRenderTarget(&pRT,0,0);
        BeforePaintEx(pRT);
        DrawText(pRT,m_strText, m_strText.GetLength(), rcTest, nTestDrawMode | DT_CALCRECT);
        rcTest.right += m_style.m_nMarginX * 2;
        rcTest.bottom += m_style.m_nMarginY * 2;

        CSize szRet = rcTest.Size();
        if(!m_layout.IsFitContent(PD_X)) szRet.cx = m_layout.uSpecifyWidth;
        if(!m_layout.IsFitContent(PD_Y)) szRet.cy = m_layout.uSpecifyHeight;
        return szRet;
    }

    void SWindow::GetTextRect( LPRECT pRect )
    {
        GetClientRect(pRect);
    }

    void SWindow::DrawText(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat)
    {
        pRT->DrawText(pszBuf,cchText,pRect,uFormat);
    }

    void SWindow::DrawFocus(IRenderTarget *pRT)
    {
        CRect rcFocus;
        GetTextRect(&rcFocus);
        if(IsFocusable())    DrawDefFocusRect(pRT,rcFocus);
    }


    void SWindow::DrawDefFocusRect(IRenderTarget *pRT,CRect rcFocus )
    {
        rcFocus.DeflateRect(2,2);
        CAutoRefPtr<IPen> pPen,oldPen;
        pRT->CreatePen(PS_DOT,RGBA(88,88,88,0xFF),1,&pPen);
        pRT->SelectObject(pPen,(IRenderObj**)&oldPen);
        pRT->DrawRectangle(&rcFocus);    
        pRT->SelectObject(oldPen);
    }

    UINT SWindow::OnGetDlgCode()
    {
        return 0;
    }

    BOOL SWindow::IsFocusable()
    {
        return m_bFocusable;
    }

    void SWindow::OnShowWindow(BOOL bShow, UINT nStatus)
    {
        if(nStatus == ParentShow)
        {
            if(bShow && !IsVisible(FALSE)) bShow=FALSE;
        }
        else
        {
            m_bVisible=bShow;
        }
        if(bShow && m_pParent)
        {
            bShow=m_pParent->IsVisible(TRUE);
        }

        if (bShow)
            ModifyState(0, WndState_Invisible);
        else
            ModifyState(WndState_Invisible, 0);

        SWindow *pChild=m_pFirstChild;
        while(pChild)
        {
            pChild->AddRef();
            pChild->SSendMessage(WM_SHOWWINDOW,bShow,ParentShow);
            SWindow *pNextChild=pChild->GetWindow(GSW_NEXTSIBLING);;
            pChild->Release();
            pChild=pNextChild;
        }
        if(!IsVisible(TRUE) && m_swnd == GetContainer()->SwndGetFocus())
        {
            GetContainer()->OnSetSwndFocus(NULL);
        }

        if(!m_bDisplay)
        {
            SWindow *pParent=GetParent();
            if(pParent) pParent->UpdateChildrenPosition();
        }
    }


    void SWindow::OnEnable( BOOL bEnable,UINT nStatus )
    {
        if(nStatus == ParentEnable)
        {
            if(bEnable && IsDisabled(FALSE)) bEnable=FALSE;
        }
        else
        {
            m_bDisable=!bEnable;
        }
        if(bEnable && m_pParent)
        {
            bEnable=!m_pParent->IsDisabled(TRUE);
        }

        if (bEnable)
            ModifyState(0, WndState_Disable);
        else
            ModifyState(WndState_Disable, WndState_Hover);

        SWindow *pChild=m_pFirstChild;
        while(pChild)
        {
            pChild->SSendMessage(WM_ENABLE,bEnable,ParentEnable);
            pChild=pChild->GetWindow(GSW_NEXTSIBLING);
        }
        if(IsDisabled(TRUE) && m_swnd == GetContainer()->SwndGetFocus())
        {
            GetContainer()->OnSetSwndFocus(NULL);
        }
    }

    void SWindow::OnLButtonDown(UINT nFlags,CPoint pt)
    {
        if(m_bFocusable) SetFocus();
        SetCapture();
        ModifyState(WndState_PushDown, 0,TRUE);
    }

    void SWindow::OnLButtonUp(UINT nFlags,CPoint pt)
    {
        ReleaseCapture();
        ModifyState(0, WndState_PushDown,TRUE);
        if(!m_rcWindow.PtInRect(pt)) return;


        if (GetID() || GetName())
        {
            FireCommand();
        }
    }

    void SWindow::OnRButtonDown( UINT nFlags, CPoint point )
    {
        FireCtxMenu(point);
    }

    void SWindow::OnMouseHover(WPARAM wParam, CPoint ptPos)
    {
        if(GetCapture()==m_swnd)
            ModifyState(WndState_PushDown,0,FALSE);
        ModifyState(WndState_Hover, 0,TRUE);
        OnNcPaint(0);
        EventCmnArgs evtHover(this,EVT_MOUSE_HOVER);
        FireEvent(evtHover);
    }

    void SWindow::OnMouseLeave()
    {
        if(GetCapture()==m_swnd)
            ModifyState(0,WndState_PushDown,FALSE);
        ModifyState(0,WndState_Hover,TRUE);
        OnNcPaint(0);
        EventCmnArgs evtLeave(this,EVT_MOUSE_LEAVE);
        FireEvent(evtLeave);
    }

    BOOL SWindow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
    {
        BOOL bRet=FALSE;
        if(m_pParent) bRet=(BOOL)m_pParent->SSendMessage(WM_MOUSEWHEEL,MAKEWPARAM(nFlags,zDelta),MAKELPARAM(pt.x,pt.y));
        return bRet;
    }

    CRect SWindow::GetChildrenLayoutRect()
    {
        CRect rcRet;
        GetClientRect(rcRet);//ͨ���ǿͻ���������tab�����Ŀؼ���һ����
        return rcRet;
    }

    void SWindow::UpdateChildrenPosition()
    {
        SList<SWindowRepos*> lstWnd;
        SWindow *pChild=GetWindow(GSW_FIRSTCHILD);
        while(pChild)
        {
            if(!pChild->m_bFloat)
                lstWnd.AddTail(new SWindowRepos(pChild));
            pChild=pChild->GetWindow(GSW_NEXTSIBLING);
        }
        SwndLayoutBuilder::CalcChildrenPosition(&lstWnd,GetChildrenLayoutRect());
    }

    void SWindow::OnSetFocus()
    {
        EventSetFocus evt(this);
        FireEvent(evt);
        InvalidateRect(m_rcWindow);
    }

    void SWindow::OnKillFocus()
    {
        EventKillFocus evt(this);
        FireEvent(evt);
        InvalidateRect(m_rcWindow);
    }

    //�������а�͸�����Բ���͸����Ҫ��ҪӦ�����Ӵ���ʱ���Ӵ��ڵ�ͼ����Ⱦ��this�Ļ���RT�ϡ�
    BOOL SWindow::IsLayeredWindow()
    {
        return m_style.m_bLayeredWindow;
    }

    //��ѯ��ǰ�������ݽ�����Ⱦ����һ����Ⱦ���ϣ�û����Ⱦ��ʱ����NULL
    SWindow * SWindow::_GetCurrentLayeredWindow()
    {
        SWindow *pWnd = this;
        while(pWnd)
        {
            if(pWnd->IsLayeredWindow())
            {
                break;
            }
            pWnd = pWnd->GetParent();
        }

        return pWnd;
    }

    IRenderTarget * SWindow::GetRenderTarget(const LPRECT pRc/*=NULL*/,DWORD gdcFlags/*=0*/,BOOL bClientDC/*=TRUE*/)
    {
        CRect rcRT;        
        if(bClientDC)
            GetClientRect(&rcRT);
        else
            GetWindowRect(&rcRT);
        if(pRc) rcRT.IntersectRect(pRc,&rcRT);
        
        SWindow *pParent = GetParent();
        while(pParent)
        {
            rcRT.IntersectRect(rcRT,pParent->m_rcWindow);
            pParent = pParent->GetParent();
        }
        
        CAutoRefPtr<IRegion> rgn;
        GETRENDERFACTORY->CreateRegion(&rgn);
        rgn->CombineRect(rcRT,RGN_COPY);
        if(!bClientDC)
        {
            CRect rcClient;
            GetClientRect(&rcClient);
            rgn->CombineRect(rcClient,RGN_DIFF);
        }

        //��������һ����Ⱦ���RT
        IRenderTarget *pRT = _GetRenderTarget(rcRT,gdcFlags,m_uZorder+1,rgn);
        BeforePaintEx(pRT);
        return pRT;
    }

    void SWindow::ReleaseRenderTarget(IRenderTarget *pRT)
    {
        _ReleaseRenderTarget(pRT);        
    }
    
    IRenderTarget * SWindow::_GetRenderTarget(CRect & rcGetRT,DWORD gdcFlags,UINT uMinFrgndZorder,IRegion *pRgn)
    {
        IRenderTarget *pRT = NULL;
        SWindow *pLayerWindow = _GetCurrentLayeredWindow();
        
        GETRTDATA *pGetRTData = new GETRTDATA;
        pGetRTData->gdcFlags = gdcFlags;
        pGetRTData->rcRT = rcGetRT;
        pGetRTData->uMinFrgndZorder = uMinFrgndZorder;
        pGetRTData->rgn = pRgn;

        if(pLayerWindow)
        {
            pRT = pLayerWindow->GetCachedRenderTarget();
            pRT->ClearRect(rcGetRT,0);
        }else
        {
            pLayerWindow = GetRoot();
            pRT = GetContainer()->OnGetRenderTarget(rcGetRT,gdcFlags);
        }
        
        //save get RT data
        SASSERT(pLayerWindow->m_pGetRTData == NULL);
        pLayerWindow->m_pGetRTData = pGetRTData;
        
        pRT->PushClipRegion(pRgn,RGN_COPY);
        
        if(gdcFlags & OLEDC_PAINTBKGND)
        {//���Ƶ�ǰ���ڵı���
            GetContainer()->BuildWndTreeZorder();
            pLayerWindow->_PaintRegion(pRT,pRgn,ZORDER_MIN,m_uZorder);
        }
        return pRT;
    }

    
    void SWindow::_ReleaseRenderTarget(IRenderTarget *pRT)
    {
        pRT->SelectDefaultObject(OT_FONT);//���pRT��������ܵ�ռ��

        SWindow *pLayerWindow = _GetCurrentLayeredWindow();
        if(pLayerWindow)
        {//����һ����Ⱦ��
            PGETRTDATA &pGetRTData = pLayerWindow->m_pGetRTData;
            SASSERT(pGetRTData);
            if(pGetRTData->gdcFlags != OLEDC_NODRAW)
            {
                if(pGetRTData->gdcFlags == OLEDC_PAINTBKGND)
                {//��ָ���Ĵ��ڿ�ʼ����ǰ��
                    GetContainer()->BuildWndTreeZorder();
                    pLayerWindow->_PaintRegion(pRT,pGetRTData->rgn,pGetRTData->uMinFrgndZorder,ZORDER_MAX);
                }
                
                SWindow *pParent = pLayerWindow->GetParent();
                SASSERT(pParent);
                
                
                UINT uFrgndZorderMin = ZORDER_MAX;
                //������һ����Ⱦ���ǰ�������ϲ������һ���ֵܣ�ֱ���ҵ�Ϊֹ
                SWindow *pWnd = pLayerWindow;
                while(pWnd)
                {
                    SWindow *pNextSibling = pWnd->GetWindow(GSW_NEXTSIBLING);
                    if(pNextSibling)
                    {
                        uFrgndZorderMin = pNextSibling->m_uZorder;
                        break;
                    }else
                    {
                        pWnd = pWnd->GetParent();
                    }
                }
                
                //�����һ��Ⱦ��(LayerWindow)��RT,����ǰ��ʱ���ٻ��Ƶ�ǰ��(�ӵ�ǰ��zorder���Ĵ���+1��ʼ)
                //Ϊ�˲��ƻ��²���Ⱦ״̬,ֻ�����ϲ�ĵ��ö�����ָ��OLEDC_OFFSCREEN��־,�ڲ������Զ�ʹ��OLEDC_PAINTBKGND
                IRenderTarget *pRT2 = pParent->_GetRenderTarget(pGetRTData->rcRT,OLEDC_PAINTBKGND,uFrgndZorderMin,pGetRTData->rgn);
                pParent->_PaintWindowClient(pRT2);
                pRT2->AlphaBlend(pGetRTData->rcRT,pRT,pGetRTData->rcRT,pLayerWindow->m_style.m_byAlpha);
                pParent->_PaintWindowNonClient(pRT2);
                pParent->_ReleaseRenderTarget(pRT2);
            }
            pRT->PopClip();//��Ӧ_GetRenderTarget�е��õ�PushClipRegion
            delete pGetRTData;
            pGetRTData = NULL;
        }else
        {//�Ѿ�������ײ���
            SWindow *pRoot = GetRoot();
            PGETRTDATA & pGetRTData = pRoot->m_pGetRTData;
            SASSERT(pGetRTData);
            
            if(pGetRTData->gdcFlags == OLEDC_PAINTBKGND)
            {//��ָ���Ĵ��ڿ�ʼ����ǰ��
                GetContainer()->BuildWndTreeZorder();
                pRoot->_PaintRegion(pRT,pGetRTData->rgn,pGetRTData->uMinFrgndZorder,ZORDER_MAX);
            }

            pRT->PopClip();//��Ӧ_GetRenderTarget�е��õ�PushClipRegion
            GetContainer()->OnReleaseRenderTarget(pRT,pGetRTData->rcRT,pGetRTData->gdcFlags);
            delete pGetRTData;
            pGetRTData = NULL;
        }
    }

    SWND SWindow::GetCapture()
    {
        return GetContainer()->OnGetSwndCapture();
    }

    SWND SWindow::SetCapture()
    {
        return GetContainer()->OnSetSwndCapture(m_swnd);
    }

    BOOL SWindow::ReleaseCapture()
    {
        return GetContainer()->OnReleaseSwndCapture();
    }

    void SWindow::SetFocus()
    {
        GetContainer()->OnSetSwndFocus(m_swnd);
    }

    void SWindow::KillFocus()
    {
        if(GetContainer()->SwndGetFocus()==m_swnd)
        {
            GetContainer()->OnSetSwndFocus(NULL);
        }
    }

    BOOL SWindow::OnNcHitTest(CPoint pt)
    {
        return FALSE;
    }

    SWindow * SWindow::GetWindow(int uCode)
    {
        SWindow *pRet=NULL;
        switch(uCode)
        {
        case GSW_FIRSTCHILD:
            pRet=m_pFirstChild;
            break;
        case GSW_LASTCHILD:
            pRet=m_pLastChild;
            break;
        case GSW_PREVSIBLING:
            pRet=m_pPrevSibling;
            break;
        case GSW_NEXTSIBLING:
            pRet=m_pNextSibling;
            break;
        case GSW_OWNER:
            pRet=m_pOwner;
            break;
        case GSW_PARENT:
            pRet=m_pParent;
            break;
        }
        return pRet;
    }


    void SWindow::PaintBackground(IRenderTarget *pRT,LPRECT pRc )
    {
        CRect rcDraw=m_rcWindow;
        if(pRc) rcDraw.IntersectRect(rcDraw,pRc);
        pRT->PushClipRect(&rcDraw);

        SWindow *pTopWnd=GetTopLevelParent();
        CAutoRefPtr<IRegion> pRgn;
        GETRENDERFACTORY->CreateRegion(&pRgn);
        pRgn->CombineRect(&rcDraw,RGN_COPY);

        pRT->ClearRect(&rcDraw,0);//���������alphaֵ
        
        GetContainer()->BuildWndTreeZorder();
        pTopWnd->_PaintRegion(pRT,pRgn,ZORDER_MIN,m_uZorder);

        pRT->PopClip();
    }

    void SWindow::PaintForeground( IRenderTarget *pRT,LPRECT pRc )
    {
        CRect rcDraw=m_rcWindow;
        if(pRc) rcDraw.IntersectRect(rcDraw,pRc);
        CAutoRefPtr<IRegion> pRgn;
        GETRENDERFACTORY->CreateRegion(&pRgn);
        pRgn->CombineRect(&rcDraw,RGN_COPY);
        pRT->PushClipRect(&rcDraw);
        
        GetContainer()->BuildWndTreeZorder();
        GetTopLevelParent()->_PaintRegion(pRT,pRgn,m_uZorder+1,ZORDER_MAX);

        pRT->PopClip();
    }

    void SWindow::DrawAniStep( CRect rcFore,CRect rcBack,IRenderTarget * pRTFore,IRenderTarget * pRTBack,CPoint ptAnchor)
    {
        IRenderTarget * pRT=GetRenderTarget(rcBack,OLEDC_OFFSCREEN,FALSE);
        pRT->BitBlt(&rcBack,pRTBack,rcBack.left,rcBack.top,SRCCOPY);
        pRT->BitBlt(&rcFore,pRTFore,ptAnchor.x,ptAnchor.y,SRCCOPY);
        PaintForeground(pRT,rcBack);//��ǰ��
        ReleaseRenderTarget(pRT);
    }

    void SWindow::DrawAniStep( CRect rcWnd,IRenderTarget * pRTFore,IRenderTarget * pRTBack,BYTE byAlpha)
    {
        IRenderTarget * pRT=GetRenderTarget(rcWnd,OLEDC_OFFSCREEN,FALSE);
        if(byAlpha>0 && byAlpha<255)
        {
            pRT->BitBlt(&rcWnd,pRTBack,rcWnd.left,rcWnd.top,SRCCOPY);
            pRT->AlphaBlend(&rcWnd,pRTFore,&rcWnd,byAlpha);
        }else if(byAlpha==0)
        {
            pRT->BitBlt(&rcWnd,pRTBack,rcWnd.left,rcWnd.top,SRCCOPY);
        }else if(byAlpha==255)
        {
            pRT->BitBlt(&rcWnd,pRTFore,rcWnd.left,rcWnd.top,SRCCOPY);
        }
        PaintForeground(pRT,rcWnd);//��ǰ��
        ReleaseRenderTarget(pRT);
    }

    BOOL SWindow::AnimateWindow(DWORD dwTime,DWORD dwFlags )
    {
        if(dwFlags & AW_HIDE)
        {
            if(!IsVisible(TRUE))
                return FALSE;
        }else
        {//������ʾ����ʱ����������㴰�ڣ�ͬʱ������һ�㴰��Ӧ�ÿɼ�
            if(IsVisible(TRUE))
                return FALSE;
            SWindow *pParent=GetParent();
            if(!pParent) return FALSE;
            if(!pParent->IsVisible(TRUE)) return FALSE;
        }
        CRect rcWnd;
        GetWindowRect(&rcWnd);

        CAutoRefPtr<IRegion> rgn;
        GETRENDERFACTORY->CreateRegion(&rgn);
        rgn->CombineRect(&rcWnd,RGN_COPY);

        IRenderTarget *pRT=GetRenderTarget(rcWnd,OLEDC_NODRAW,FALSE);
        CAutoRefPtr<IRenderTarget> pRTBefore;
        GETRENDERFACTORY->CreateRenderTarget(&pRTBefore,rcWnd.Width(),rcWnd.Height());
        pRTBefore->OffsetViewportOrg(-rcWnd.left,-rcWnd.top);

        //��Ⱦ���ڱ仯ǰ״̬
        PaintBackground(pRT,rcWnd);
        RedrawRegion(pRT,rgn);
        pRTBefore->BitBlt(&rcWnd,pRT,rcWnd.left,rcWnd.top,SRCCOPY);

        //���´��ڿɼ���
        SetVisible(!(dwFlags&AW_HIDE),FALSE);
        //���ڱ仯��
        CAutoRefPtr<IRenderTarget> pRTAfter;
        GETRENDERFACTORY->CreateRenderTarget(&pRTAfter,rcWnd.Width(),rcWnd.Height());
        pRTAfter->OffsetViewportOrg(-rcWnd.left,-rcWnd.top);

        PaintBackground(pRT,rcWnd);
        RedrawRegion(pRT,rgn);
        pRTAfter->BitBlt(&rcWnd,pRT,rcWnd.left,rcWnd.top,SRCCOPY);

        ReleaseRenderTarget(pRT);

        int nSteps=dwTime/10;
        if(dwFlags & AW_HIDE)
        {//hide
            if(dwFlags& AW_SLIDE)
            {
                CRect rcNewState(rcWnd);
                LONG  x1 = rcNewState.left;
                LONG  x2 = rcNewState.left;
                LONG  y1 = rcNewState.top;
                LONG  y2 = rcNewState.top;
                LONG * x =&rcNewState.left;
                LONG * y =&rcNewState.top;

                if(dwFlags & AW_HOR_POSITIVE)
                {//left->right:move left
                    x1=rcNewState.left,x2=rcNewState.right;
                    x=&rcNewState.left;
                }else if(dwFlags & AW_HOR_NEGATIVE)
                {//right->left:move right
                    x1=rcNewState.right,x2=rcNewState.left;
                    x=&rcNewState.right;
                }
                if(dwFlags & AW_VER_POSITIVE)
                {//top->bottom
                    y1=rcNewState.top,y2=rcNewState.bottom;
                    y=&rcNewState.top;
                }else if(dwFlags & AW_VER_NEGATIVE)
                {//bottom->top
                    y1=rcNewState.bottom,y2=rcNewState.top;
                    y=&rcNewState.bottom;
                }
                LONG xStepLen=(x2-x1)/nSteps;
                LONG yStepLen=(y2-y1)/nSteps;

                CPoint ptAnchor;
                for(int i=0;i<nSteps;i++)
                {
                    *x+=xStepLen;
                    *y+=yStepLen;
                    ptAnchor=rcWnd.TopLeft();
                    if(dwFlags & AW_HOR_NEGATIVE)
                    {//right->left:move right
                        ptAnchor.x=rcWnd.right-rcNewState.Width();
                    }
                    if(dwFlags & AW_VER_NEGATIVE)
                    {
                        ptAnchor.y=rcWnd.bottom-rcNewState.Height();
                    }
                    DrawAniStep(rcNewState,rcWnd,pRTBefore,pRTAfter,ptAnchor);
                    Sleep(10);
                }
                DrawAniStep(CRect(),rcWnd,pRTBefore,pRTAfter,rcWnd.TopLeft());
                return TRUE;
            }else if(dwFlags&AW_CENTER)
            {
                CRect rcNewState(rcWnd);
                int xStep=rcNewState.Width()/(2*nSteps);
                int yStep=rcNewState.Height()/(2*nSteps);
                for(int i=0;i<nSteps;i++)
                {
                    rcNewState.DeflateRect(xStep,yStep);
                    DrawAniStep(rcNewState,rcWnd,pRTBefore,pRTAfter,rcNewState.TopLeft());
                    Sleep(10);
                }
                DrawAniStep(CRect(),rcWnd,pRTBefore,pRTAfter,rcWnd.TopLeft());
                return TRUE;
            }else if(dwFlags&AW_BLEND)
            {
                BYTE byAlpha=255;
                BYTE byStepLen=255/nSteps;
                for(int i=0;i<nSteps;i++)
                {
                    DrawAniStep(rcWnd,pRTBefore,pRTAfter,byAlpha);
                    Sleep(10);
                    byAlpha-=byStepLen;
                }
                DrawAniStep(rcWnd,pRTBefore,pRTAfter,0);
                return TRUE;
            }
            return FALSE;
        }else
        {//show
            if(dwFlags& AW_SLIDE)
            {
                CRect rcNewState(rcWnd);
                LONG  x1 = rcNewState.left;
                LONG  x2 = rcNewState.left;
                LONG  y1 = rcNewState.top;
                LONG  y2 = rcNewState.top;
                LONG * x =&rcNewState.left;
                LONG * y =&rcNewState.top;

                if(dwFlags & AW_HOR_POSITIVE)
                {//left->right:move right
                    x1=rcNewState.left,x2=rcNewState.right;
                    rcNewState.right=rcNewState.left,x=&rcNewState.right;
                }else if(dwFlags & AW_HOR_NEGATIVE)
                {//right->left:move left
                    x1=rcNewState.right,x2=rcNewState.left;
                    rcNewState.left=rcNewState.right,x=&rcNewState.left;
                }
                if(dwFlags & AW_VER_POSITIVE)
                {//top->bottom
                    y1=rcNewState.top,y2=rcNewState.bottom;
                    rcNewState.bottom=rcNewState.top,y=&rcNewState.bottom;
                }else if(dwFlags & AW_VER_NEGATIVE)
                {//bottom->top
                    y1=rcNewState.bottom,y2=rcNewState.top;
                    rcNewState.top=rcNewState.bottom,y=&rcNewState.top;
                }
                LONG xStepLen=(x2-x1)/nSteps;
                LONG yStepLen=(y2-y1)/nSteps;

                CPoint ptAnchor;
                for(int i=0;i<nSteps;i++)
                {
                    *x+=xStepLen;
                    *y+=yStepLen;
                    ptAnchor=rcWnd.TopLeft();
                    if(dwFlags & AW_HOR_POSITIVE)
                    {//left->right:move left
                        ptAnchor.x=rcWnd.right-rcNewState.Width();
                    }
                    if(dwFlags & AW_VER_POSITIVE)
                    {
                        ptAnchor.y=rcWnd.bottom-rcNewState.Height();
                    }
                    DrawAniStep(rcNewState,rcWnd,pRTAfter,pRTBefore,ptAnchor);
                    Sleep(10);
                }
                DrawAniStep(rcWnd,rcWnd,pRTAfter,pRTBefore,rcWnd.TopLeft());
                return TRUE;
            }else if(dwFlags&AW_CENTER)
            {
                CRect rcNewState(rcWnd);
                int xStep=rcNewState.Width()/(2*nSteps);
                int yStep=rcNewState.Height()/(2*nSteps);
                rcNewState.left=rcNewState.right=(rcNewState.left+rcNewState.right)/2;
                rcNewState.top=rcNewState.bottom=(rcNewState.top+rcNewState.bottom)/2;
                for(int i=0;i<nSteps;i++)
                {
                    rcNewState.InflateRect(xStep,yStep);
                    DrawAniStep(rcNewState,rcWnd,pRTAfter,pRTBefore,rcNewState.TopLeft());
                    Sleep(10);
                }
                DrawAniStep(rcWnd,rcWnd,pRTAfter,pRTBefore,rcWnd.TopLeft());
                return TRUE;
            }else if(dwFlags&AW_BLEND)
            {
                BYTE byAlpha=0;
                BYTE byStepLen=255/nSteps;
                for(int i=0;i<nSteps;i++)
                {
                    DrawAniStep(rcWnd,pRTAfter,pRTBefore,byAlpha);
                    Sleep(10);
                    byAlpha+=byStepLen;
                }
                DrawAniStep(rcWnd,pRTAfter,pRTBefore,255);
                return TRUE;
            }
            return FALSE;
        }
    }

    BOOL SWindow::FireCommand()
    {
        EventCmd evt(this);
        return FireEvent(evt);
    }

    BOOL SWindow::FireCtxMenu( CPoint pt )
    {
        EventCtxMenu evt(this);
        evt.pt=pt;
        FireEvent(evt);
        return evt.bCancel;
    }

    //////////////////////////////////////////////////////////////////////////
    HRESULT SWindow::OnAttrPos(const SStringW& strValue, BOOL bLoading)
    {
        if (strValue.IsEmpty()) return E_FAIL;
        if(!m_layout.InitPosFromString(strValue)) return E_FAIL;
        if(!bLoading && GetParent())
        {
            GetParent()->UpdateChildrenPosition();
        }
        return S_FALSE;
    }

    HRESULT SWindow::OnAttrOffset(const SStringW& strValue, BOOL bLoading)
    {
        if (strValue.IsEmpty()) return E_FAIL;
        if(!m_layout.InitOffsetFromString(strValue)) return E_FAIL;

        if(!bLoading && GetParent())
        {
            GetParent()->UpdateChildrenPosition();
        }
        return S_FALSE;
    }


    HRESULT SWindow::OnAttrPos2type(const SStringW& strValue, BOOL bLoading)
    {
        if(!m_layout.InitOffsetFromPos2Type(strValue)) return E_FAIL;

        if(!bLoading)
        {
            SWindow *pParent=GetParent();
            SASSERT(pParent);
            pParent->UpdateChildrenPosition();
        }
        return S_FALSE;
    }

    HRESULT SWindow::OnAttrVisible( const SStringW& strValue, BOOL bLoading )
    {
        BOOL bVisible = strValue != L"0";
        if(!bLoading)   SetVisible(bVisible,TRUE);
        else m_bVisible=bVisible;
        return S_FALSE;
    }

    HRESULT SWindow::OnAttrEnable( const SStringW& strValue, BOOL bLoading )
    {
        BOOL bEnable = strValue != L"0";
        if(bLoading)
        {
            if (bEnable)
                ModifyState(0, WndState_Disable);
            else
                ModifyState(WndState_Disable, WndState_Hover);
        }
        else
        {
            EnableWindow(bEnable,TRUE);
        }
        return S_FALSE;
    }

    HRESULT SWindow::OnAttrDisplay( const SStringW& strValue, BOOL bLoading )
    {
        m_bDisplay = strValue != L"0";
        if(!bLoading && !IsVisible(TRUE))
        {
            SWindow *pParent=GetParent();
            SASSERT(pParent);
            pParent->UpdateChildrenPosition();
        }
        return S_FALSE;
    }

    HRESULT SWindow::OnAttrSkin( const SStringW& strValue, BOOL bLoading )
    {
        m_pBgSkin = GETSKIN(strValue);
        if(!bLoading && m_layout.IsFitContent(PD_ALL))
        {
            SWindow *pParent=GetParent();
            SASSERT(pParent);
            pParent->UpdateChildrenPosition();
        }
        return S_FALSE;
    }

    HRESULT SWindow::OnAttrClass( const SStringW& strValue, BOOL bLoading )
    {
        BOOL bGet=GETSTYLE(strValue,m_style);
        if(!bGet) return E_FAIL;
        if(!m_style.m_strSkinName.IsEmpty())
        {
            m_pBgSkin = GETSKIN(m_style.m_strSkinName);
        }
        if(!m_style.m_strNcSkinName.IsEmpty())
        {
            m_pNcSkin = GETSKIN(m_style.m_strNcSkinName);
        }
        if(!bLoading)
        {
            if(m_layout.IsFitContent(PD_ALL))
            {
                SWindow *pParent=GetParent();
                SASSERT(pParent);
                pParent->UpdateChildrenPosition();
            }else
            {
                InvalidateRect(m_rcWindow);
            }
        }
        return S_FALSE;
    }

    HRESULT SWindow::OnAttrTrackMouseEvent( const SStringW& strValue, BOOL bLoading )
    {
        m_style.m_bTrackMouseEvent = strValue==L"0"?0:1;
        if(!bLoading)
        {
            if(m_style.m_bTrackMouseEvent)
                GetContainer()->RegisterTrackMouseEvent(m_swnd);
            else
                GetContainer()->UnregisterTrackMouseEvent(m_swnd);
        }
        return S_FALSE;
    }

    void SWindow::OnSize( UINT nType, CSize size )
    {
        if(IsDrawToCache())
        {
            if(!m_cachedRT)
            {
                GETRENDERFACTORY->CreateRenderTarget(&m_cachedRT,m_rcWindow.Width(),m_rcWindow.Height());
            }else
            {
                m_cachedRT->Resize(m_rcWindow.Size());
            }
            m_cachedRT->SetViewportOrg(-m_rcWindow.TopLeft());

            MarkCacheDirty(true);
        }
    }

    void SWindow::UpdateCacheMode()
    {
        if(IsDrawToCache() && !m_cachedRT)
        {
            GETRENDERFACTORY->CreateRenderTarget(&m_cachedRT,m_rcWindow.Width(),m_rcWindow.Height());
            m_cachedRT->SetViewportOrg(-m_rcWindow.TopLeft());
            MarkCacheDirty(true);
        }
        if(!IsDrawToCache() && m_cachedRT)
        {
            m_cachedRT=NULL;
        }
    }

    HRESULT SWindow::OnAttrCache( const SStringW& strValue, BOOL bLoading )
    {
        m_bCacheDraw = strValue != L"0";

        if(!bLoading)
        {
            UpdateCacheMode();
            InvalidateRect(NULL);
        }
        return S_FALSE;
    }

    HRESULT SWindow::OnAttrAlpha( const SStringW& strValue, BOOL bLoading )
    {
        m_style.m_byAlpha = _wtoi(strValue);
        if(!bLoading)
        {
            UpdateCacheMode();
            InvalidateRect(NULL);
        }
        return bLoading?S_FALSE:S_OK;
    }

    SWindow * SWindow::GetSelectedChildInGroup()
    {
        SWindow *pChild = GetWindow(GSW_FIRSTCHILD);
        if(!pChild || !pChild->IsSiblingsAutoGroupped()) return NULL;
        return pChild->GetSelectedSiblingInGroup();
    }

    void SWindow::_Update()
    {
        SASSERT(!m_style.m_bBkgndBlend);

        if(!m_style.m_bBkgndBlend && m_invalidRegion && !m_invalidRegion->IsEmpty()) 
        {
            if(m_invalidRegion)
            {
                //ˢ�·Ǳ�����ϵĴ���
                CRect rcDirty;
                m_invalidRegion->GetRgnBox(&rcDirty);
                CAutoRefPtr<IRegion> tmpRegin = m_invalidRegion;
                m_invalidRegion = NULL;

                if(IsVisible(TRUE))
                {//�����Ѿ����ɼ��ˡ�
                    IRenderTarget *pRT = GetRenderTarget(rcDirty,OLEDC_OFFSCREEN,TRUE);

                    pRT->PushClipRegion(tmpRegin,RGN_AND);
                    SSendMessage(WM_ERASEBKGND, (WPARAM)pRT);
                    SSendMessage(WM_PAINT, (WPARAM)pRT);
                    PaintForeground(pRT,rcDirty);//��ǰ��
                    pRT->PopClip();

                    ReleaseRenderTarget(pRT);
                }
            }
        }
    }

    const SwndLayout * SWindow::GetLayout() const
    {
        return &m_layout;
    }

    IRenderTarget * SWindow::GetCachedRenderTarget()
    {
        SASSERT(IsDrawToCache());
        if(!m_cachedRT) GETRENDERFACTORY->CreateRenderTarget(&m_cachedRT,0,0);
        return m_cachedRT;
    }

    bool SWindow::IsDrawToCache() const
    {
        return m_bCacheDraw || m_style.m_byAlpha!=0xFF || m_style.m_bLayeredWindow;
    }

}//namespace SOUI
