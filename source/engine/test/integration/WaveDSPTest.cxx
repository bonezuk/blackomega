#include "gtest/gtest.h"

#include "common/inc/DiskOps.h"
#include "common/inc/BIOBufferedStream.h"
#include "engine/inc/Codec.h"
#include "engine/blueomega/inc/WaveEngine.h"
#include "engine/inc/FormatTypeFromFloat.h"
#include "engine/inc/BiQuadFilter.h"
#include "engine/inc/RemezFIR.h"
#include "engine/inc/FIRFilter.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

void intToMemory(tuint32 v, tubyte *mem)
{
    mem[0] = static_cast<tubyte>(v & 0x000000ff);
    mem[1] = static_cast<tubyte>((v >> 8) & 0x000000ff);
    mem[2] = static_cast<tubyte>((v >> 16) & 0x000000ff);
    mem[3] = static_cast<tubyte>((v >> 24) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

void intToMemory(tint32 v, tubyte *mem)
{
    intToMemory(static_cast<tuint32>(v), mem);
}

//-------------------------------------------------------------------------------------------

void shortToMemory(tuint16 v, tubyte *mem)
{
    mem[0] = static_cast<tubyte>(v & 0x00ff);
    mem[1] = static_cast<tubyte>((v >> 8) & 0x00ff);
}

//-------------------------------------------------------------------------------------------

void shortToMemory(tint16 v, tubyte *mem)
{
    shortToMemory(static_cast<tuint16>(v), mem);
}

//-------------------------------------------------------------------------------------------

void populateHeaderFromCodec(engine::Codec *codec, tubyte hdr[44])
{
    intToMemory(RIFF_ID, &hdr[0]); // Chunk ID
    intToMemory(0, &hdr[4]); // Chunk size
    intToMemory(WAVE_ID, &hdr[8]); // Wave format
    
    intToMemory(FMT_ID, &hdr[12]); // fmt ID
    intToMemory(16, &hdr[16]);
    
    shortToMemory(static_cast<tuint16>(engine::blueomega::WaveInformation::e_formatPCM), &hdr[20]); // audio format
    shortToMemory(static_cast<tuint16>(codec->noChannels()), &hdr[22]); // number of channels
    intToMemory(codec->frequency(), &hdr[24]); // sample rate
    intToMemory(codec->frequency() * codec->noChannels() * 4, &hdr[28]); // bytes per second
    shortToMemory(static_cast<tuint16>(codec->noChannels() * 4), &hdr[32]); // block align
    shortToMemory(static_cast<tuint16>(32), &hdr[34]); // bits per sample
    
    intToMemory(DATA_ID, &hdr[36]);
    intToMemory(0, &hdr[40]);
}

//-------------------------------------------------------------------------------------------

bool saveWaveHeaderFromCodec(engine::Codec *codec, common::BIOStream *out)
{
    tubyte hdr[44];
    populateHeaderFromCodec(codec, hdr);
    return (out->write(hdr, 44) == 44);
}

//-------------------------------------------------------------------------------------------

bool saveWaveHeaderSize(engine::Codec *codec, int totalDataSize, common::BIOStream *out)
{
    bool res = false;
    tubyte hdr[44];
    
    populateHeaderFromCodec(codec, hdr);
    intToMemory(totalDataSize + 36, &hdr[4]);
    intToMemory(totalDataSize, &hdr[40]);
    if(out->seek(0, common::e_Seek_Start) && out->write(hdr, 44) == 44)
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

TEST(WaveDSPTest, lowPass400HzBiQuad)
{
    QString inFilename = "/Users/bonez/Development/Temp/kiss_rose_44.1_24bit.wav";
    QString outFilename = "/Users/bonez/Development/Temp/lp_kiss_rose_1.wav";
    
    //QString inFilename = "D:\\Music\\Temp\\kiss_rose_44.1_24bit.wav";
    //QString outFilename = "D:\\Music\\Temp\\lp_kiss_rose_1.wav";

    ASSERT_TRUE(common::DiskOps::exist(inFilename));
    
    engine::Codec *codec = engine::Codec::get(inFilename);
    ASSERT_TRUE(codec != NULL);
    ASSERT_TRUE(codec->init());
    
    if(common::DiskOps::exist(outFilename))
    {
        common::DiskOps::remove(outFilename);
    }
    
    common::BIOBufferedStream *out = new common::BIOBufferedStream(common::e_BIOStream_FileCreate | common::e_BIOStream_FileWrite);
    ASSERT_TRUE(out != NULL);
    ASSERT_TRUE(out->open(outFilename));
    
    ASSERT_TRUE(saveWaveHeaderFromCodec(codec, out));
    
    int noChannels = codec->noChannels();
    
    engine::BiQuadFilter *lpFilter = new engine::BiQuadFilter [noChannels];
    for(int fIdx = 0; fIdx < noChannels; fIdx++)
    {
        lpFilter[fIdx] = engine::BiQuadFilter::filter(engine::BiQuadFilter::e_LowPass_FirstOrder, 200.0, static_cast<tfloat64>(codec->frequency()));
        ASSERT_FALSE(lpFilter[fIdx].type() == engine::BiQuadFilter::e_UnknownFilter);
    }
    
    int totalDataSize = 0;
    engine::RData data(2048, codec->noChannels(), codec->noChannels());
    while(codec->next(data))
    {
        for(int partIdx = 0; partIdx < data.noParts(); partIdx++)
        {
            sample_t *x = data.partData(partIdx);
            for(int idx = 0; idx < data.part(partIdx).length(); idx++)
            {
                int chIdx;
                sample_t y[8];
                tubyte yOut[8 * 4];
                
                for(chIdx = 0; chIdx < noChannels; chIdx++)
                {
                    y[chIdx] = lpFilter[chIdx].process(x[(idx * noChannels) + chIdx]);
                }
                
                for(chIdx = 0; chIdx < noChannels; chIdx++)
                {
                    engine::write32BitsLittleEndianFromSample(y[chIdx], reinterpret_cast<tchar *>(&yOut[chIdx << 2]));
                }
                ASSERT_EQ(out->write(yOut, noChannels * 4), noChannels * 4);
                totalDataSize += noChannels * 4;
            }
        }
        data.reset();
    }
    
    ASSERT_TRUE(saveWaveHeaderSize(codec, totalDataSize, out));

    delete [] lpFilter;

    out->close();
    delete out;

    codec->close();
    delete codec;
}

//-------------------------------------------------------------------------------------------

TEST(WaveDSPTest, lowPass200HzLowPassFIR)
{
    double lpCoef[] = {
         0.011493756055707984,  0.000419514657231557,  0.00042511474704677246,  0.0004287708898402567,  // 4
         0.0004313506085726602,  0.0004316048251819769,  0.0004310292805484582,  0.0004280591828505792,  // 8
         0.000423956252261581,  0.0004172847704645939,  0.0004095417251442245,  0.00039914166781919105,  // 12
         0.0003875749422861093,  0.00037320443243875074,  0.00035764626774729063,  0.00033912100233492843,  // 16
         0.0003193829204316577,  0.0002965339229748938,  0.00027253789524572363,  0.00024529510968648567,  // 20
         0.00021701755982560304,  0.00018536181768974,  0.00015296783193058272,  0.00011711606432653892,  // 24
         8.09956099759371e-05,  4.1437586123190953e-05,  2.6628093662497723e-06,  -3.9647529708701295e-05,  // 28
         -7.988053496697216e-05,  -0.00012437455576883304,  -0.00016384543062125082,  -0.00021526048608988958,  // 32
         -0.0002871439671436366,  -0.0003212610748945226,  -0.00037924106325626817,  -0.0004316343781019311,  // 36
         -0.0004893036300629168,  -0.0005447600803544823,  -0.0006040610140799099,  -0.0006620645762067646,  // 40
         -0.000722761407139236,  -0.0007823751110035738,  -0.0008441950092087913,  -0.0009050430195767554,  // 44
         -0.0009676070627625766,  -0.00102915132679422,  -0.0010920453150631666,  -0.0011537767532945455,  // 48
         -0.0012164990690338876,  -0.0012778767997117459,  -0.0013399413761508443,  -0.0014003897007514394,  // 52
         -0.0014611389928127169,  -0.0015198478160387068,  -0.0015784593129836092,  -0.0016344833732507868,  // 56
         -0.0016899568918665889,  -0.001742342180603472,  -0.0017942536603418313,  -0.0018427585089210664,  // 60
         -0.0018912492159515331,  -0.0019362937372402983,  -0.001983471949143951,  -0.0020224712820652965,  // 64
         -0.002055596343515114,  -0.002094119480134138,  -0.002124021361613045,  -0.0021527696713530458,  // 68
         -0.0021768357694822854,  -0.0021983706897839446,  -0.002215055651707507,  -0.0022283906646699547,  // 72
         -0.002237057916734304,  -0.0022419747544301888,  -0.002241977786741861,  -0.0022378050497036546,  // 76
         -0.0022285569571144324,  -0.0022148118002939653,  -0.0021957904693434924,  -0.002172033753363249,  // 80
         -0.0021428217479876974,  -0.0021086029462176,  -0.002068637490961064,  -0.002023362768000719,  // 84
         -0.0019720661448380003,  -0.0019152377618792125,  -0.0018521549418922122,  -0.0017835203621959113,  // 88
         -0.0017086933198136665,  -0.0016284908741378975,  -0.0015419564648128446,  -0.0014502031597686052,  // 92
         -0.0013517547984078944,  -0.0012475478215791126,  -0.0011343729195178727,  -0.00101832657473346,  // 96
         -0.0008968209053470643,  -0.0007661383304860812,  -0.0006317218309118385,  -0.0004903445779739781,  // 100
         -0.00034345542121111487,  -0.0001899475727535199,  -3.12693978503117e-05,  0.00013357377185474093,  // 104
         0.00030375136950374775,  0.0004799214882409309,  0.00066120031335063,  0.0008481391916685287,  // 108
         0.0010399607565748851,  0.0012371004416278912,  0.001438846037314817,  0.0016456537323067792,  // 112
         0.0018568623808538922,  0.0020728593835711235,  0.0022929322918728693,  0.002517403976624513,  // 116
         0.0027455178211333187,  0.002977526980589608,  0.0032125861669459513,  0.003451044642604683,  // 120
         0.003692171133124984,  0.003936388415531123,  0.0041828431819327305,  0.004432244394815688,  // 124
         0.004683488502611578,  0.004936697224657421,  0.005189333378529627,  0.005445427387947005,  // 128
         0.005702294452782571,  0.005958427016590396,  0.00621602889388872,  0.006473582993375189,  // 132
         0.006730764523797591,  0.006987213138377185,  0.007243090152968217,  0.00749787683778526,  // 136
         0.007751244752631636,  0.008002806110325774,  0.008252327059319439,  0.008499457275410963,  // 140
         0.008743950656599,  0.008985565522971313,  0.009224044003704327,  0.00945908493553305,  // 144
         0.009690360657655247,  0.00991756336641019,  0.010140367688382307,  0.010358473652763947,  // 148
         0.010571575284319502,  0.010779456915464657,  0.010981940941023503,  0.011178794814014282,  // 152
         0.011369759685097675,  0.011554587167032006,  0.011733042856098275,  0.011904535009473907,  // 156
         0.012068835254250178,  0.01222610435892239,  0.012377295092120932,  0.012519573687821202,  // 160
         0.012654181321593964,  0.012782189442010369,  0.012901113400347702,  0.013011905619073665,  // 164
         0.013114562022930694,  0.013209043884045563,  0.013294661212804206,  0.013371565155388359,  // 168
         0.013439510980882774,  0.013498654692054622,  0.013548728923577672,  0.013589942846066173,  // 172
         0.013622039949768216,  0.013645127351431833,  0.013658924936003286,  0.013663594367800541,  // 176
         0.013658924936003286,  0.013645127351431833,  0.013622039949768216,  0.013589942846066173,  // 180
         0.013548728923577672,  0.013498654692054622,  0.013439510980882774,  0.013371565155388359,  // 184
         0.013294661212804206,  0.013209043884045563,  0.013114562022930694,  0.013011905619073665,  // 188
         0.012901113400347702,  0.012782189442010369,  0.012654181321593964,  0.012519573687821202,  // 192
         0.012377295092120932,  0.01222610435892239,  0.012068835254250178,  0.011904535009473907,  // 196
         0.011733042856098275,  0.011554587167032006,  0.011369759685097675,  0.011178794814014282,  // 200
         0.010981940941023503,  0.010779456915464657,  0.010571575284319502,  0.010358473652763947,  // 204
         0.010140367688382307,  0.00991756336641019,  0.009690360657655247,  0.00945908493553305,  // 208
         0.009224044003704327,  0.008985565522971313,  0.008743950656599,  0.008499457275410963,  // 212
         0.008252327059319439,  0.008002806110325774,  0.007751244752631636,  0.00749787683778526,  // 216
         0.007243090152968217,  0.006987213138377185,  0.006730764523797591,  0.006473582993375189,  // 220
         0.00621602889388872,  0.005958427016590396,  0.005702294452782571,  0.005445427387947005,  // 224
         0.005189333378529627,  0.004936697224657421,  0.004683488502611578,  0.004432244394815688,  // 228
         0.0041828431819327305,  0.003936388415531123,  0.003692171133124984,  0.003451044642604683,  // 232
         0.0032125861669459513,  0.002977526980589608,  0.0027455178211333187,  0.002517403976624513,  // 236
         0.0022929322918728693,  0.0020728593835711235,  0.0018568623808538922,  0.0016456537323067792,  // 240
         0.001438846037314817,  0.0012371004416278912,  0.0010399607565748851,  0.0008481391916685287,  // 244
         0.00066120031335063,  0.0004799214882409309,  0.00030375136950374775,  0.00013357377185474093,  // 248
         -3.12693978503117e-05,  -0.0001899475727535199,  -0.00034345542121111487,  -0.0004903445779739781,  // 252
         -0.0006317218309118385,  -0.0007661383304860812,  -0.0008968209053470643,  -0.00101832657473346,  // 256
         -0.0011343729195178727,  -0.0012475478215791126,  -0.0013517547984078944,  -0.0014502031597686052,  // 260
         -0.0015419564648128446,  -0.0016284908741378975,  -0.0017086933198136665,  -0.0017835203621959113,  // 264
         -0.0018521549418922122,  -0.0019152377618792125,  -0.0019720661448380003,  -0.002023362768000719,  // 268
         -0.002068637490961064,  -0.0021086029462176,  -0.0021428217479876974,  -0.002172033753363249,  // 272
         -0.0021957904693434924,  -0.0022148118002939653,  -0.0022285569571144324,  -0.0022378050497036546,  // 276
         -0.002241977786741861,  -0.0022419747544301888,  -0.002237057916734304,  -0.0022283906646699547,  // 280
         -0.002215055651707507,  -0.0021983706897839446,  -0.0021768357694822854,  -0.0021527696713530458,  // 284
         -0.002124021361613045,  -0.002094119480134138,  -0.002055596343515114,  -0.0020224712820652965,  // 288
         -0.001983471949143951,  -0.0019362937372402983,  -0.0018912492159515331,  -0.0018427585089210664,  // 292
         -0.0017942536603418313,  -0.001742342180603472,  -0.0016899568918665889,  -0.0016344833732507868,  // 296
         -0.0015784593129836092,  -0.0015198478160387068,  -0.0014611389928127169,  -0.0014003897007514394,  // 300
         -0.0013399413761508443,  -0.0012778767997117459,  -0.0012164990690338876,  -0.0011537767532945455,  // 304
         -0.0010920453150631666,  -0.00102915132679422,  -0.0009676070627625766,  -0.0009050430195767554,  // 308
         -0.0008441950092087913,  -0.0007823751110035738,  -0.000722761407139236,  -0.0006620645762067646,  // 312
         -0.0006040610140799099,  -0.0005447600803544823,  -0.0004893036300629168,  -0.0004316343781019311,  // 316
         -0.00037924106325626817,  -0.0003212610748945226,  -0.0002871439671436366,  -0.00021526048608988958,  // 320
         -0.00016384543062125082,  -0.00012437455576883304,  -7.988053496697216e-05,  -3.9647529708701295e-05,  // 324
         2.6628093662497723e-06,  4.1437586123190953e-05,  8.09956099759371e-05,  0.00011711606432653892,  // 328
         0.00015296783193058272,  0.00018536181768974,  0.00021701755982560304,  0.00024529510968648567,  // 332
         0.00027253789524572363,  0.0002965339229748938,  0.0003193829204316577,  0.00033912100233492843,  // 336
         0.00035764626774729063,  0.00037320443243875074,  0.0003875749422861093,  0.00039914166781919105,  // 340
         0.0004095417251442245,  0.0004172847704645939,  0.000423956252261581,  0.0004280591828505792,  // 344
         0.0004310292805484582,  0.0004316048251819769,  0.0004313506085726602,  0.0004287708898402567,  // 348
         0.00042511474704677246,  0.000419514657231557,  0.011493756055707984,  // 351
    };


    QString inFilename = "/Users/bonez/Development/Temp/kiss_rose_44.1_24bit.wav";
    QString outFilename = "/Users/bonez/Development/Temp/lp_kiss_rose_1.wav";
    
    //QString inFilename = "D:\\Music\\Temp\\kiss_rose_44.1_24bit.wav";
    //QString outFilename = "D:\\Music\\Temp\\lp_kiss_rose_1.wav";

    ASSERT_TRUE(common::DiskOps::exist(inFilename));
    
    engine::Codec *codec = engine::Codec::get(inFilename);
    ASSERT_TRUE(codec != NULL);
    ASSERT_TRUE(codec->init());
    
    if(common::DiskOps::exist(outFilename))
    {
        common::DiskOps::remove(outFilename);
    }
    
    common::BIOBufferedStream *out = new common::BIOBufferedStream(common::e_BIOStream_FileCreate | common::e_BIOStream_FileWrite);
    ASSERT_TRUE(out != NULL);
    ASSERT_TRUE(out->open(outFilename));
    
    ASSERT_TRUE(saveWaveHeaderFromCodec(codec, out));
    
    int noChannels = codec->noChannels();
    
    engine::FIRFilter filterL(lpCoef, 351);
    engine::FIRFilter filterR(lpCoef, 351);

    int totalDataSize = 0;
    
    const int c_dataSize = 2048;
    int dataIdx = 0;
    int sampleCount = 0;
    engine::RData dataA(c_dataSize, codec->noChannels(), codec->noChannels());
    engine::RData dataB(c_dataSize, codec->noChannels(), codec->noChannels());
    engine::RData dataC(c_dataSize, codec->noChannels(), codec->noChannels());
    engine::RData *dataPkts[3] = {&dataA, &dataB, &dataC};
    
    engine::RData *data = dataPkts[dataIdx % 3];
    while(codec->next(*data))
    {
        filterL.process(data, 0, 0, false);
        filterR.process(data, 1, 0, false);

        if(dataIdx > 0)
        {
            data = dataPkts[(dataIdx - 1) % 3];

            for(int partIdx = 0; partIdx < data->noParts(); partIdx++)
            {
                sample_t *x = data->partFilterData(partIdx, 0);
                
                for(int idx = 0; idx < data->part(partIdx).length(); idx++)
                {
                    int chIdx;
                    sample_t y[8];
                    tubyte yOut[8 * 4];
                    
                    for(chIdx = 0; chIdx < noChannels; chIdx++)
                    {
                        y[chIdx] = x[(idx * noChannels) + chIdx];
                    }
                    
                    for(chIdx = 0; chIdx < noChannels; chIdx++)
                    {
                        engine::write32BitsLittleEndianFromSample(y[chIdx], reinterpret_cast<tchar *>(&yOut[chIdx << 2]));
                    }
                    ASSERT_EQ(out->write(yOut, noChannels * 4), noChannels * 4);
                    totalDataSize += noChannels * 4;
                    sampleCount++;
                }
            }
        }
        
        dataIdx++;
        
        data = dataPkts[dataIdx % 3];
        data->reset();
    }
    
    ASSERT_TRUE(saveWaveHeaderSize(codec, totalDataSize, out));

    out->close();
    delete out;

    codec->close();
    delete codec;
}

//-------------------------------------------------------------------------------------------

TEST(WaveDSPTest, remezTest)
{
    const tint N = 327;
    double f[] = {0.0, 8000.0 / 44100.0 , 8100.0 / 44100.0, 0.5};
    double a[] = {1.0, 1.0, 0.0, 0.0};
    double w[] = {1.0, 1.0};
    
    double h[N + 1];

    bool res = engine::RemezFIR::designBandPass(N, 2, f, a, w, h);
    ASSERT_TRUE(res);
    
    for(tint i = 0; i < N + 1; i+=4)
    {
        fprintf(stdout, "\t%.12f, %.12f, %.12f, %.12f,\n", h[i], h[i+1], h[i+2], h[i+3]);
    }
}

//-------------------------------------------------------------------------------------------

TEST(WaveDSPTest, lowPass500HzRemezLowPassFIR)
{
    const tint N = 400;
    double f[] = {0.0, 500.0 / 44100.0 , 550.0 / 44100.0, 0.5};
    double a[] = {1.0, 1.0, 0.0, 0.0};
    double w[] = {1.0, 1.0};
    
    double h[N + 1];

    bool res = engine::RemezFIR::designBandPass(N, 2, f, a, w, h);
    ASSERT_TRUE(res);

    QString inFilename = "/Users/bonez/Development/Temp/kiss_rose_44.1_24bit.wav";
    QString outFilename = "/Users/bonez/Development/Temp/lp_kiss_rose_1_remez.wav";
    
    //QString inFilename = "D:\\Music\\Temp\\kiss_rose_44.1_24bit.wav";
    //QString outFilename = "D:\\Music\\Temp\\lp_kiss_rose_1.wav";

    ASSERT_TRUE(common::DiskOps::exist(inFilename));
    
    engine::Codec *codec = engine::Codec::get(inFilename);
    ASSERT_TRUE(codec != NULL);
    ASSERT_TRUE(codec->init());
    
    if(common::DiskOps::exist(outFilename))
    {
        common::DiskOps::remove(outFilename);
    }
    
    common::BIOBufferedStream *out = new common::BIOBufferedStream(common::e_BIOStream_FileCreate | common::e_BIOStream_FileWrite);
    ASSERT_TRUE(out != NULL);
    ASSERT_TRUE(out->open(outFilename));
    
    ASSERT_TRUE(saveWaveHeaderFromCodec(codec, out));
    
    int noChannels = codec->noChannels();
    
    engine::FIRFilter filterL(h, N);
    engine::FIRFilter filterR(h, N);

    int totalDataSize = 0;
    
    const int c_dataSize = 2048;
    int dataIdx = 0;
    int sampleCount = 0;
    engine::RData dataA(c_dataSize, codec->noChannels(), codec->noChannels());
    engine::RData dataB(c_dataSize, codec->noChannels(), codec->noChannels());
    engine::RData dataC(c_dataSize, codec->noChannels(), codec->noChannels());
    engine::RData *dataPkts[3] = {&dataA, &dataB, &dataC};
    
    engine::RData *data = dataPkts[dataIdx % 3];
    while(codec->next(*data))
    {
        filterL.process(data, 0, 0, false);
        filterR.process(data, 1, 0, false);

        if(dataIdx > 0)
        {
            data = dataPkts[(dataIdx - 1) % 3];

            for(int partIdx = 0; partIdx < data->noParts(); partIdx++)
            {
                sample_t *x = data->partFilterData(partIdx, 0);
                
                for(int idx = 0; idx < data->part(partIdx).length(); idx++)
                {
                    int chIdx;
                    sample_t y[8];
                    tubyte yOut[8 * 4];
                    
                    for(chIdx = 0; chIdx < noChannels; chIdx++)
                    {
                        y[chIdx] = x[(idx * noChannels) + chIdx];
                    }
                    
                    for(chIdx = 0; chIdx < noChannels; chIdx++)
                    {
                        engine::write32BitsLittleEndianFromSample(y[chIdx], reinterpret_cast<tchar *>(&yOut[chIdx << 2]));
                    }
                    ASSERT_EQ(out->write(yOut, noChannels * 4), noChannels * 4);
                    totalDataSize += noChannels * 4;
                    sampleCount++;
                }
            }
        }
        
        dataIdx++;
        
        data = dataPkts[dataIdx % 3];
        data->reset();
    }
    
    ASSERT_TRUE(saveWaveHeaderSize(codec, totalDataSize, out));

    out->close();
    delete out;

    codec->close();
    delete codec;
}

//-------------------------------------------------------------------------------------------