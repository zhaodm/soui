#include "souistd.h"
#include "control/SSliderBar.h"

namespace SOUI
{

#define TIMERID_NOTIFY1     1
#define TIMERID_DELAY1      2

//////////////////////////////////////////////////////////////////////////
//  SSliderBar
SSliderBar::SSliderBar()
    : m_bDrag(FALSE)
    , m_uHtPrev(-1)
    , m_pSkinThumb(GETBUILTINSKIN(SKIN_SYS_SLIDER_THUMB))
{
    m_evtSet.addEvent(EventSliderPos::EventID);
}

SSliderBar::~SSliderBar()
{
}

int SSliderBar::HitTest(CPoint pt)
{
    CRect rc;

    rc = GetPartRect(SC_THUMB);
    if (rc.PtInRect(pt))
        return SC_THUMB;

    rc = GetPartRect(SC_SELECT);
    if (rc.PtInRect(pt))
        return SC_SELECT;

    rc = GetPartRect(SC_RAIL);
    if (rc.PtInRect(pt))
        return SC_RAIL;

    return -1;
}


CRect SSliderBar::GetPartRect(UINT uSBCode)
{
    SASSERT(m_pSkinThumb);

    CRect rcClient;
    GetClientRect(&rcClient);


    SIZE szThumb = m_pSkinThumb->GetSkinSize();

    int nLength = IsVertical()? rcClient.Height():rcClient.Width();
    int nHei=IsVertical()? rcClient.Width():rcClient.Height();
    int nRailWid=IsVertical()? m_pSkinBg->GetSkinSize().cx:m_pSkinBg->GetSkinSize().cy;
    int nRailLength=nLength-(IsVertical()?szThumb.cy:szThumb.cx);
    if(nRailWid>nHei) nRailWid=nHei;

    CRect rcRet(0,0,nLength,nHei);
    switch (uSBCode)
    {
    case SC_RAIL:
        {
            //  左边要偏移滑块的一半(+1)
            rcRet.left   += szThumb.cx/2;
            //  右边要偏移滑块的一半(-1)
            rcRet.right  -= szThumb.cx/2;
            //  垂直居中
            rcRet.top    += (nHei-nRailWid)/2;
            rcRet.bottom -= (nHei-nRailWid)/2;
        }
        break;

    case SC_SELECT:
        {
            rcRet.left   += szThumb.cx/2;
            rcRet.right  = rcRet.left+(m_nValue-m_nMinValue)*nRailLength/(m_nMaxValue-m_nMinValue);
            //  垂直居中
            rcRet.top    += (nHei-nRailWid)/2;
            rcRet.bottom -= (nHei-nRailWid)/2;
        }
        break;

    case SC_THUMB:
        {
            int nPos=(m_nValue-m_nMinValue)*nRailLength/(m_nMaxValue-m_nMinValue);
            CSize szThumb=m_pSkinThumb->GetSkinSize();
            if(!IsVertical())
            {
                rcRet.left += nPos;
                //rcRet.left-=szThumb.cx/2-szThumb.cx/2;left不需要变化了
                rcRet.right=rcRet.left+szThumb.cx;
                int nMargin=(rcClient.Height()-szThumb.cy)/2;
                if(nMargin>0)
                {
                    rcRet.top+=nMargin;
                    rcRet.bottom=rcRet.top+szThumb.cy;
                }else
                {
                    rcRet.top=0;
                    rcRet.bottom=rcClient.Height();
                }
            }
            else
            {
                rcRet.top += nPos;
                //rcRet.top-=szThumb.cy/2-szThumb.cy/2;
                rcRet.bottom=rcRet.top+szThumb.cy;
                int nMargin=(rcClient.Width()-szThumb.cx)/2;
                if(nMargin>0)
                {
                    rcRet.left+=nMargin;
                    rcRet.right=rcRet.left+szThumb.cx;
                }else
                {
                    rcRet.left=0;
                    rcRet.right=rcClient.Width();
                }
            }
            
        }
        break;
    }

    if(uSBCode!=SC_THUMB && IsVertical())
    {//将横坐标，纵坐标交换
        
        _TSWAP(rcRet.left,rcRet.top);
        _TSWAP(rcRet.right,rcRet.bottom);
    }
    rcRet.OffsetRect(rcClient.TopLeft());
    return rcRet;
}

void SSliderBar::OnPaint(IRenderTarget * pRT)
{
    SASSERT(m_pSkinThumb && m_pSkinBg && m_pSkinPos);

    SPainter painter;

    BeforePaint(pRT, painter);

    CRect rcRail=GetPartRect(SC_RAIL);
    m_pSkinBg->Draw(pRT,rcRail,0);
    if(m_nValue!=m_nMinValue)
    {
        CRect rcSel=GetPartRect(SC_SELECT);
        m_pSkinPos->Draw(pRT,rcSel,0);
    }
    CRect rcThumb = GetPartRect(SC_THUMB);
    int nState=0;//normal
    if(m_bDrag) nState=2;//pushback
    else if(m_uHtPrev==SC_THUMB) nState=1;//hover
    m_pSkinThumb->Draw(pRT, rcThumb, nState);

    AfterPaint(pRT, painter);
}

void SSliderBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();

    if (m_bDrag)
    {
        m_bDrag   = FALSE;
        CRect rcThumb = GetPartRect(SC_THUMB);
        InvalidateRect(rcThumb);
    }
    OnMouseMove(nFlags,point);
}

void SSliderBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    SetCapture();

    UINT uHit = HitTest(point);
    if (uHit == SC_THUMB)
    {
        m_bDrag    = TRUE;
        m_ptDrag   = point;
        m_nDragValue=m_nValue;
        Invalidate();
    }
    else
    {
        if (uHit == SC_SELECT || uHit == SC_RAIL)
        {
            CRect rcRail=GetPartRect(SC_RAIL);
            int nValue=0;
            if(IsVertical())
            {
                nValue= (point.y-rcRail.top)*(m_nMaxValue-m_nMinValue+1)/rcRail.Height();
            }else
            {
                nValue= (point.x-rcRail.left)*(m_nMaxValue-m_nMinValue+1)/rcRail.Width();
            }
            SetValue(nValue);
            NotifyPos(SC_THUMB,m_nValue);
            Invalidate();

            m_bDrag    = TRUE;
            m_ptDrag   = point;
            m_nDragValue=m_nValue;
        }
    }
}

void SSliderBar::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (m_bDrag)
    {
        CRect rcRail=GetPartRect(SC_RAIL);

        int nInterHei=(IsVertical()?rcRail.Height():rcRail.Width());
        int nDragLen=IsVertical()?(point.y-m_ptDrag.y):(point.x-m_ptDrag.x);
        int nSlide=nDragLen*(m_nMaxValue-m_nMinValue+1)/nInterHei;

        int nNewTrackPos=m_nDragValue+nSlide;
        if(nNewTrackPos<m_nMinValue)
        {
            nNewTrackPos=m_nMinValue;
        }
        else if(nNewTrackPos>m_nMaxValue)
        {
            nNewTrackPos=m_nMaxValue;
        }
        if(nNewTrackPos!=m_nValue)
        {
            m_nValue=nNewTrackPos;
            Invalidate();
            NotifyPos(SC_THUMB,m_nValue);
        }
    }
    else
    {
        int uHit = HitTest(point);
        if (uHit != m_uHtPrev && (m_uHtPrev==SC_THUMB || uHit==SC_THUMB))
        {
            m_uHtPrev = uHit;
            CRect rcThumb = GetPartRect(SC_THUMB);
            InvalidateRect(rcThumb);
        }
    }
}

void SSliderBar::OnMouseLeave()
{
    if (!m_bDrag && m_uHtPrev==SC_THUMB)
    {
        CRect rcThumb = GetPartRect(SC_THUMB);
        InvalidateRect(rcThumb);
        Invalidate();
        m_uHtPrev=-1;
    }
}

LRESULT SSliderBar::NotifyPos(UINT uCode, int nPos)
{
    EventSliderPos evt(this);
    evt.nPos = nPos;

    return FireEvent(evt);
}

CSize SSliderBar::GetDesiredSize(LPCRECT pRcContainer)
{
    SASSERT(m_pSkinBg && m_pSkinThumb);
    CSize szRet;
    SIZE sizeBg = m_pSkinBg->GetSkinSize();
    SIZE sizeThumb= m_pSkinThumb->GetSkinSize();
    
    if(IsVertical())
    {
        szRet.cx=max(sizeBg.cx,sizeThumb.cx);
        szRet.cy=100;
    }else
    {
        szRet.cy=max(sizeBg.cy,sizeThumb.cy);
        szRet.cx=100;
    }
    return szRet;
}

}//end of namespace