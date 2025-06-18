//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_QPLITEMATRACK_H
#define __OMEGA_PLAYER_QPLITEMATRACK_H
//-------------------------------------------------------------------------------------------

#include "player/inc/QPLItemAlbum.h"
#include "player/inc/QPlaylistWidget.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class QPLItemATrack : public QPLItemBase
{
    public:
        Q_OBJECT

    public:
        QPLItemATrack(QPlaylistWidget *playListW,QPLItemAlbum *parent,QSharedPointer<track::info::Info>& info);
        QPLItemATrack(QPlaylistWidget *playListW,QPLItemAlbum *parent,tint childIdx,QSharedPointer<track::info::Info>& info);
        QPLItemATrack(QPlaylistWidget *playListW,QPLItemAlbum *parent,QSharedPointer<track::info::Info>& info,tint subTIndex);
        QPLItemATrack(QPlaylistWidget *playListW,QPLItemAlbum *parent,tint childIdx,QSharedPointer<track::info::Info>& info,tint subTIndex);
        virtual ~QPLItemATrack();

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

