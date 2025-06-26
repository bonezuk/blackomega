//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_TEST_IMAGEREPOSITARYMOCK_H
#define __OMEGA_TRACK_MODEL_TEST_IMAGEREPOSITARYMOCK_H
//-------------------------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "track/model/inc/ImageRepositary.h"

using namespace omega::track::model;

//-------------------------------------------------------------------------------------------

class ImageRepositaryMock : public omega::track::model::ImageRepositary
{
    public:
        MOCK_METHOD3(getImage,QImage *(int imageID,int iWidth,int iHeight));
        MOCK_METHOD2(getReference,QImage *(int iWidth,int iHeight));
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

