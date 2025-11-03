//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_QPLITEMSINGLE_H
#define __OMEGA_PLAYER_QPLITEMSINGLE_H
//-------------------------------------------------------------------------------------------

#include "player/inc/QPLItemBase.h"
#include "player/inc/QPlaylistWidget.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class QPLItemSingle : public QPLItemBase
{
    public:
        Q_OBJECT
    
    public:
        QPLItemSingle(QPlaylistWidget *playListW,QSharedPointer<track::info::Info>& info);
        QPLItemSingle(QPlaylistWidget *playListW,QSharedPointer<track::info::Info>& info,tint subTIndex);
        virtual ~QPLItemSingle();
        
        virtual PlaylistItemType type() const;
        
        virtual tint height() const;
        virtual void paint(tint yTop,QPainter *painter);
        
    protected:
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

