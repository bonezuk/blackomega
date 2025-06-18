#include "gtest/gtest.h"

#include "engine/inc/BiQuadFilter.h"

using namespace omega::engine;

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, allPassFirstOrder)
{
    const sample_t c_tolerance = 0.0000001;
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;
    sample_t a = (::tan(c_PI_D * fc / fs) - 1.0) / (tan(c_PI_D*fc / fs) + 1.0);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_AllPass_FirstOrder, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_AllPass_FirstOrder);
    EXPECT_NEAR(f.a0(), a, c_tolerance);
    EXPECT_NEAR(f.a1(), 1.0, c_tolerance);
    EXPECT_NEAR(f.a2(), 0.0, c_tolerance);
    EXPECT_NEAR(f.b1(), a, c_tolerance);
    EXPECT_NEAR(f.b2(), 0.0, c_tolerance);
    EXPECT_NEAR(f.process(x[0]), 0.018287231797256782, c_tolerance);
    EXPECT_NEAR(f.process(x[1]), -0.26694954041003038, c_tolerance);
    EXPECT_NEAR(f.process(x[2]), 0.19389846391088889, c_tolerance);
    EXPECT_NEAR(f.process(x[3]), 0.45932176922691148, c_tolerance);
    EXPECT_NEAR(f.process(x[4]), 0.82017335824802806, c_tolerance);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, highPassFirstOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t th = 2.0 * c_PI_D * fc / fs;
    sample_t g = cos(th) / (1.0 + sin(th));

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_HighPass_FirstOrder, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_HighPass_FirstOrder);
    EXPECT_DOUBLE_EQ(f.a0(), (1.0 + g) / 2.0);
    EXPECT_DOUBLE_EQ(f.a1(), -((1.0 + g) / 2.0));
    EXPECT_DOUBLE_EQ(f.a2(), 0.0);
    EXPECT_DOUBLE_EQ(f.b1(), -g);
    EXPECT_DOUBLE_EQ(f.b2(), 0.0);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.13414361589862839);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.25847477020501519);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.15305076804455556);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.14533911538654426);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.66008667912401409);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, lowPassFirstOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t th = 2.0 * c_PI_D * fc / fs;
    sample_t g = ::cos(th) / (1.0 + ::sin(th));

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_LowPass_FirstOrder, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_LowPass_FirstOrder);
    EXPECT_DOUBLE_EQ(f.a0(), (1.0 - g) / 2.0);
    EXPECT_DOUBLE_EQ(f.a1(), (1.0 - g) / 2.0);
    EXPECT_DOUBLE_EQ(f.a2(), 0.0);
    EXPECT_DOUBLE_EQ(f.b1(), -g);
    EXPECT_DOUBLE_EQ(f.b2(), 0.0);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.11585638410137161);
    EXPECT_DOUBLE_EQ(f.process(x[1]), -0.0084747702050151849);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.34694923195544447);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.60466088461345568);
    EXPECT_DOUBLE_EQ(f.process(x[4]), 0.16008667912401398);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, highPassShelfFirstOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;
    const sample_t gainDB = 2.0;

    sample_t th = 2.0 * c_PI_D * fc / fs;
    sample_t m = ::pow(10.0, gainDB / 20.0);
    sample_t b = (1.0 + m) / 4.0;
    sample_t d = b * ::tan(th / 2.0);
    sample_t g = (1.0 - d) / (1.0 + d);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_HighPassShelf_FirstOrder, gainDB, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_HighPassShelf_FirstOrder);
    EXPECT_DOUBLE_EQ(f.a0(), (1.0 + g) / 2.0);
    EXPECT_DOUBLE_EQ(f.a1(), -((1.0 + g) / 2.0));
    EXPECT_DOUBLE_EQ(f.a2(), 0.0);
    EXPECT_DOUBLE_EQ(f.b1(), -g);
    EXPECT_DOUBLE_EQ(f.b2(), 0.0);
    EXPECT_DOUBLE_EQ(f.c0(), m - 1.0);
    EXPECT_DOUBLE_EQ(f.d0(), 1.0);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.29350974132512675);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.32203832261311893);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.56831378766062224);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.81703136555265554);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.69446864261541452);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, lowPassShelfFirstOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;
    const sample_t gainDB = 2.0;

    sample_t th = 2.0 * c_PI_D * fc / fs;
    sample_t m = ::pow(10.0, gainDB / 20.0);
    sample_t b = 4.0 / (1.0 + m);
    sample_t d = b * ::tan(th / 2.0);
    sample_t g = (1.0 - d) / (1.0 + d);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_LowPassShelf_FirstOrder, gainDB, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_LowPassShelf_FirstOrder);
    EXPECT_DOUBLE_EQ(f.a0(), (1.0 - g) / 2.0);
    EXPECT_DOUBLE_EQ(f.a1(), (1.0 - g) / 2.0);
    EXPECT_DOUBLE_EQ(f.a2(), 0.0);
    EXPECT_DOUBLE_EQ(f.b1(), -g);
    EXPECT_DOUBLE_EQ(f.b2(), 0.0);
    EXPECT_DOUBLE_EQ(f.c0(), m - 1.0);
    EXPECT_DOUBLE_EQ(f.d0(), 1.0);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.28913930473869098);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.25819124238106067);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.61570361570736798);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.92148157233057959);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.49674909778207765);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, allPassSecondOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t Q = 0.8;
    const sample_t fs = 44100.0;

    sample_t a = (::tan(c_PI_D*Q / fs) - 1.0) / (::tan(c_PI_D*Q / fs) + 1.0);
    sample_t b = -::cos(c_PI_D*Q / fs);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_allPassSecondOrder, Q, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_allPassSecondOrder);
    EXPECT_DOUBLE_EQ(f.a0(),  -a);
    EXPECT_DOUBLE_EQ(f.a1(), b*(1.0 - a));
    EXPECT_DOUBLE_EQ(f.a2(), 1.0);
    EXPECT_DOUBLE_EQ(f.b1(), f.a1());
    EXPECT_DOUBLE_EQ(f.b2(), f.a0());
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.24997150645236507);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.2500284903000134);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.49994300640976469);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.74980054516738837);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.50022792564689578);
}


//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, bandPassSecondOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t Q = 0.8;
    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t w = 2.0 * c_PI_D * fc / fs;
    sample_t b = 0.5*((1.0 - ::tan(w / (2.0*Q))) / (1.0 + ::tan(w / (2.0*Q))));
    sample_t g = (0.5 + b)*::cos(w);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_bandPassSecondOrder, Q, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_bandPassSecondOrder);
    EXPECT_DOUBLE_EQ(f.a0(), 0.5 - b);
    EXPECT_DOUBLE_EQ(f.a1(), 0.0);
    EXPECT_DOUBLE_EQ(f.a2(), -(0.5 - b));
    EXPECT_DOUBLE_EQ(f.b1(), -2.0 * g);
    EXPECT_DOUBLE_EQ(f.b2(), 2.0 * b);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.13818304664976697);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.12019546619345862);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.41562185047999806);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.34314481117826651);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.46423100030350162);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, bandStopSecondOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t Q = 0.8;
    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t w = 2.0 * c_PI_D * fc / fs;
    sample_t b = 0.5*((1.0 - ::tan(w / (2.0*Q))) / (1.0 + ::tan(w / (2.0*Q))));
    sample_t g = (0.5 + b)*::cos(w);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_bandStopSecondOrder, Q, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_bandStopSecondOrder);
    EXPECT_DOUBLE_EQ(f.a0(), 0.5 + b);
    EXPECT_DOUBLE_EQ(f.a1(), -2.0 * g);
    EXPECT_DOUBLE_EQ(f.a2(), 0.5 + b);
    EXPECT_DOUBLE_EQ(f.b1(), -2.0 * g);
    EXPECT_DOUBLE_EQ(f.b2(), 2.0 * b);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.11181695335023303);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.12980453380654136);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.084378149520001902);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.40685518882173338);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.03576899969649848);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, bandPassButterworthSecondOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t bW = 1.4;
    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t c = 1.0 / (::tan(c_PI_D*fc*bW / fs));
    sample_t d = 2.0 * cos(2.0 * c_PI_D * fc / fs);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_bandPassButterworthSecondOrder, bW, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_bandPassButterworthSecondOrder);
    EXPECT_DOUBLE_EQ(f.a0(), 1.0 / (1.0 + c));
    EXPECT_DOUBLE_EQ(f.a1(), 0.0);
    EXPECT_DOUBLE_EQ(f.a2(), -f.a0());
    EXPECT_DOUBLE_EQ(f.b1(), -f.a0() * (c * d));
    EXPECT_DOUBLE_EQ(f.b2(), f.a0() * (c - 1.0));
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.15189549070914579);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.13454769968679348);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.43837056390312945);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.38280653786048485);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.46954058304484636);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, bandStopButterworthSecondOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t bW = 1.4;
    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t c = ::tan(c_PI_D*fc*bW / fs);
    sample_t d = 2.0 * cos(2.0 * c_PI_D * fc / fs);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_bandStopButterworthSecondOrder, bW, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_bandStopButterworthSecondOrder);
    EXPECT_DOUBLE_EQ(f.a0(), 1.0 / (1.0 + c));
    EXPECT_DOUBLE_EQ(f.a1(), -f.a0() * d);
    EXPECT_DOUBLE_EQ(f.a2(), f.a0());
    EXPECT_DOUBLE_EQ(f.b1(), -f.a0() * d);
    EXPECT_DOUBLE_EQ(f.b2(), f.a0() * (1.0 - c));
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.098104509290854222);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.11545230031320654);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.061629436096870598);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.36719346213951526);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.030459416955153681);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, highPassButterworthSecondOrder)
{
    const sample_t c_tolerance = 0.0000001;
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t c = ::tan(c_PI_D * fc / fs);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_highPassButterworthSecondOrder, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_highPassButterworthSecondOrder);
    EXPECT_NEAR(f.a0(), 1.0 / (1.0 + c_SQR_TWO_D*c + ::pow(c, 2.0)), c_tolerance);
    EXPECT_NEAR(f.a1(), -2.0 * f.a0(), c_tolerance);
    EXPECT_NEAR(f.a2(), f.a0(), c_tolerance);
    EXPECT_NEAR(f.b1(), 2.0 * f.a0()*(::pow(c, 2.0) - 1.0), c_tolerance);
    EXPECT_NEAR(f.b2(), f.a0() * (1.0 - c_SQR_TWO_D*c + ::pow(c, 2.0)), c_tolerance);
    EXPECT_NEAR(f.process(x[0]), -0.084250172255100841, c_tolerance);
    EXPECT_NEAR(f.process(x[1]), 0.23832340012822295, c_tolerance);
    EXPECT_NEAR(f.process(x[2]), -0.028547166347985785, c_tolerance);
    EXPECT_NEAR(f.process(x[3]), -0.047014826141254615, c_tolerance);
    EXPECT_NEAR(f.process(x[4]), -0.50850588161725063, c_tolerance);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, lowPassButterworthSecondOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t c = 1.0 / (::tan(c_PI_D * fc / fs));

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_lowPassButterworthSecondOrder, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_lowPassButterworthSecondOrder);
    EXPECT_DOUBLE_EQ(f.a0(), 1.0 / (1.0 + c_SQR_TWO_D*c + ::pow(c, 2.0)));
    EXPECT_DOUBLE_EQ(f.a1(), 2.0 * f.a0());
    EXPECT_DOUBLE_EQ(f.a2(), f.a0());
    EXPECT_DOUBLE_EQ(f.b1(), 2.0 * f.a0()*(1.0 - ::pow(c, 2.0)));
    EXPECT_DOUBLE_EQ(f.b2(), f.a0() * (1.0 - c_SQR_TWO_D*c + ::pow(c, 2.0)));
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.06284499845706433);
    EXPECT_DOUBLE_EQ(f.process(x[1]), -0.07360666337155071);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.18703905574841687);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.54779962996101694);
    EXPECT_DOUBLE_EQ(f.process(x[4]), 0.43781468209608371);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, highPassSecondOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t Q = 0.8;
    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t w = 2.0 * c_PI_D * fc / fs;
    sample_t d = 1.0 / Q;
    sample_t b = 0.5*(1.0 - (d / 2)*::sin(w)) / (1.0 + (d / 2.0)*::sin(w));
    sample_t g = (0.5 + b)*::cos(w);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_highPassSecondOrder, Q, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_highPassSecondOrder);
    EXPECT_DOUBLE_EQ(f.a0(), (0.5 + b + g) / 2.0);
    EXPECT_DOUBLE_EQ(f.a1(), -(0.5 + b + g));
    EXPECT_DOUBLE_EQ(f.a2(), f.a0());
    EXPECT_DOUBLE_EQ(f.b1(), -2.0 * g);
    EXPECT_DOUBLE_EQ(f.b2(), 2.0 * b);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.088479102087170872);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.24952550443694901);
    EXPECT_DOUBLE_EQ(f.process(x[2]), -0.02273930453151778);
    EXPECT_DOUBLE_EQ(f.process(x[3]), -0.062934664074401195);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.53683000059503083);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, lowPassSecondOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t Q = 0.8;
    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t w = 2.0 * c_PI_D * fc / fs;
    sample_t d = 1.0 / Q;
    sample_t b = 0.5*(1.0 - (d / 2)*::sin(w)) / (1.0 + (d / 2.0)*::sin(w));
    sample_t g = (0.5 + b)*::cos(w);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_lowPassSecondOrder, Q, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_lowPassSecondOrder);
    EXPECT_DOUBLE_EQ(f.a0(), (0.5 + b - g) / 2.0);
    EXPECT_DOUBLE_EQ(f.a1(), 0.5 + b - g);
    EXPECT_DOUBLE_EQ(f.a2(), f.a0());
    EXPECT_DOUBLE_EQ(f.b1(), -2.0 * g);
    EXPECT_DOUBLE_EQ(f.b2(), 2.0 * b);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.065999497512173225);
    EXPECT_DOUBLE_EQ(f.process(x[1]), -0.077868638361950793);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.1995594250305126);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.58224778310152892);
    EXPECT_DOUBLE_EQ(f.process(x[4]), 0.4536447254093719);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, highPassLinkwitzRileySecondOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t th = c_PI_D * fc / fs;
    sample_t Wc = c_PI_D * fc;
    sample_t k = Wc / ::tan(th);
    sample_t d = ::pow(k, 2.0) + ::pow(Wc, 2.0) + 2.0 * k * Wc;

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_highPassLinkwitzRileySecondOrder, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_highPassLinkwitzRileySecondOrder);
    EXPECT_DOUBLE_EQ(f.a0(), ::pow(k, 2.0) / d);
    EXPECT_DOUBLE_EQ(f.a1(), -2.0 * ::pow(k, 2.0) / d);
    EXPECT_DOUBLE_EQ(f.a2(), f.a0());
    EXPECT_DOUBLE_EQ(f.b1(), (-2.0 * ::pow(k, 2.0) + 2.0 * ::pow(Wc, 2.0)) / d);
    EXPECT_DOUBLE_EQ(f.b2(), (-2.0 * k * Wc + ::pow(k, 2.0) + ::pow(Wc, 2.0)) / d);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.071978038745434986);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.20540388360550738);
    EXPECT_DOUBLE_EQ(f.process(x[2]), -0.04154275368303282);
    EXPECT_DOUBLE_EQ(f.process(x[3]), -0.0071766837527914677);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.43269588037197926);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, lowPassLinkwitzRileySecondOrder)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t th = c_PI_D * fc / fs;
    sample_t Wc = c_PI_D * fc;
    sample_t k = Wc / ::tan(th);
    sample_t d = ::pow(k, 2.0) + ::pow(Wc, 2.0) + 2.0 * k * Wc;

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_lowPassLinkwitzRileySecondOrder, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_lowPassLinkwitzRileySecondOrder);
    EXPECT_DOUBLE_EQ(f.a0(), ::pow(Wc, 2.0) / d);
    EXPECT_DOUBLE_EQ(f.a1(), 2.0 * ::pow(Wc, 2.0) / d);
    EXPECT_DOUBLE_EQ(f.a2(), f.a2());
    EXPECT_DOUBLE_EQ(f.b1(), (-2.0 * ::pow(k, 2.0) + 2.0 * ::pow(Wc, 2.0)) / d);
    EXPECT_DOUBLE_EQ(f.b2(), (-2.0 * k * Wc + ::pow(k, 2.0) + ::pow(Wc, 2.0)) / d);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.053690806948178217);
    EXPECT_DOUBLE_EQ(f.process(x[1]), -0.061545656804523015);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.15235571022785613);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.45214508547412008);
    EXPECT_DOUBLE_EQ(f.process(x[4]), 0.38747747787604869);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, parametricBoostCQ)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t gainDB = 1.5;
    const sample_t Q = 0.8;
    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t K = 2.0 * c_PI_D * fc / fs;
    sample_t V0 = ::pow(10.0, gainDB / 20.0);
    sample_t dd = 1.0 + K/Q + ::pow(K, 2.0);
    sample_t a = 1.0 + (V0*K)/Q + ::pow(K, 2.0);
    sample_t b = 2.0*(::pow(K, 2.0) - 1.0);
    sample_t g = 1.0 - (V0*K)/Q + ::pow(K, 2.0);
    sample_t d = 1.0 - K/Q + ::pow(K, 2.0);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_parametricBoostCQ, gainDB, Q, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_parametricBoostCQ);
    EXPECT_DOUBLE_EQ(f.a0(), a/dd);
    EXPECT_DOUBLE_EQ(f.a1(), b/dd);
    EXPECT_DOUBLE_EQ(f.a2(), g/dd);
    EXPECT_DOUBLE_EQ(f.b1(), b/dd);
    EXPECT_DOUBLE_EQ(f.b2(), d/dd);
    EXPECT_DOUBLE_EQ(f.c0(), 1.0);
    EXPECT_DOUBLE_EQ(f.d0(), 0.0);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.26744547475279712);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.27491509375470236);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.5461976888719503);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.75864212327944014);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.58547590884095213);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, parametricCutCQ)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t gainDB = 1.5;
    const sample_t Q = 0.8;
    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t K = 2.0 * c_PI_D * fc / fs;
    sample_t V0 = ::pow(10.0, gainDB / 20.0);
    sample_t dd = 1.0 + K / Q + ::pow(K, 2.0);
    sample_t e = 1.0 + K / (V0*Q) + ::pow(K, 2.0);
    sample_t b = 2.0*(::pow(K, 2.0) - 1.0);
    sample_t d = 1.0 - K / Q + ::pow(K, 2.0);
    sample_t h = 1.0 - K / (V0*Q) + ::pow(K, 2.0);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_parametricCutCQ, gainDB, Q, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_parametricCutCQ);
    EXPECT_DOUBLE_EQ(f.a0(), dd/e);
    EXPECT_DOUBLE_EQ(f.a1(), b/e);
    EXPECT_DOUBLE_EQ(f.a2(), d/e);
    EXPECT_DOUBLE_EQ(f.b1(), b/e);
    EXPECT_DOUBLE_EQ(f.b2(), h/e);
    EXPECT_DOUBLE_EQ(f.c0(), 1.0);
    EXPECT_DOUBLE_EQ(f.d0(), 0.0);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.26559413403825854);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.27268754939864009);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.54173610015100992);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.75453081688330559);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.57855214148810596);
}

//-------------------------------------------------------------------------------------------

TEST(BiQuadFilter, parametricNCQ)
{
    const sample_t x[5] = { -0.25, 0.25, 0.5, 0.75, -0.5 };

    const sample_t Q = 0.8;
    const sample_t gainDB = 1.6;
    const sample_t fc = 10000.0;
    const sample_t fs = 44100.0;

    sample_t w = 2.0 * c_PI_D * fc / fs;
    sample_t m = ::pow(10.0, gainDB / 20.0);
    sample_t z = 4.0 / (1.0 + m);
    sample_t b = 0.5 * ((1.0 - z*::tan(w / (2.0*Q)) / (1 + z*::tan(w / (2.0*Q)))));
    sample_t g = (0.5 + b) * ::cos(w);

    BiQuadFilter f = BiQuadFilter::filter(BiQuadFilter::e_parametricNCQ, gainDB, Q, fc, fs);
    EXPECT_EQ(f.type(), BiQuadFilter::e_parametricNCQ);
    EXPECT_DOUBLE_EQ(f.a0(), 0.5 - b);
    EXPECT_DOUBLE_EQ(f.a1(), 0.0);
    EXPECT_DOUBLE_EQ(f.a2(), -(0.5 - b));
    EXPECT_DOUBLE_EQ(f.b1(), -2.0*g);
    EXPECT_DOUBLE_EQ(f.b2(), 2.0 * b);
    EXPECT_DOUBLE_EQ(f.c0(), m - 1.0);
    EXPECT_DOUBLE_EQ(f.d0(), 1.0);
    EXPECT_DOUBLE_EQ(f.process(x[0]), -0.26749068927827485);
    EXPECT_DOUBLE_EQ(f.process(x[1]), 0.26416100473533355);
    EXPECT_DOUBLE_EQ(f.process(x[2]), 0.56055860253462375);
    EXPECT_DOUBLE_EQ(f.process(x[3]), 0.7921453642987657);
    EXPECT_DOUBLE_EQ(f.process(x[4]), -0.58060405460782039);
}

//-------------------------------------------------------------------------------------------
