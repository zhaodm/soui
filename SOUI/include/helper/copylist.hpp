#pragma  once

namespace SOUI
{
    /**
     * CopyList
     * @brief    �б�Copy
     * @param    SList<T> & sour --  ԴList
     * @param    SList<T> & dest --  Ŀ��List
     * @return   void
     * Describe  
     */    
    template<class T>
    void CopyList(SList<T> &sour,SList<T> &dest)
    {
        SASSERT(dest.IsEmpty());
        SPOSITION pos=sour.GetHeadPosition();
        while(pos)
        {
            T &t=sour.GetNext(pos);
            dest.AddTail(t);
        }
    }

}