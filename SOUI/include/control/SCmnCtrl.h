/**
 * Copyright (C) 2014-2050 SOUI�Ŷ�
 * All rights reserved.
 * 
 * @file       SCmnCtrl.h
 * @brief      ͨ�ÿؼ�
 * @version    v1.0      
 * @author     soui      
 * @date       2014-05-28
 * 
 * Describe    ���ļ��ж����˺ܶ�ͨ�ÿؼ�:��̬�ı��������ӣ���ť����ѡ��ť��
 */

#pragma once
#include "core/SWnd.h"
#include "core/Accelerator.h"

namespace SOUI
{

/**
 * @class      SStatic
 * @brief      ��̬�ı��ؼ���
 * 
 * Describe    ��̬�ı��ؼ���֧�ֶ��У��ж�������ʱ��\n����ǿ�ƻ���
 * Usage       <text>inner text example</text>
 */
class SOUI_EXP SStatic : public SWindow
{
    SOUI_CLASS_NAME(SStatic, L"text")
public:
    /**
     * SStatic::SStatic
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SStatic():m_bMultiLines(0),m_nLineInter(5)
    {
        m_bMsgTransparent=TRUE;
        m_style.SetAttribute(L"align",L"left");
    }
    /**
     * SStatic::SDrawText
     * @brief    �����ı�
     * @param    IRenderTarget *pRT -- �����豸���         
     * @param    LPCTSTR pszBuf -- �ı������ַ���         
     * @param    int cchText -- �ַ�������         
     * @param    LPRECT pRect -- ָ����νṹRECT��ָ��         
     * @param    UINT uFormat --  ���ĵĻ���ѡ��         
     *
     * Describe  ��DrawText��װ
     */    
    virtual void DrawText(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat);

protected:
    int m_bMultiLines;  /**< �Ƿ���������ʾ */  
    int m_nLineInter;   /**< �м�� */

    SOUI_ATTRS_BEGIN()
        ATTR_INT(L"multiLines", m_bMultiLines, FALSE)
        ATTR_INT(L"interHeight", m_nLineInter, FALSE)
    SOUI_ATTRS_END()
};

/**
 * @class      SLink
 * @brief      �����ӿؼ���
 * 
 * Describe    Only For Header Drag Test
 * Usage       <link>inner text example</link>
 */
class SOUI_EXP SLink : public SWindow
{
    SOUI_CLASS_NAME(SLink, L"link")

public:
    /**
     * SLink::SLink
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SLink()
    {
        m_style.SetAttribute(L"align",L"left");
    }

protected:
    /**
     * SLink::OnAttributeFinish
     * @brief    ����xml��������
     *
     * Describe  ����xml�ļ������������
     */
    virtual void OnInitFinished(pugi::xml_node xmlNode);
    /**
     * SLink::SDrawText
     * @brief    �����ı�
     * @param    IRenderTarget *pRT -- �����豸���         
     * @param    LPCTSTR pszBuf -- �ı������ַ���         
     * @param    int cchText -- �ַ�������         
     * @param    LPRECT pRect -- ָ����νṹRECT��ָ��         
     * @param    UINT uFormat --  ���ĵĻ���ѡ��         
     *
     * Describe  ��DrawText��װ
     */
    virtual void DrawText(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat);

    /**
     * SLink::OnSetCursor
     * @brief    ���ù����ʽ��λ��
     * @param    CPoint &pt -- ���ù��λ��
     * @return   ����ֵBOOL �ɹ�--TRUE ʧ��--FALSE
     *
     * Describe  �����ڲ�����ع����ʽ
     */
    virtual BOOL OnSetCursor(const CPoint &pt);

    void OnLButtonDown(UINT nFlags,CPoint pt);
    void OnLButtonUp(UINT nFlags,CPoint pt);
    void OnMouseMove(UINT nFlags,CPoint pt);
    void OnMouseHover(WPARAM wParam, CPoint ptPos);

    SOUI_ATTRS_BEGIN()
        ATTR_STRINGT(L"href", m_strLinkUrl, FALSE)
    SOUI_ATTRS_END()
    
    SOUI_MSG_MAP_BEGIN()
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSEHOVER(OnMouseHover)
    SOUI_MSG_MAP_END()

protected:
    CRect m_rcText;  /**< �ı���ʾ����λ�� */
    SStringT m_strLinkUrl;  /**< ����URL */
};

/**
 * @class      SButton
 * @brief      ��ť�ؼ���
 * 
 * Describe    ͨ������ID��click�¼� Use id attribute to process click event
 * Usage       <button id=xx>inner text example</button>
 */
class SOUI_EXP SButton : public SWindow
    , public IAcceleratorTarget
    , public ITimelineHandler
{
    SOUI_CLASS_NAME(SButton, L"button")
public:
    /**
     * SButton::SButton
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SButton();
protected:
    /**
     * SButton::NeedRedrawWhenStateChange
     * @brief    ״̬�仯��Ҫ�ػ�
     * @return   ����ֵBOOL �ɹ�--TRUE ʧ��--FALSE
     *
     * Describe  ����ť״̬�����仯ʱ����Ҫ���»��� Ĭ�Ϸ���TRUE
     */
    virtual BOOL NeedRedrawWhenStateChange()
    {
        return TRUE;
    }
    /**
     * SButton::OnGetDlgCode
     * @brief    ��ñ���
     *
     * Describe  ���غ궨��SC_WANTCHARS������ҪWM_CHAR��Ϣ
     */
    virtual UINT OnGetDlgCode()
    {
        return SC_WANTCHARS;
    }

    /**
     * SButton::OnAcceleratorPressed
     * @brief    ���ټ�����
     * @param    CAccelerator& accelerator -- ���ټ���ؽṹ��
     * @return   ����ֵBOOL �ɹ�--TRUE ʧ��--FALSE
     *
     * Describe  ������ټ���Ӧ��Ϣ
     */
    virtual bool OnAcceleratorPressed(const CAccelerator& accelerator);
protected:
    /**
     * SButton::GetDesiredSize
     * @brief    ��������Ĵ�Сֵ
     * @param    LPRECT pRcContainer -- ���ݴ������
     *
     * Describe  �������ݴ�����δ�С��������ʺϵĴ�С
     */
    virtual CSize GetDesiredSize(LPCRECT pRcContainer);
    
    /**
     * SButton::OnStateChanged
     * @brief    ״̬�ı䴦����
     * @param    DWORD dwOldState -- ��״̬
     * @param    DWORD dwNewState -- ��״̬
     *
     * Describe  ״̬�ı䴦����
     */
    virtual void OnStateChanged(DWORD dwOldState,DWORD dwNewState);
    
    void OnPaint(IRenderTarget *pRT);

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

    void OnDestroy();

    void OnSize(UINT nType, CSize size);

    BOOL OnEraseBkgnd(IRenderTarget * pRT){return TRUE;}

    HRESULT OnAttrAccel(SStringW strAccel,BOOL bLoading);

protected:
    virtual void OnNextFrame();
    
    /**
     * SLink::StopCurAnimate
     * @brief    ֹͣ����
     *
     * Describe  ֹͣ����
     */
    void StopCurAnimate();

    DWORD  m_accel;
    BOOL   m_bAnimate;    /**< ������־ */
    BYTE   m_byAlphaAni;  /**< ����״̬ */
public:
    SOUI_ATTRS_BEGIN()
        ATTR_CUSTOM(L"accel",OnAttrAccel)
        ATTR_INT(L"animate", m_bAnimate, FALSE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_ERASEBKGND_EX(OnEraseBkgnd)
        MSG_WM_LBUTTONDBLCLK(OnLButtonDown) //��˫����Ϣ����Ϊ����
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_KEYUP(OnKeyUp)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SIZE(OnSize)
    SOUI_MSG_MAP_END()
};

/**
 * @class      SImageButton
 * @brief      ͼƬ��ť��
 * 
 * Describe    ͼƬ��ť�࣬�̳�SButton
 */
class SOUI_EXP SImageButton : public SButton
{
    SOUI_CLASS_NAME(SImageButton, L"imgbtn")
public:
    SImageButton()
    {
        m_bFocusable=FALSE;
    }
};

/**
 * @class      SImageWnd
 * @brief      ͼƬ�ؼ���
 * 
 * Describe    Image Control ͼƬ�ؼ���
 * Usage       Usage: <img skin="skin" sub="0"/>
 */
class SOUI_EXP SImageWnd : public SWindow
{
    SOUI_CLASS_NAME(SImageWnd, L"img")
public:
    /**
     * SImageWnd::SImageWnd
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SImageWnd();
    
    /**
     * SImageWnd::~SImageWnd
     * @brief    ��������
     *
     * Describe  ��������
     */
    virtual ~SImageWnd();

    void OnPaint(IRenderTarget *pRT);
    /**
     * SImageWnd::SetSkin
     * @param    ISkinObj *pSkin -- ��Դ
     * @param    int iFrame -- Ƥ���е�֡��
     * @param    BOOL bAutoFree -- ��ԴID
     * @brief    ����Ƥ��
     * @return   ����ֵBOOL �ɹ�--TRUE ʧ��--FALSE
     *
     * Describe  ����Ƥ��
     */
    BOOL SetSkin(ISkinObj *pSkin,int iFrame=0,BOOL bAutoFree=TRUE);
    /**
     * SImageWnd::SetIcon
     * @param    int nSubID -- ��ԴID
     * @brief    ����ͼ��
     * @return   ����ֵBOOL �ɹ�--TRUE ʧ��--FALSE
     *
     * Describe  ����ͼ��
     */
    BOOL SetIcon(int nSubID);

    /**
     * SImageWnd::GetSkin
     * @brief    ��ȡ��Դ
     * @return   ����ֵISkinObjָ��
     *
     * Describe  ��ȡ��Դ
     */
    ISkinObj * GetSkin(){return m_pSkin;}
protected:
    /**
     * SImageWnd::GetDesiredSize
     * @brief    ��ȡԤ�ڴ�С
     * @param    LPRECT pRcContainer  --  ���ݾ��ο� 
     * @return   ����ֵ CSize���� 
     *
     * Describe  ���ݾ��εĴ�С����ȡԤ�ڴ�С(�����е㲻��)
     */
    virtual CSize GetDesiredSize(LPCRECT pRcContainer);

    BOOL m_bManaged;  /**< �Ƿ�Ҫ�Զ��ͷŵ�ǰ��m_pSkin���� */
    int m_iFrame;  /**< ��ԴͼƬID */
    ISkinObj *m_pSkin;  /**< ISkinObj���� */

    SOUI_ATTRS_BEGIN()
        ATTR_SKIN(L"skin", m_pSkin, TRUE)
        ATTR_INT(L"iconIndex", m_iFrame, FALSE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
    SOUI_MSG_MAP_END()
};

/**
 * @class      SAnimateImgWnd
 * @brief      ����ͼƬ����
 * 
 * Describe    �˴���֧�ֶ���Ч��
 */
class SOUI_EXP SAnimateImgWnd : public SWindow, public ITimelineHandler
{
    SOUI_CLASS_NAME(SAnimateImgWnd, L"animateimg")
public:    
    /**
     * SAnimateImgWnd::SAnimateImgWnd
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SAnimateImgWnd();

    /**
     * SAnimateImgWnd::~SAnimateImgWnd
     * @brief    ��������
     *
     * Describe  ��������
     */    
    virtual ~SAnimateImgWnd() {}

    /**
     * SAnimateImgWnd::Start
     * @brief    ��������
     *
     * Describe  ��������
     */  
    void Start();
    /**
     * SAnimateImgWnd::Stop
     * @brief    ֹͣ����
     *
     * Describe  ֹͣ����
     */  
    void Stop();

    /**
     * SAnimateImgWnd::IsPlaying
     * @brief    �ж϶�������״̬
     * @return   ����ֵ�Ƕ���״̬ TRUE -- ������
     *
     * Describe  �ж϶�������״̬
     */  
    BOOL IsPlaying(){return m_bPlaying;}
protected:
    /**
     * SAnimateImgWnd::GetDesiredSize
     * @brief    ��ȡԤ�ڴ�С
     * @param    LPRECT pRcContainer  --  ���ݾ��ο� 
     * @return   ����ֵ CSize���� 
     *
     * Describe  ���ݾ��εĴ�С����ȡԤ�ڴ�С(�����е㲻��)
     */
    virtual CSize GetDesiredSize(LPCRECT pRcContainer);
    virtual void OnNextFrame();

    void OnPaint(IRenderTarget *pRT);

    void OnShowWindow(BOOL bShow, UINT nStatus);
    void OnDestroy();

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_SHOWWINDOW(OnShowWindow)
    SOUI_MSG_MAP_END()

    SOUI_ATTRS_BEGIN()
        ATTR_SKIN(L"skin", m_pSkin, TRUE)
        ATTR_UINT(L"speed", m_nSpeed, FALSE)
        ATTR_UINT(L"autoStart", m_bAutoStart, FALSE)
    SOUI_ATTRS_END()

protected:
    ISkinObj *m_pSkin;        /**< ��ʱ���� */
    int           m_nSpeed;       /**< �ٶ� */
    int           m_iCurFrame;    /**< ��ǰ֡ */
    BOOL          m_bAutoStart;   /**< �Ƿ��Զ����� */
    BOOL          m_bPlaying;     /**< �Ƿ������� */
};

/**
 * @class      SProgress
 * @brief      ��������
 * 
 * Describe    ��������
 * Usage: <progress bgskin=xx posskin=xx min=0 max=100 value=10,showpercent=0/>
 */
class SOUI_EXP SProgress : public SWindow
{
    SOUI_CLASS_NAME(SProgress, L"progress")
public:
    /**
     * SProgress::SProgress
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SProgress();

    /**
     * SProgress::SetValue
     * @brief    ���ý���������ֵ
     * @param    int nValue  --  ����ֵ 
     * @return   ����ֵ�� TRUE -- ���óɹ�
     *
     * Describe  ���ý���������ֵ
     */  
    BOOL SetValue(int nValue);
    /**
     * SProgress::GetValue
     * @brief    ��ȡ����ֵ
     * @return   ����ֵ��int 
     *
     * Describe  ��ȡ����ֵ
     */  
    int GetValue(){return m_nValue;}

    /**
     * SProgress::SetRange
     * @param    int nMin  --  ������Сֵ 
     * @param    int nMax  --  �������ֵ      
     * @brief    ���ý���ֵ��С��ֵ
     *
     * Describe  ���ý���ֵ
     */  
    void SetRange(int nMin,int nMax);
    /**
     * SProgress::GetRange
     * @param    int nMin  --  ������Сֵ 
     * @param    int nMax  --  �������ֵ      
     * @brief    ��ȡ����ֵ��С��ֵ
     *
     * Describe  ��ȡ����ֵ
     */  
    void GetRange(int *pMin,int *pMax);
    /**
     * SProgress::IsVertical
     * @brief    �жϽ������Ƿ�Ϊ��ֱ״̬
     * @return   ����ֵ�� TRUE -- ��ֱ״̬
     *
     * Describe  ��ȡ����ֵ
     */  
    BOOL IsVertical(){return m_bVertical;}
protected:
    /**
     * SProgress::GetDesiredSize
     * @brief    ��ȡԤ�ڴ�С
     * @param    LPRECT pRcContainer  --  ���ݾ��ο� 
     * @return   ����ֵ CSize���� 
     *
     * Describe  ���ݾ��εĴ�С����ȡԤ�ڴ�С(�����е㲻��)
     */
    virtual CSize GetDesiredSize(LPCRECT pRcContainer);

    void OnPaint(IRenderTarget *pRT);

    int m_nMinValue; /**< ������Сֵ */
    int m_nMaxValue; /**< �������ֵ */
    int m_nValue;    /**< ����ֵ */

    BOOL m_bShowPercent; /**< �Ƿ���ʾ�ٷֱ� */
    BOOL m_bVertical;    /**< �Ƿ���ֱ״̬ */

    ISkinObj *m_pSkinBg;   /**< ������Դ */
    ISkinObj *m_pSkinPos;  /**< ǰ����Դ */

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
    SOUI_MSG_MAP_END()

    SOUI_ATTRS_BEGIN()
        ATTR_SKIN(L"bkgndSkin", m_pSkinBg, TRUE)
        ATTR_SKIN(L"posSkin", m_pSkinPos, TRUE)
        ATTR_INT(L"min", m_nMinValue, FALSE)
        ATTR_INT(L"max", m_nMaxValue, FALSE)
        ATTR_INT(L"value", m_nValue, FALSE)
        ATTR_UINT(L"vertical", m_bVertical, FALSE)
        ATTR_UINT(L"showPercent", m_bShowPercent, FALSE)
    SOUI_ATTRS_END()
};

/**
 * @class      SLine
 * @brief      �����ؼ�
 * 
 * Describe    �����ؼ�
 * Usage: <hr size=1 mode="vert" lineStyle="dash"/>
 */
class SOUI_EXP SLine : public SWindow
{
    SOUI_CLASS_NAME(SLine, L"hr")

public:
    /**
     * SLine::SLine
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SLine();

    void OnPaint(IRenderTarget *pRT);
protected:
    int m_nLineStyle;
    int m_nLineSize;
    
    enum HRMODE{
        HR_HORZ=0,
        HR_VERT,
        HR_TILT,
    }m_mode;

    SOUI_ATTRS_BEGIN()
        ATTR_INT(L"size", m_nLineSize, FALSE)
        ATTR_ENUM_BEGIN(L"mode", HRMODE, FALSE)
            ATTR_ENUM_VALUE(L"vertical", HR_VERT)
            ATTR_ENUM_VALUE(L"horizontal", HR_HORZ)
            ATTR_ENUM_VALUE(L"tilt", HR_TILT)
        ATTR_ENUM_END(m_mode)
        ATTR_ENUM_BEGIN(L"lineStyle", int, FALSE)
            ATTR_ENUM_VALUE(L"solid", PS_SOLID)             // default
            ATTR_ENUM_VALUE(L"dash", PS_DASH)               /* -------  */
            ATTR_ENUM_VALUE(L"dot", PS_DOT)                 /* .......  */
            ATTR_ENUM_VALUE(L"dashdot", PS_DASHDOT)         /* _._._._  */
            ATTR_ENUM_VALUE(L"dashdotdot", PS_DASHDOTDOT)   /* _.._.._  */
        ATTR_ENUM_END(m_nLineStyle)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
    SOUI_MSG_MAP_END()
};

/**
 * @class      SCheckBox
 * @brief      ��ѡ��ؼ�
 * 
 * Describe    ��ѡ��ؼ�
 * Usage: <check checked="1">This is a check-box</check>
 */
class SOUI_EXP SCheckBox : public SWindow
{
    SOUI_CLASS_NAME(SCheckBox, L"check")

    enum
    {
        CheckBoxSpacing = 4,
    };

public:
    /**
     * SCheckBox::SCheckBox
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SCheckBox();

protected:

    ISkinObj *m_pSkin;   /**< ״̬ͼƬ��Դ */
    ISkinObj *m_pFocusSkin; /**< ����״̬��Դ */
    /**
     * SCheckBox::_GetDrawState
     * @brief    ��ø�ѡ��״̬
     * @return   ����״ֵ̬
     *
     * Describe  ��ȡ��ѡ��״̬
     */
    UINT _GetDrawState();
    /**
     * SCheckBox::GetCheckRect
     * @brief    ��ø�ѡ�����
     * @return   ����ֵCRect���ο�
     *
     * Describe  ��ȡ��ѡ�����
     */
    CRect GetCheckRect();
    /**
     * SCheckBox::GetDesiredSize
     * @brief    ��ȡԤ�ڴ�С
     * @param    LPRECT pRcContainer  --  ���ݾ��ο� 
     * @return   ����ֵ CSize���� 
     *
     * Describe  ���ݾ��εĴ�С����ȡԤ�ڴ�С(�����е㲻��)
     */
    virtual CSize GetDesiredSize(LPCRECT pRcContainer);
    /**
     * SCheckBox::GetTextRect
     * @brief    ��ȡ�ı���С
     * @param    LPRECT pRect  --  ���ݾ��ο� 
     *
     * Describe  ���þ��εĴ�С
     */
    virtual void GetTextRect(LPRECT pRect);
    /**
     * SCheckBox::NeedRedrawWhenStateChange
     * @brief    �ж�״̬�ı��Ƿ���Ҫ�ػ�
     * @return   ����ֵ BOOL 
     *
     * Describe  ״̬�ı��Ƿ���Ҫ�ػ�
     */
    virtual BOOL NeedRedrawWhenStateChange()
    {
        return TRUE;
    }

    /**
     * SCheckBox::OnGetDlgCode
     * @brief    ���ض�Ӧ��Ϣ��
     * @return   ����ֵ UINT 
     *
     * Describe  ���ض�Ӧ��Ϣ��
     */
    virtual UINT OnGetDlgCode()
    {
        return SC_WANTCHARS;
    }
    /**
     * SCheckBox::DrawFocus
     * @brief    ���ƻ�ȡ����
     * @param    IRenderTarget *pRT  --  �豸���
     *
     * Describe  ���ض�Ӧ��Ϣ��
     */
    virtual void DrawFocus(IRenderTarget *pRT);

    void OnLButtonUp(UINT nFlags, CPoint point);

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    
    void OnPaint(IRenderTarget *pRT);

    HRESULT OnAttrCheck(const SStringW& strValue, BOOL bLoading);
    
    SOUI_ATTRS_BEGIN()
        ATTR_SKIN(L"skin", m_pSkin, FALSE)
        ATTR_SKIN(L"focusSkin", m_pFocusSkin, FALSE)
        ATTR_CUSTOM(L"checked",OnAttrCheck)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_LBUTTONUP(OnLButtonUp)
        MSG_WM_KEYDOWN(OnKeyDown)
    SOUI_MSG_MAP_END()
};

/**
 * @class      SIconWnd
 * @brief      ͼ��ؼ�
 * 
 * Describe    ͼ��ؼ�
 * Usage: <icon src="icon:16" />
 */
class SOUI_EXP SIconWnd : public SWindow
{
    SOUI_CLASS_NAME(SIconWnd, L"icon")
public:    
    /**
     * SIconWnd::SIconWnd
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SIconWnd();
    virtual ~SIconWnd();

    void OnPaint(IRenderTarget *pRT);
    /**
     * SIconWnd::AttachIcon
     * @brief    ����ͼ����Դ
     * @param    HICON hIcon -- ͼ����Դ���
     * @return   ����ֵ HICON 
     *
     * Describe  ����ͼ����Դ
     */
    void SetIcon(HICON hIcon);
protected:
    virtual CSize GetDesiredSize(LPCRECT pRcContainer);

    HICON m_theIcon; /**< ͼ����Դ��� */

    SOUI_ATTRS_BEGIN()
        ATTR_ICON(L"src", m_theIcon, FALSE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
    SOUI_MSG_MAP_END()
};


/**
 * @class      SRadioBox
 * @brief      ��ѡ��ؼ�
 * 
 * Describe    ��ѡ��ؼ�
 * Usage: <radio checked="1">This is a check-box</radio>
 */
class SOUI_EXP SRadioBox : public SWindow
{
    SOUI_CLASS_NAME(SRadioBox, L"radio")

    enum
    {
        RadioBoxSpacing = 4,
    };

public:
    /**
     * SRadioBox::SRadioBox
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SRadioBox();


protected:

    ISkinObj *m_pSkin;  /**< Ƥ����Դ */
    ISkinObj *m_pFocusSkin; /**< ����Ƥ����Դ */
    /**
     * SRadioBox::SRadioBox
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    UINT _GetDrawState(); 
    /**
     * SRadioBox::SRadioBox
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    CRect GetRadioRect();
    
    /**
     * SRadioBox::GetTextRect
     * @brief    ����ı���С
     * @param    LPRECT pRect -- �ı���СRect
     *
     * Describe  ���캯��
     */
    virtual void GetTextRect(LPRECT pRect);
    /**
     * SRadioBox::GetDesiredSize
     * @brief    ��ȡԤ�ڴ�Сֵ
     * @param    LPRECT pRcContainer -- ���ݴ���Rect
     *
     * Describe  ��ȡԤ�ڴ�Сֵ
     */
    virtual CSize GetDesiredSize(LPCRECT pRcContainer);
    /**
     * SRadioBox::NeedRedrawWhenStateChange
     * @brief    ��״̬�ı�ʱ���Ƿ���Ҫ�ػ�
     * @return   ����BOOL
     *
     * Describe  ��״̬�ı�ʱ���Ƿ���Ҫ�ػ�
     */
    virtual BOOL NeedRedrawWhenStateChange();
    /**
     * SRadioBox::DrawFocus
     * @brief    ���ƽ�����ʽ
     * @param    IRenderTarget *pRT -- �����豸
     *
     * Describe  ����ý���ʱ����Ҫ����
     */
    virtual void DrawFocus(IRenderTarget *pRT);
    /**
     * SRadioBox::OnGetDlgCode
     * @brief    ��ȡ��Ϣ����
     * @return   ���ر���ֵ
     *
     * Describe  ��ȡ��Ϣ����
     */
    virtual UINT OnGetDlgCode()
    {
        return 0;
    }
    /**
     * SRadioBox::IsSiblingsAutoGroupped
     * @brief    �Ƿ��Զ���ӵ�ͬһ��
     * @return   ����BOOL 
     *
     * Describe  ��ͬ���Ƶĵ�ѡ��ť�Ƿ��Զ���ӵ�ͬһ����
     */
    virtual BOOL IsSiblingsAutoGroupped() {return TRUE;}
    
    
    /**
     * SRadioBox::GetSelectedSiblingInGroup
     * @brief    ��ȡһ��group����ѡ��״̬���ֵ�
     * @return   ����ѡ��״̬���ֵܴ���ָ��
     *
     * Describe  û��ѡ��״̬�ֵ�ʱ����NULL
     */
    virtual SWindow * GetSelectedSiblingInGroup();
    
    
    /**
     * OnStateChanging
     * @brief    ״̬�����仯
     * @param    DWORD dwOldState --  ԭ״̬
     * @param    DWORD dwNewState --  ��״̬
     * @return   void 
     *
     * Describe  
     */
    virtual void OnStateChanging(DWORD dwOldState,DWORD dwNewState);
protected:
    void OnSetFocus();
    
    void OnPaint(IRenderTarget *pRT);

    HRESULT OnAttrCheck(const SStringW& strValue, BOOL bLoading);

    SOUI_ATTRS_BEGIN()
        ATTR_SKIN(L"skin", m_pSkin, FALSE)
        ATTR_SKIN(L"focusSkin", m_pFocusSkin, FALSE)
        ATTR_CUSTOM(L"checked",OnAttrCheck)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_SETFOCUS_EX(OnSetFocus)
    SOUI_MSG_MAP_END()
};

/**
 * @class      SToggle
 * @brief      Toggle�ؼ�
 * 
 * Describe    Toggle�ؼ�
 */
class SOUI_EXP SToggle : public SWindow
{
    SOUI_CLASS_NAME(SToggle, L"toggle")
public:
    
    /**
     * SToggle::SToggle
     * @brief    ���캯��
     *
     * Describe  ���캯��
     */
    SToggle();
    /**
     * SToggle::SetToggle
     * @brief    ����Toggle����
     * @param    BOOL bToggle -- �Ƿ�����Toggle��Ч         
     * @param    BOOL bUpdate -- �Ƿ���� Ĭ��ֵTRUE
     *
     * Describe  ����Toggle����
     */
    void SetToggle(BOOL bToggle,BOOL bUpdate=TRUE);
    /**
     * SToggle::GetToggle
     * @brief    ��ȡToggle����
     * @return   ����ֵBOOL        
     *
     * Describe  ��ȡToggle���� ��Ҫ�ǻ�ȡ�Ƿ�Toggle
     */
    BOOL GetToggle();
protected:
    void OnPaint(IRenderTarget *pRT);
    void OnLButtonUp(UINT nFlags,CPoint pt);
    virtual CSize GetDesiredSize(LPCRECT pRcContainer);
    virtual BOOL NeedRedrawWhenStateChange(){return TRUE;}

    SOUI_ATTRS_BEGIN()
        ATTR_INT(L"toggled", m_bToggled, TRUE)
        ATTR_SKIN(L"skin", m_pSkin, TRUE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
        MSG_WM_LBUTTONUP(OnLButtonUp)
    SOUI_MSG_MAP_END()
protected:
    BOOL m_bToggled;
    ISkinObj *m_pSkin;
};

/**
 * @class      SGroup
 * @brief      ��ؼ�
 * 
 * Describe    ��ؼ�
 * Usage       <group colorLine1="#b8d5e2" colorLine2="#999999">group text</>
 */
class SOUI_EXP SGroup : public SWindow
{
    SOUI_CLASS_NAME(SGroup, L"group")
public:
    SGroup();

protected:
    virtual CRect GetChildrenLayoutRect();

    void OnPaint(IRenderTarget *pRT);
    
    COLORREF m_crLine1,m_crLine2; /**< group 3D��ʾʹ�õ�������ɫ */
    int         m_nRound; /**< Բ�ǰ뾶 */
    int         m_nHeaderHeight; /**< ͷ���߶� */
public:
    SOUI_ATTRS_BEGIN()
        ATTR_COLOR(L"colorLine1", m_crLine1, FALSE)
        ATTR_COLOR(L"colorLine2", m_crLine2, FALSE)
        ATTR_INT(L"round",m_nRound,FALSE)
        ATTR_INT(L"headerHeight",m_nHeaderHeight,TRUE)
    SOUI_ATTRS_END()

    SOUI_MSG_MAP_BEGIN()
        MSG_WM_PAINT_EX(OnPaint)
    SOUI_MSG_MAP_END()
};

}//namespace SOUI
