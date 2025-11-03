#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/KeyCodeExclusion.h"

using namespace omega::remote;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(KeyCodeExclusion,keyCodeThatIsEmptyCannotBeAdded)
{
    KeyCodeExclusionSPtr pKeyExclusion = KeyCodeExclusion::instance();

    KeyCode kEmpty;
    pKeyExclusion->add(kEmpty,"Empty Key");
    
    EXPECT_TRUE(pKeyExclusion->isAvailable(kEmpty));
    EXPECT_TRUE(pKeyExclusion->description(kEmpty).isEmpty());

    KeyCodeExclusion::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeExclusion,keyCodeThatIsRemoteCannotBeAdded)
{

    KeyCodeExclusionSPtr pKeyExclusion = KeyCodeExclusion::instance();

    KeyCode kRemote(QString::fromLatin1("Play"));
    pKeyExclusion->add(kRemote,"Remote Key");
    
    EXPECT_TRUE(pKeyExclusion->isAvailable(kRemote));
    EXPECT_TRUE(pKeyExclusion->description(kRemote).isEmpty());

    KeyCodeExclusion::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeExclusion,singletonInstancePreservseExclusion)
{
    KeyCode keyA(static_cast<tuint32>(Qt::Key_A));
    
    {
        KeyCodeExclusionSPtr pKeyExclusion = KeyCodeExclusion::instance();
        pKeyExclusion->add(keyA,"Key name A");        
    }
    
    {
        EXPECT_FALSE(KeyCodeExclusion::instance()->isAvailable(keyA));
        EXPECT_TRUE(KeyCodeExclusion::instance()->description(keyA)=="Key name A");        
    }
    
    KeyCodeExclusion::release();
    
    {
        EXPECT_TRUE(KeyCodeExclusion::instance()->isAvailable(keyA));
        EXPECT_TRUE(KeyCodeExclusion::instance()->description(keyA).isEmpty());
    }    
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeExclusion,mappingIsOverwrittenWhenAddToDuplicate)
{
    KeyCodeExclusionSPtr pKeyExclusion = KeyCodeExclusion::instance();
    
    KeyCode keyA(static_cast<tuint32>(Qt::Key_A));
    pKeyExclusion->add(keyA,"Key name A");

    EXPECT_FALSE(KeyCodeExclusion::instance()->isAvailable(keyA));
    EXPECT_TRUE(KeyCodeExclusion::instance()->description(keyA)=="Key name A");            

    KeyCode keyB(static_cast<tuint32>(Qt::Key_A));
    pKeyExclusion->add(keyB,"Select All");

    EXPECT_FALSE(KeyCodeExclusion::instance()->isAvailable(keyA));
    EXPECT_TRUE(KeyCodeExclusion::instance()->description(keyA)=="Select All");            

    KeyCodeExclusion::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeExclusion,buildExclusionMapAndCheck)
{
    KeyCodeExclusionSPtr pKeyExclusion = KeyCodeExclusion::instance();
    
    KeyCode keyA(static_cast<tuint32>(Qt::Key_A));
    pKeyExclusion->add(keyA,"Key name A");
    KeyCode keyB(static_cast<tuint32>(Qt::Key_Space));
    pKeyExclusion->add(keyB,"Key name space");
    KeyCode keyPref(static_cast<tuint32>(Qt::Key_Comma) | static_cast<tuint32>(Qt::ControlModifier));
    pKeyExclusion->add(keyPref,"Preferences");
    KeyCode keySelectAll(static_cast<tuint32>(Qt::Key_A) | static_cast<tuint32>(Qt::ControlModifier));
    pKeyExclusion->add(keySelectAll,"Select All");
    KeyCode keyCut(static_cast<tuint32>(Qt::Key_X) | static_cast<tuint32>(Qt::ControlModifier));
    pKeyExclusion->add(keyCut,"Cut");
    KeyCode keyCopy(static_cast<tuint32>(Qt::Key_C) | static_cast<tuint32>(Qt::ControlModifier));
    pKeyExclusion->add(keyCopy,"Copy");
    KeyCode keyPaste(static_cast<tuint32>(Qt::Key_V) | static_cast<tuint32>(Qt::ControlModifier));
    pKeyExclusion->add(keyPaste,"Paste");
    KeyCode keyHelp(static_cast<tuint32>(Qt::Key_F1));
    pKeyExclusion->add(keyHelp,"Show Help");
    KeyCode keyDelete(static_cast<tuint32>(Qt::Key_Delete));
    pKeyExclusion->add(keyDelete,"Delete Tracks");

    KeyCode testKeyA(static_cast<tuint32>(Qt::Key_A));
    EXPECT_FALSE(pKeyExclusion->isAvailable(testKeyA));
    EXPECT_TRUE(pKeyExclusion->description(testKeyA)=="Key name A");
    
    KeyCode testKeyB(static_cast<tuint32>(Qt::Key_B));
    EXPECT_TRUE(pKeyExclusion->isAvailable(testKeyB));
    EXPECT_TRUE(pKeyExclusion->description(testKeyB).isEmpty());

    KeyCode testKeyC(static_cast<tuint32>(Qt::Key_C));
    EXPECT_TRUE(pKeyExclusion->isAvailable(testKeyC));
    EXPECT_TRUE(pKeyExclusion->description(testKeyC).isEmpty());

    KeyCode testKeyD(static_cast<tuint32>(Qt::Key_B));
    EXPECT_TRUE(pKeyExclusion->isAvailable(testKeyD));
    EXPECT_TRUE(pKeyExclusion->description(testKeyD).isEmpty());

    KeyCode testKeyE(static_cast<tuint32>(Qt::Key_Space));
    EXPECT_FALSE(pKeyExclusion->isAvailable(testKeyE));
    EXPECT_TRUE(pKeyExclusion->description(testKeyE)=="Key name space");

    KeyCode keyPrefA(static_cast<tuint32>(Qt::Key_Comma) | static_cast<tuint32>(Qt::ControlModifier));
    EXPECT_FALSE(pKeyExclusion->isAvailable(keyPrefA));
    EXPECT_TRUE(pKeyExclusion->description(keyPrefA)=="Preferences");
    
    KeyCode keyPrefB(static_cast<tuint32>(Qt::Key_Comma));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyPrefB));
    EXPECT_TRUE(pKeyExclusion->description(keyPrefB).isEmpty());

    KeyCode keySelectAllA(static_cast<tuint32>(Qt::Key_A) | static_cast<tuint32>(Qt::ControlModifier));
    EXPECT_FALSE(pKeyExclusion->isAvailable(keySelectAllA));
    EXPECT_TRUE(pKeyExclusion->description(keySelectAllA)=="Select All");

    KeyCode keySelectAllB(static_cast<tuint32>(Qt::Key_A) | static_cast<tuint32>(Qt::AltModifier));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keySelectAllB));
    EXPECT_TRUE(pKeyExclusion->description(keySelectAllB).isEmpty());
    KeyCode keySelectAllC(static_cast<tuint32>(Qt::Key_A) | static_cast<tuint32>(Qt::ShiftModifier));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keySelectAllC));
    EXPECT_TRUE(pKeyExclusion->description(keySelectAllC).isEmpty());
    KeyCode keySelectAllD(static_cast<tuint32>(Qt::Key_A) | static_cast<tuint32>(Qt::KeypadModifier));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keySelectAllD));
    EXPECT_TRUE(pKeyExclusion->description(keySelectAllD).isEmpty());

    KeyCode keyCutA(static_cast<tuint32>(Qt::Key_X) | static_cast<tuint32>(Qt::ControlModifier));
    EXPECT_FALSE(pKeyExclusion->isAvailable(keyCutA));
    EXPECT_TRUE(pKeyExclusion->description(keyCutA)=="Cut");

    KeyCode keyCutB(static_cast<tuint32>(Qt::Key_X));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyCutB));
    EXPECT_TRUE(pKeyExclusion->description(keyCutB).isEmpty());
    KeyCode keyCutC(static_cast<tuint32>(Qt::ControlModifier));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyCutC));
    EXPECT_TRUE(pKeyExclusion->description(keyCutC).isEmpty());

    KeyCode keyCopyA(static_cast<tuint32>(Qt::Key_C) | static_cast<tuint32>(Qt::ControlModifier));
    EXPECT_FALSE(pKeyExclusion->isAvailable(keyCopyA));
    EXPECT_TRUE(pKeyExclusion->description(keyCopyA)=="Copy");

    KeyCode keyCopyB(static_cast<tuint32>(Qt::Key_C));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyCopyB));
    EXPECT_TRUE(pKeyExclusion->description(keyCopyB).isEmpty());
    KeyCode keyCopyC(static_cast<tuint32>(Qt::Key_C) | static_cast<tuint32>(Qt::AltModifier));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyCopyC));
    EXPECT_TRUE(pKeyExclusion->description(keyCopyC).isEmpty());
    KeyCode keyCopyD(static_cast<tuint32>(Qt::Key_C) | static_cast<tuint32>(Qt::AltModifier) | static_cast<tuint32>(Qt::ShiftModifier));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyCopyD));
    EXPECT_TRUE(pKeyExclusion->description(keyCopyD).isEmpty());

    KeyCode keyPasteA(static_cast<tuint32>(Qt::Key_V) | static_cast<tuint32>(Qt::ControlModifier));
    EXPECT_FALSE(pKeyExclusion->isAvailable(keyPasteA));
    EXPECT_TRUE(pKeyExclusion->description(keyPasteA)=="Paste");

    KeyCode keyPasteB(static_cast<tuint32>(Qt::Key_V));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyPasteB));
    EXPECT_TRUE(pKeyExclusion->description(keyPasteB).isEmpty());
    KeyCode keyPasteC(static_cast<tuint32>(Qt::Key_V) | static_cast<tuint32>(Qt::ShiftModifier));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyPasteC));
    EXPECT_TRUE(pKeyExclusion->description(keyPasteC).isEmpty());
    KeyCode keyPasteD(static_cast<tuint32>(Qt::Key_V) | static_cast<tuint32>(Qt::ShiftModifier) | static_cast<tuint32>(Qt::ControlModifier));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyPasteD));
    EXPECT_TRUE(pKeyExclusion->description(keyPasteD).isEmpty());

    KeyCode keyHelpA(static_cast<tuint32>(Qt::Key_F1));
    EXPECT_FALSE(pKeyExclusion->isAvailable(keyHelpA));
    EXPECT_TRUE(pKeyExclusion->description(keyHelpA)=="Show Help");

    KeyCode keyHelpB(static_cast<tuint32>(Qt::Key_F2));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyHelpB));
    EXPECT_TRUE(pKeyExclusion->description(keyHelpB).isEmpty());
    KeyCode keyHelpC(static_cast<tuint32>(Qt::Key_F3));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyHelpC));
    EXPECT_TRUE(pKeyExclusion->description(keyHelpC).isEmpty());

    KeyCode keyDeleteA(static_cast<tuint32>(Qt::Key_Delete));
    EXPECT_FALSE(pKeyExclusion->isAvailable(keyDeleteA));
    EXPECT_TRUE(pKeyExclusion->description(keyDeleteA)=="Delete Tracks");

    KeyCode keyDeleteB(static_cast<tuint32>(Qt::Key_PageUp));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyDeleteB));
    EXPECT_TRUE(pKeyExclusion->description(keyDeleteB).isEmpty());
    KeyCode keyDeleteC(static_cast<tuint32>(Qt::Key_PageDown));
    EXPECT_TRUE(pKeyExclusion->isAvailable(keyDeleteC));
    EXPECT_TRUE(pKeyExclusion->description(keyDeleteC).isEmpty());

    KeyCodeExclusion::release();
}

//-------------------------------------------------------------------------------------------
