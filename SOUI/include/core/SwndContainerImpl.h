/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       SwndContainerImpl.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/02
* 
* Describe    SOUI����������ʵ��
*/

#pragma  once

#include "SDropTargetDispatcher.h"
#include "FocusManager.h"

namespace SOUI
{

    class SOUI_EXP SwndContainerImpl : public ISwndContainer
    {
    public:
        SwndContainerImpl(SWindow *pRoot);
        
        IDropTarget * GetDropTarget(){return &m_dropTarget;}

        CFocusManager * GetFocusManager() {return &m_focusMgr;}
    protected:
        //ISwndContainer
        virtual BOOL RegisterDragDrop(SWND swnd,IDropTarget *pDropTarget);

        virtual BOOL RevokeDragDrop(SWND swnd);

        virtual LRESULT DoFrameEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

        virtual BOOL OnReleaseSwndCapture();

        virtual SWND OnSetSwndCapture(SWND swnd);
        virtual void OnSetSwndFocus(SWND swnd);

        virtual SWND OnGetSwndCapture();

        virtual SWND SwndGetFocus();

        virtual SWND SwndGetHover();

        virtual IAcceleratorMgr* GetAcceleratorMgr(){return &m_focusMgr;}

        virtual BOOL RegisterTimelineHandler(ITimelineHandler *pHandler);

        virtual BOOL UnregisterTimelineHandler(ITimelineHandler *pHandler);

        virtual BOOL RegisterTrackMouseEvent(SWND swnd);

        virtual BOOL UnregisterTrackMouseEvent(SWND swnd);

        //��Ǵ�������zorderʧЧ
        virtual void MarkWndTreeZorderDirty();

        //�ؽ���������zorder
        virtual void BuildWndTreeZorder();

    public://ITimelineHandler
        virtual void OnNextFrame();
    protected:

        void OnFrameMouseMove(UINT uFlag,CPoint pt);

        void OnFrameMouseLeave();

        BOOL OnFrameSetCursor(const CPoint &pt);

        void OnFrameMouseEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

        void OnFrameMouseWheel(UINT uMsg,WPARAM wParam,LPARAM lParam);

        void OnFrameKeyEvent(UINT uMsg,WPARAM wParam,LPARAM lParam);

        void OnFrameKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

        void OnActivate(UINT nState);

        void OnActivateApp(BOOL bActive, DWORD dwThreadID);

        void _BuildWndTreeZorder(SWindow *pWnd,UINT &iOrder);
    protected:
        SWND m_hCapture;
        SWND m_hHover;
        BOOL    m_bNcHover;

        CFocusManager m_focusMgr;

        SDropTargetDispatcher m_dropTarget;

        SWindow    *m_pRoot;
        BOOL        m_bZorderDirty;

        SList<ITimelineHandler*>    m_lstTimelineHandler;
        SList<SWND>                 m_lstTrackMouseEvtWnd;
    };

}//namespace SOUI

