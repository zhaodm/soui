/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SwndLayout.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI�Ĵ��ڲ���ģ��
*/

#pragma once

#include "SwndLayout.h"
#include "Swnd.h"

namespace SOUI
{

    enum
    {
        POS_INIT=0x11000000,    //����ĳ�ʼ��ֵ
        POS_WAIT=0x12000000,    //����ļ����������������ڵĲ���
    };

    class SWindow;
    
    class SWindowRepos
    {
    public:
        explicit SWindowRepos(SWindow *pWnd);
        ~SWindowRepos();
        SWindow * GetWindow(){return m_pWnd;}
    protected:
        SWindow * m_pWnd;
        CRect     m_rcWnd;
    };
    
    class SOUI_EXP SwndLayoutBuilder
    {
    public:
        
        static void InitLayoutState(CRect &rcWindow);

        static BOOL IsWaitingPos(int nPos);

        /**
         * CalcPosition
         * @brief    ���㴰������
         * @param   SWindow *pWnd --  ����������Ĵ���ָ��
         * @param   const CRect & rcContainer --  ����λ��
         * @param   CRect & rcWindow --  ���ھ���
         * @param   const SwndLayout * pSwnLayout -- ���ڲ���
         * @return   int ��Ҫ�ȴ������������(<=4)
         *
         * Describe  ÿ�����ڰ���4�����꣬����һ��������������������ֵܴ��ڵĲ��֣�һ�μ�����ܲ���ȫ���õ�4������
         */
        static int CalcPosition(SWindow *pWnd,const CRect & rcContainer,CRect & rcWindow, const SwndLayout * pSwnLayout=NULL);


        /**
         * CalcChildrenPosition
         * @brief    �����б����Ӵ��ڵ�����
         * @param    SList<SWindow * > * pListChildren --  �Ӵ����б�
         * @param    const CRect & rcContainer --  ��������
         * @return   BOOL TRUE-�ɹ���FALSE-ʧ�ܣ��������ڲ��������γ�����
         *
         * Describe  
         */
        static BOOL CalcChildrenPosition(SList<SWindowRepos*> *pListChildren,const CRect & rcContainer);

    protected:
    
        /**
         * PositionItem2Value
         * @brief    ��һ��position_item����Ϊ��������
         * @param    SWindow *pWindow -- ����������Ĵ���ָ��
         * @param    const POSITION_ITEM & pos --  һ��λ�ö��������
         * @param    int nMin --  �����ڵķ�Χ
         * @param    int nMax --  �����ڵķ�Χ
         * @param    BOOL bX --  ����X����
         * @return   int ����õ�������
         *
         * Describe  
         */
        static int PositionItem2Value(SWindow *pWindow,const POSITION_ITEM &pos,int nMin, int nMax,BOOL bX);


        /**
         * CalcSize
         * @brief    ���㴰�ڴ�С
         * @param    SWindow *pWindow -- ����������Ĵ���ָ��
         * @param    const CRect & rcContainer --  ����λ��
         * @param    const SwndLayout * pSwnLayout -- ���ڲ���
         * @return   CSize 
         *
         * Describe  
         */
        static CSize CalcSize(SWindow *pWindow,const CRect & rcContainer,const SwndLayout * pSwndLayout);
        
        /**
         * GetWindowLayoutRect
         * @brief    ���һ�����ڲ���ռ�õ�λ��
         * @param    SWindow * pWindow --  
         * @return   CRect 
         *
         * Describe  
         */
        static CRect GetWindowLayoutRect(SWindow *pWindow);
    };
}
