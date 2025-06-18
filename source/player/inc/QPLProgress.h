//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_QPLPROGRESS_H
#define __OMEGA_PLAYER_QPLPROGRESS_H
//-------------------------------------------------------------------------------------------

#include "player/inc/QPLItemBase.h"
#include "common/inc/AbstractProgressInterface.h"

#include <QTimer>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class QPLProgress : public common::AbstractProgressInterface
{
    public:
        Q_OBJECT
        
    public:
        QPLProgress(QPlaylistWidget *parent);
        virtual ~QPLProgress();
        
        virtual bool isActive();
        virtual void activate(bool useReference = false);
        virtual void deactivate(bool useReference = false);
                
        virtual void paint(QPainter *painter);

        virtual void mouseMoveEvent(QMouseEvent *e);
        virtual void mousePressEvent(QMouseEvent *e);
        virtual void mouseReleaseEvent(QMouseEvent *e);
        
        virtual void setProgress(tfloat32 v);
        virtual tfloat32 getProgress();
        virtual bool isCancelled();

    protected:
    
        QPlaylistWidget *m_parentWidget;
        
        QImage *m_leftImage;
        QImage *m_rightImage;
        QImage *m_rightMaskImage;
        QImage *m_rightHoverImage;
        QImage *m_rightClickImage;
        QList<QImage *> m_waitImageList;
        
        tfloat32 m_progressValue;
        int m_waitImageIndex;
        QTimer *m_waitTimer;
        int m_progressState;
        int m_progressReferenceCount;
        bool m_cancelFlag;
        
        virtual bool isOnCancelButton(QMouseEvent *e);
        
        virtual bool isRetina() const;
        virtual void paintRetinaImage(QPainter *painter,QPoint pos,QImage *pImage) const;
        virtual void paintRetinaImage(QPainter *painter,QPointF pos,QImage *pImage) const;
        
    protected slots:
    
        void onWaitTimer();
        
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
