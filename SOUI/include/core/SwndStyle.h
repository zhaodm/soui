/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SwndStyle.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI���ڷ�����
*/
#pragma once

#include "res.mgr/sstringpool.h"
#include "res.mgr/SSkinPool.h"
#include "SSkin.h"

namespace SOUI
{

class SOUI_EXP SwndStyle : public SObject
{
    SOUI_CLASS_NAME(SwndStyle, L"style")

    enum
    {
        Align_Left               = 0x000UL, // valign = top
        Align_Center             = 0x100UL, // valign = middle
        Align_Right              = 0x200UL, // valign = bottom

        VAlign_Top               = 0x0000UL, // valign = top
        VAlign_Middle            = 0x1000UL, // valign = middle
        VAlign_Bottom            = 0x2000UL, // valign = bottom
    };
public:
    SwndStyle();


    COLORREF m_crBg;                /**<������ɫ */
    COLORREF m_crBorder;            /**<�߿���ɫ */

    int m_nMarginX;                 /**<X����ı߿��С */
    int m_nMarginY;                 /**<Y����ı߿��С */

    SStringT m_strCursor;           /**<���NAME */
    SStringW m_strSkinName;         /**<SKIN NAME */
    SStringW m_strNcSkinName;       /**<�ǿͻ���SKIN NAME */

    BYTE     m_byAlpha;             /**<����͸���� */
    BYTE     m_bySepSpace;          /**<�Ӵ���ˮƽ��� */
    DWORD    m_bDotted:1;           /**<֧��ʡ�Ժ���ʾ�ı� */
    DWORD    m_bTrackMouseEvent:1;  /**<���������뼰�Ƴ���Ϣ */
    DWORD    m_bBkgndBlend:1;       /**<��Ⱦ�������ݺͱ�����ϱ�־ */
    DWORD    m_bLayeredWindow;      /**<ָʾ�Ƿ���һ���ֲ㴰�� */

    UINT GetTextAlign();
    int GetStates();
    COLORREF GetTextColor(int iState);
    IFontPtr GetTextFont(int iState);
    void SetTextColor(int iState,COLORREF cr){m_crText[iState]=cr;}
protected:
    UINT m_nTextAlign;      /**<�ı����� */
    UINT m_uAlign,m_uVAlign;/**<ˮƽ����ֱ���� */
    COLORREF m_crText[4];   /**<����4��״̬�µ���ɫ */
    IFontPtr m_ftText[4];   /**<����4��״̬�µ����� */
    
    SOUI_ATTRS_BEGIN()
        ATTR_STRINGW(L"skin", m_strSkinName, TRUE)
        ATTR_STRINGW(L"ncSkin", m_strNcSkinName, TRUE)
        ATTR_HEX(L"textMode", m_nTextAlign, TRUE)

        ATTR_ENUM_BEGIN(L"align", UINT, TRUE)
            ATTR_ENUM_VALUE(L"left", Align_Left)
            ATTR_ENUM_VALUE(L"center", Align_Center)
            ATTR_ENUM_VALUE(L"right", Align_Right)
        ATTR_ENUM_END(m_uAlign)
        ATTR_ENUM_BEGIN(L"valign", UINT, TRUE)
            ATTR_ENUM_VALUE(L"top", VAlign_Top)
            ATTR_ENUM_VALUE(L"middle", VAlign_Middle)
            ATTR_ENUM_VALUE(L"bottom", VAlign_Bottom)
        ATTR_ENUM_END(m_uVAlign)

        ATTR_COLOR(L"colorBkgnd", m_crBg, TRUE)
        ATTR_COLOR(L"colorBorder", m_crBorder, TRUE)

        ATTR_FONT(L"font", m_ftText[0], TRUE)
        ATTR_FONT(L"fontHover", m_ftText[1], TRUE)
        ATTR_FONT(L"fontPush", m_ftText[2], TRUE)
        ATTR_FONT(L"fontDisable", m_ftText[3], TRUE)

        ATTR_COLOR(L"colorText", m_crText[0], TRUE)
        ATTR_COLOR(L"colorTextHover", m_crText[1], TRUE)
        ATTR_COLOR(L"colorTextPush", m_crText[2], TRUE)
        ATTR_COLOR(L"colorTextDisable", m_crText[3], TRUE)

        ATTR_INT(L"margin-x", m_nMarginX, TRUE)
        ATTR_INT(L"margin-y", m_nMarginY, TRUE)
        ATTR_INT(L"margin", m_nMarginX = m_nMarginY, TRUE) // �����Ƚ�bt������.....�պ��ð�
        ATTR_STRINGT(L"cursor",m_strCursor,FALSE)
        ATTR_INT(L"dotted",m_bDotted,FALSE)
        ATTR_INT(L"trackMouseEvent",m_bTrackMouseEvent,FALSE)
        ATTR_INT(L"alpha",m_byAlpha,TRUE)
        ATTR_INT(L"layeredWindow",m_bLayeredWindow,TRUE)
        ATTR_INT(L"bkgndBlend",m_bBkgndBlend,TRUE)
        ATTR_INT(L"sepSpace",m_bySepSpace,TRUE)
    SOUI_ATTRS_BREAK()      //���Բ�����SObject����
};


}//namespace SOUI