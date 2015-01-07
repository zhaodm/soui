/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SwndPosition.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI�Ĵ��ڲ���ģ��
*/

#pragma once

namespace SOUI
{

    #define POSFLAG_REFCENTER      '|'        //�ο�����������
    #define POSFLAG_REFPREV_NEAR   '['        //�ο�ǰһ���ֵܴ������Լ����ı�
    #define POSFLAG_REFNEXT_NEAR   ']'        //�ο���һ���ֵܴ������Լ����ı�
    #define POSFLAG_REFPREV_FAR    '{'        //�ο�ǰһ���ֵܴ������Լ�Զ�ı�
    #define POSFLAG_REFNEXT_FAR    '}'        //�ο���һ���ֵܴ������Լ�Զ�ı�
    #define POSFLAG_PERCENT        '%'        //�����ڸ����ڵİٷֱȶ�������
    #define POSFLAG_SIZE           '@'        //��pos�����ж��崰�ڵ�size��ֻ���ڶ���x2,y2ʱ��Ч

    enum SWNDPOSMASK//��������
    {
        // Specify by "width" attribute
        SizeX_Mask          = 0x000fUL,
        SizeX_Specify       = 0x0001UL, // width > 0
        SizeX_FitContent    = 0x0002UL, // width <= 0
        SizeX_FitParent     = 0x0004UL, // width = "full" default

        // Specify by "height" attribute
        SizeY_Mask          = 0x00f0UL,
        SizeY_Specify       = 0x0010UL, // height > 0
        SizeY_FitContent    = 0x0020UL, // height <= 0 default
        SizeY_FitParent     = 0x0040UL, // height = "full" default
    };

    //��������
    enum PIT{
        PIT_NULL=0,        //��Ч����
        PIT_NORMAL,        //ê������
        PIT_CENTER,        //�ο����������ĵ�,��"|"��ʼ
        PIT_PERCENT,       //ָ���ڸ�����������еİٷֱ�
        PIT_PREV_NEAR,     //�ο�ǰһ���ֵܴ������Լ����ı�
        PIT_NEXT_NEAR,     //�ο���һ���ֵܴ������Լ����ı�
        PIT_PREV_FAR,      //�ο�ǰһ���ֵܴ������Լ�Զ�ı�
        PIT_NEXT_FAR,      //�ο���һ���ֵܴ������Լ�Զ�ı�
        PIT_SIZE,          //ָ�����ڵĿ���߸�
    };

    struct POSITION_ITEM
    {
        PIT     pit;
        char    cMinus;     /**<�����ֵ����"-", ����-0����ֱ����nPos��ʾ����Ҫһ�������ı�־λ */
        float   nPos;       /**<����ֵ */
    };
  
    enum POSDIR
    {
        PD_X = 1,
        PD_Y = 2,
        PD_ALL = 3,
    };
    
    enum POSINDEX
    {
        PI_LEFT = 0,
        PI_TOP,
        PI_RIGHT,
        PI_BOTTOM,
    };

    class SOUI_EXP SwndLayout
    {
    public:
        SwndLayout();
        
        void Clear();
        
        BOOL IsEmpty();

        /**
         * InitPosFromString
         * @brief    ����һ��pos�ַ���
         * @param    const SStringW & strPos --  pos�ַ���
         * @return   BOOL --  TRUE:�ɹ���FALSE:ʧ�� 
         *
         * Describe  
         */
        BOOL InitPosFromString(const SStringW & strPos);
        
        /**
         * InitOffsetFromString
         * @brief    ����һ��offset�����ַ���
         * @param    const SStringW & strPos --  offset�����ַ���
         * @return   BOOL --  TRUE:�ɹ���FALSE:ʧ�� 
         * Describe  
         */    
        BOOL InitOffsetFromString(const SStringW & strPos);

         /**
         * InitOffsetFromPos2Type
         * @brief    ��pos2type�����г�ʼ��offset����
         * @param    const SStringW & strPos2Type --  strPos2Type�����ַ���
         * @return   BOOL --  TRUE:�ɹ���FALSE:ʧ�� 
         * Describe  
         */    
        BOOL InitOffsetFromPos2Type(const SStringW & strPos2Type);

        //������size����
        BOOL InitSizeFromString( const SStringW & strSize);

        BOOL InitWidth( const SStringW & strWidth);

        BOOL InitHeight(const SStringW & strHeight);

        BOOL SetFitParent(POSDIR pd = PD_ALL);
        
        BOOL SetFitContent(POSDIR pd = PD_ALL);
        
        BOOL IsFitParent(POSDIR pd = PD_ALL) const;

        /**
         * IsFitContent
         * @brief    ��ò����Ƿ����������ݱ�־
         * @return   BOOL 
         *
         * Describe  
         */
        BOOL IsFitContent(POSDIR pd = PD_ALL) const;
        
        BOOL IsSpecifySize(POSDIR pd = PD_ALL) const;

    protected:
        //���ַ�������������ת����POSITION_ITEM
        BOOL StrPos2ItemPos(const SStringW &strPos,POSITION_ITEM & posItem);

        //������pos�ж����ǰ����λ��
        BOOL ParsePosition12(const SStringW & pos1, const SStringW &pos2);

        //������pos�ж���ĺ�����λ��
        BOOL ParsePosition34(const SStringW & pos3, const SStringW &pos4);
        
        BOOL SetWidth(UINT nWid);

        BOOL SetHeight(UINT nHei);

    public:
        UINT uPositionType;         /**< �������� �μ�SWNDPOSMASK����*/
        
        int  nCount;                /**< ������������ */
        POSITION_ITEM pos[4];       /**< ��pos���Զ����ֵ, nCount >0 ʱ��Ч*/
        UINT  uSpecifyWidth;        /**<ʹ��width���Զ���Ŀ� nCount==0 ʱ��Ч*/
        UINT  uSpecifyHeight;       /**<ʹ��height���Զ���ĸ� nCount==0 ʱ��Ч*/
        float fOffsetX,fOffsetY;    /**< ��������ƫ����, x += fOffsetX * width, y += fOffsetY * height  */
    };
}
