/**
* Copyright (C) 2014-2050 SOUI�Ŷ�
* All rights reserved.
* 
* @file       stabctrl.h
* @brief      
* @version    v1.0      
* @author     soui      
* @date       2014-07-06
* 
* Describe    ��չ�б�� 
*/
#pragma once
#include "core/SWnd.h"

namespace SOUI
{
    /** 
    * @class     STabPage
    * @brief     tab��ǩҳ��
    *
    * Describe   tab��ǩҳ��
    */
    class SOUI_EXP STabPage : public SWindow
    {
        SOUI_CLASS_NAME(STabPage, L"page")

    public:
        /**
        * STabPage::STabPage
        * @brief    ���캯��
        *
        * Describe  ���캯��  
        */
        STabPage():m_iIcon(-1)
        {
            m_bVisible = FALSE;
            m_dwState = WndState_Invisible;
            m_layout.SetFitParent();
        }
        /**
        * STabPage::~STabPage
        * @brief    ��������
        *
        * Describe  ��������  
        */
        virtual ~STabPage()
        {
        }
        /**
        * STabPage::GetTitle
        * @brief    ��ȡ����
        * @return   LPCTSTR --- ����
        *
        * Describe  ��ȡ����
        */
        LPCTSTR GetTitle()
        {
            return m_strTitle;
        }
        /**
        * STabPage::SetTitle
        * @brief    ���ñ���
        * @param    LPCTSTR lpszTitle --- ����
        *
        * Describe  ���ñ��� 
        */
        void SetTitle(LPCTSTR lpszTitle)
        {
            m_strTitle = lpszTitle;
        }
        
        int GetIconIndex() const {return m_iIcon;}
        
        void SetIconIndex(int iIcon) {m_iIcon=iIcon;}
        
        SStringT GetToolTipText(){return m_strToolTipText;}
        
        /**
         * OnUpdateToolTip
         * @brief    ����tooltip
         * @param    const CPoint & pt --  ���Ե�
         * @param [out]  SwndToolTipInfo & tipInfo -- tip��Ϣ 
         * @return   BOOL -- FALSE
         *
         * Describe  ���Ƿ���FALSE����ֹ��pageҳ������ʾtooltip
         */
        virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo &tipInfo){return FALSE;}
        
        SOUI_ATTRS_BEGIN()
            ATTR_I18NSTRT(L"title", m_strTitle, FALSE)
            ATTR_INT(L"iconIndex", m_iIcon,FALSE)
        SOUI_ATTRS_END()
    protected:

        SStringT m_strTitle; /**< ���� */
        int      m_iIcon;
    };

    typedef enum tagSLIDEDIR
    {
        SD_LEFTRIGHT=0,
        SD_RIGHTLEFT,
        SD_TOPBOTTOM,
        SD_BOTTOMTOP,
    } SLIDEDIR;

    /** 
    * @class     STabCtrl
    * @brief     tab�ؼ�
    *
    * Describe   tab�ؼ�
    */
    class SOUI_EXP STabCtrl : public SWindow
    {
        friend class STabSlider;

        SOUI_CLASS_NAME(STabCtrl, L"tabctrl")

    protected:
        int m_nHoverTabItem; /**< hover״̬item */
        int m_nCurrentPage;  /**< ��ǰҳ��      */
        int m_nTabInterSize;   /**< tabҳ����   */
        SIZE m_szTab;        /**< tab SIZE   */
        int m_nTabPos;       /**< tabλ��       */
        ISkinObj *m_pSkinTab; /**< ISkibObj���� */
        ISkinObj *m_pSkinIcon; /**< ISkibObj����  */
        ISkinObj *m_pSkinTabInter;  /**< ISkibObj����  */
        ISkinObj *m_pSkinFrame;     /**< ISkibObj����  */
        CPoint m_ptIcon;   /**< ͼ��λ�� */
        CPoint m_ptText;   /**< ����λ�� */
        int m_nTabAlign;   /**< ���з�ʽ */

        SArray<STabPage*> m_lstPages;  /**< tab��ǩҳ������ */

        enum
        {
            AlignTop,
            AlignLeft,
            AlignBottom,
            AlignRight,
        };

        int    m_nAnimateSteps; /**<  */
    public:
        /**
        * STabCtrl::STabCtrl
        * @brief    ���캯��
        *
        * Describe  ���캯��  
        */
        STabCtrl();

        /**
        * STabCtrl::~STabCtrl
        * @brief    ��������
        *
        * Describe  ��������  
        */
        virtual ~STabCtrl() {}

        /**
        * STabCtrl::GetCurSel
        * @brief    ��ȡ��ǰѡ��
        * @return   ����int
        *
        * Describe  ��ȡ��ǰѡ�� 
        */
        int GetCurSel()
        {
            return m_nCurrentPage;
        }

        SWindow * GetPage(int iPage);
        SWindow * GetPage(LPCTSTR pszTitle);

        /**
        * STabCtrl::SetCurSel
        * @brief    ���õ�ǰѡ��
        * @param    int nIndex -- ����
        * @return   ����BOOL
        *
        * Describe  ��ȡ��ǰѡ�� 
        */
        BOOL SetCurSel(int nIndex);

        /**
        * STabCtrl::SetCurSel
        * @brief    ���õ�ǰѡ��
        * @param    LPCTSTR pszTitle -- ����
        * @return   ����BOOL
        *
        * Describe  ��ȡ��ǰѡ�� 
        */
        BOOL SetCurSel(LPCTSTR pszTitle);

        /**
        * STabCtrl::SetItemTitle
        * @brief    ���ñ���
        * @param    int nIndex  -- ����
        * @param    LPCTSTR lpszTitle  -- ����
        * @return   ����BOOL
        *
        * Describe  ��ȡ��ǰѡ�� 
        */
        BOOL SetItemTitle(int nIndex, LPCTSTR lpszTitle);
        /**
        * STabCtrl::CreateChildren
        * @brief    ����tabҳ��
        * @param    pugi::xml_node xmlNode  -- xml�ļ�
        * @return   ����BOOL
        *
        * Describe  ����tabҳ��
        */
        BOOL CreateChildren(pugi::xml_node xmlNode);

        /**
        * STabCtrl::InsertItem
        * @brief    ����tabҳ��
        * @param    LPCWSTR lpContent  -- XML������page��Ϣ
        * @param    int iInsert  -- λ��
        * @return   ���ز���λ��
        *
        * Describe  ����tabҳ��
        */
        int InsertItem(LPCWSTR lpContent,int iInsert=-1);

        /**
        * STabCtrl::InsertItem
        * @brief    ����tabҳ��
        * @param    pugi::xml_node xmlNode  -- xml�ļ�
        * @param    int iInsert  -- λ��
        * @param    BOOL bLoading -- �Ƿ����
        * @return   ����int
        *
        * Describe  ����tabҳ��
        */
        int InsertItem(pugi::xml_node xmlNode,int iInsert=-1,BOOL bLoading=FALSE);

        /**
        * STabCtrl::GetItemCount
        * @brief    ��ȡtabҳ����
        * @return   ����int
        *
        * Describe  ��ȡtabҳ����
        */
        size_t GetItemCount()
        {
            return m_lstPages.GetCount();
        }
        /**
        * STabCtrl::GetItem
        * @brief    ��ȡָ��tabҳ��
        * @param    int nIndex -- ����
        * @return   ����int
        *
        * Describe  ��ȡ��ǰѡ�� 
        */
        STabPage* GetItem(int nIndex);

        /**
        * STabCtrl::RemoveItem
        * @brief    ɾ��ָ��tabҳ��
        * @param    int nIndex -- ����
        * @param    int nSelPage -- ѡ��ҳ��
        * @return   ɾ��ָ��tabҳ��
        *
        * Describe  ��ȡ��ǰѡ�� 
        */
        BOOL RemoveItem(int nIndex, int nSelPage=0);

        /**
        * STabCtrl::RemoveAllItems
        * @brief    ɾ������ҳ��
        *
        * Describe  ɾ������ҳ�� 
        */
        void RemoveAllItems(void);
        
    protected:
        /**
        * BeforePaint
        * @brief    ΪRT׼���õ�ǰ���ڵĻ�ͼ����
        * @param    IRenderTarget * pRT --  
        * @param    SPainter & painter --  
        * @return   void 
        *
        * Describe  ��SWindow��ͬ��STabCtrl��ǿ��ʹ��normal״̬�������弰��ɫ������״̬��tabͷʹ��
        */
        virtual void BeforePaint(IRenderTarget *pRT, SPainter &painter);

        /**
        * STabCtrl::GetChildrenLayoutRect
        * @brief    
        *
        * Describe  
        */
        virtual CRect GetChildrenLayoutRect();
        
        /**
         * GetTitleRect
         * @brief    ��ȡtabͷ�ľ���
         * @return   CRect 
         *
         * Describe  
         */
        virtual CRect GetTitleRect();
        
        /**
        * STabCtrl::GetItemRect
        * @brief    ��ȡָ��itemλ��
        * @param    int nIndex -- ����
        * @param     CRect &rcItem -- λ��
        *
        * Describe  ��ȡָ��itemλ�� 
        */
        virtual BOOL GetItemRect(int nIndex, CRect &rcItem);
        
        /**
        * STabCtrl::DrawItem
        * @brief    ����item
        * @param    IRenderTarget *pRT -- �����豸
        * @param    const CRect &rcItem -- ��������
        * @param    int iItem  -- ����
        * @param    DWORD dwState  -- ״̬
        *
        * Describe  ����item
        */
        virtual void DrawItem(IRenderTarget *pRT,const CRect &rcItem,int iItem,DWORD dwState);

        /**
        * STabCtrl::OnGetDlgCode
        * @brief    ��ȡ������Ϣ��
        * @return   ����UINT
        *
        * Describe  ��ȡ������Ϣ��
        */
        virtual UINT OnGetDlgCode()
        {
            return SC_WANTARROWS;
        }

        virtual BOOL OnUpdateToolTip(CPoint pt, SwndToolTipInfo & tipInfo);
        
        /**
        * UpdateChildrenPosition
        * @brief    �����Ӵ���λ��
        * @return   void 
        *
        * Describe  
        */
        virtual void UpdateChildrenPosition();

        virtual void OnInitFinished(pugi::xml_node xmlNode);
    protected:
        int HitTest(CPoint pt);
        
    protected:
        /**
        * STabCtrl::OnPaint
        * @brief    �滭��Ϣ
        * @param    IRenderTarget *pRT -- �����豸���
        *
        * Describe  �˺�������Ϣ��Ӧ����
        */
        void OnPaint(IRenderTarget *pRT);
        /**
        * STabCtrl::OnLButtonDown
        * @brief    �����������¼�
        * @param    UINT nFlags -- ��־
        * @param    CPoint point -- �������
        *
        * Describe  �˺�������Ϣ��Ӧ����
        */
        void OnLButtonDown(UINT nFlags, CPoint point);
        /**
        * STabCtrl::OnMouseMove
        * @brief    ����ƶ��¼�
        * @param    UINT nFlags -- ��־
        * @param    CPoint point -- �������
        *
        * Describe  �˺�������Ϣ��Ӧ����
        */
        void OnMouseMove(UINT nFlags, CPoint point);
        /**
        * STabCtrl::OnMouseLeave
        * @brief    ����뿪�¼�
        *
        * Describe  �˺�������Ϣ��Ӧ����
        */
        void OnMouseLeave()
        {
            OnMouseMove(0,CPoint(-1,-1));
        }
        /**
        * STabCtrl::OnKeyDown
        * @brief    ���̰���
        * @param    UINT nChar -- ����
        * @param    UINT nRepCnt -- �ظ�����
        * @param    UINT nFlags -- ��־
        *
        * Describe  �˺�������Ϣ��Ӧ����
        */
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        /**
        * STabCtrl::OnDestroy
        * @brief    ����
        *
        * Describe  �˺�������Ϣ��Ӧ����
        */
        void OnDestroy();

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_PAINT_EX(OnPaint)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_MOUSEMOVE(OnMouseMove)
            MSG_WM_MOUSELEAVE(OnMouseLeave)
            MSG_WM_KEYDOWN(OnKeyDown)
        SOUI_MSG_MAP_END()

        SOUI_ATTRS_BEGIN()
            ATTR_INT(L"curSel", m_nCurrentPage, FALSE)
            ATTR_SIZE(L"tabSize",m_szTab,TRUE)
            ATTR_INT(L"tabWidth", m_szTab.cx, FALSE)
            ATTR_INT(L"tabHeight", m_szTab.cy, FALSE)
            ATTR_INT(L"tabPos", m_nTabPos, FALSE)
            ATTR_INT(L"tabInterSize", m_nTabInterSize, FALSE)
            ATTR_SKIN(L"tabInterSkin", m_pSkinTabInter, FALSE)
            ATTR_SKIN(L"tabSkin", m_pSkinTab, FALSE)
            ATTR_SKIN(L"iconSkin", m_pSkinIcon, FALSE)
            ATTR_SKIN(L"frameSkin", m_pSkinFrame, FALSE)
            ATTR_INT(L"icon-x", m_ptIcon.x, FALSE)
            ATTR_INT(L"icon-y", m_ptIcon.y, FALSE)
            ATTR_INT(L"text-x", m_ptText.x, FALSE)
            ATTR_INT(L"text-y", m_ptText.y, FALSE)
            ATTR_ENUM_BEGIN(L"tabAlign", int, TRUE)
                ATTR_ENUM_VALUE(L"top", AlignTop)
                ATTR_ENUM_VALUE(L"left", AlignLeft)
                ATTR_ENUM_VALUE(L"right", AlignRight)
                ATTR_ENUM_VALUE(L"bottom", AlignBottom)
            ATTR_ENUM_END(m_nTabAlign)
            ATTR_INT(L"animateSteps",m_nAnimateSteps,FALSE)
        SOUI_ATTRS_END()
    };

}//namespace SOUI