#include "track/model/inc/FileNameParser.h"

#include "gtest/gtest.h"

//-------------------------------------------------------------------------------------------

TEST(FileNameParser, parseNamesA)
{
    omega::track::model::FileNameParser parser;
    
    QString nameA = "Chuck Berry - Little Queenie.mp3";
    QStringList listA = parser.parse(nameA);
    EXPECT_EQ(listA.size(), 2);
    EXPECT_EQ(listA.at(0), "Chuck Berry");
    EXPECT_EQ(listA.at(1), "Little Queenie");
    
    QString nameB = "03 Waiting Song.mp3";
    QStringList listB = parser.parse(nameB);
    EXPECT_EQ(listB.size(), 2);
    EXPECT_EQ(listB.at(0), "3");
    EXPECT_EQ(listB.at(1), "Waiting Song");
    
    QString nameC = "(01)-Gustav Holst - Mars, The Bringer of War.mp3";
    QStringList listC = parser.parse(nameC);
    EXPECT_EQ(listC.size(), 4);
    EXPECT_EQ(listC.at(0), "1");
    EXPECT_EQ(listC.at(1), "Gustav Holst");
    EXPECT_EQ(listC.at(2), "Mars");
    EXPECT_EQ(listC.at(3), "The Bringer of War");
        
    QString nameD = "(19) - Exsultate, Jubilate (K165 ) , IV. Allegro. Alleluja.ogg";
    QStringList listD = parser.parse(nameD);
    EXPECT_EQ(listD.size(), 5);
    EXPECT_EQ(listD.at(0), "19");
    EXPECT_EQ(listD.at(1), "Exsultate");
    EXPECT_EQ(listD.at(2), "Jubilate");
    EXPECT_EQ(listD.at(3), "K165");
    EXPECT_EQ(listD.at(4), "IV. Allegro. Alleluja");
    
    QString nameE = "(03) - Vaughan Williams , Fantasia on \'Greensleeves\'.ogg";
    QStringList listE = parser.parse(nameE);
    EXPECT_EQ(listE.size(), 3);
    EXPECT_EQ(listE.at(0), "3");
    EXPECT_EQ(listE.at(1), "Vaughan Williams");
    EXPECT_EQ(listE.at(2), "Fantasia on \'Greensleeves\'");
    
    QString nameF = "Waiting Song.mp3";
    QStringList listF = parser.parse(nameF);
    EXPECT_EQ(listF.size(), 1);
    EXPECT_EQ(listF.at(0), "Waiting Song");
}

//-------------------------------------------------------------------------------------------
