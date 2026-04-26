#include "gtest/gtest.h"
#include "fftw3.h"

#include "engine/inc/Codec.h"
#include "engine/inc/RData.h"

//-------------------------------------------------------------------------------------------

using namespace omega;

extern double dsd_fir_coeffs[];

//-------------------------------------------------------------------------------------------

void read_from_codec(engine::Codec *codec, int channelIdx, int noSamples, double *buffer)
{
    static int offset = -1;
    static engine::RData *codecData = NULL;

    int idx = 0;

    if(codecData == NULL)
    {
        codecData = new engine::RData(4096, codec->noChannels(), codec->noChannels());
    }
    sample_t *in = codecData->partData(0);

    if(offset >= 0)
    {
        while(offset < codecData->part(0).length() && idx < noSamples)
        {
            buffer[idx] = in[(offset * codec->noChannels) + channelIdx]
            offset++;
            idx++;
        }
        offset = -1;
    }
    if(!codecData->isComplete())
    {
        while(idx < noSamples)
        {
            codecData->reset();
            if(codec->next(*cocdecData))
            {
                offset = 0;
                in = codecData->partData(0);
                while(offset < codecData->part(0).length() && idx < noSamples)
                {
                    buffer[idx] = in[(offset * codec->noChannels) + channelIdx]
                    offset++;
                    idx++;
                }
            }
            else
            {
                break;
            }
        }
    }
    while(idx < noSamples)
    {
        buffer[idx] = 0.0;
        idx++;
    }
}

//-------------------------------------------------------------------------------------------

void wav_filter_renew(engine::Codec *codec, int channelIndex, QVector<QByteArray>& dsdOut)
{
    const double c_noiseShapeCoefficient[2][8] = {
        {
            7.9959288660876267,
            -27.975578426642503,
            55.938953909594289,
            -69.91860869807897,
            55.938953909594282,
            -27.975578426642503,
            7.9959288660876267,
            -0.99999999999999978
        },
        {
            -0.54184091028035863,
            4.6211255639903257,
            -17.319327891504788,
            37.237233820988472,
            -50.213208401223746,
            43.471176498498245,
            -23.588887732076959,
            7.3337285721047616
        }
    };

    unsigned int section_1 = 4095;
    double* firfilter_table = new double[section_1];
    unsigned int i = 0;
    for(i = 0; i < 4095; i++)
    {
        firfilter_table[i] = dsd_fir_coeffs[i];
    }

    int s = 0;
    unsigned int order = 8;
	double** NS = new double* [2];
	NS[0] = new double[order];
	NS[1] = new double[order];
    for(s = 0; s < order; s++)
    {
        NS[0][s] = c_noiseShapeCoefficient[0][s];
        NS[1][order - s - 1] = c_noiseShapeCoefficient[1][s];
    }
	for (i = 0; i < order; i++) 
    {
		NS[1][i] = NS[1][i];
		NS[0][i] = NS[0][i] - NS[1][i];
	}

    unsigned __int64 samplesize;
    sampleSize = (unsigned __int64)(static_cast<tfloat64>(codec->length()) * codec->frequency());

	// FIR filter convolution operation using the FFT Overlap-Add Method
	// When x(L), h(N), and FFT size M are used, M must satisfy M >= L + N - 1, so
	// At the final upsampling stage it is defined so that M = 2 * L = 2 * (N + 1)
	//FFT Overlap add Method‚ð—p‚¢‚½FIRƒtƒBƒ‹ƒ^ô‚Ý‚±‚Ý‰‰ŽZ
	//x(L),h(N),FFT(M)‚Æ‚µ‚½‚Æ‚«AM>=L+N-1‚É‚È‚é•K—v‚ª‚ ‚é‚Ì‚Å
	//ÅIƒAƒbƒvƒTƒ“ƒvƒŠƒ“ƒOŽž‚ÉM=2*L=2*(N+1)‚Æ‚È‚é‚æ‚¤‚É’è‹`
	const unsigned int logtimes = unsigned int(log(Times) / log(2));
	const unsigned int fftsize = (section_1 + 1) * Times;
	const unsigned int datasize = fftsize / 2;
	unsigned int* nowfftsize = new  unsigned int[logtimes];
	unsigned int* zerosize = new  unsigned int[logtimes];
	unsigned int* puddingsize = new  unsigned int[logtimes];
	unsigned int* realfftsize = new unsigned int[logtimes];
	unsigned int* addsize = new  unsigned int[logtimes];
	double** prebuffer = new double* [logtimes];
	double gain = 1;

	double* buffer = new double[fftsize];
	unsigned char* out = new unsigned char[datasize];
	for (i = 0; i < datasize; i++) 
    {
		out[i] = 0;
	}

	double* deltabuffer = new double[order + 1];
	for (i = 0; i < order; i++) 
    {
		deltabuffer[i] = 0;
	}

	double x_in = 0;
	double error_y = 0;
	double deltagain = 0.5;

	// FIR FFT—p•Ï”
	double** fftin = (double**)fftw_malloc(sizeof(double) * logtimes);
	fftw_complex** fftout = (fftw_complex**)fftw_malloc(sizeof(fftw_complex) * logtimes);
	fftw_complex** ifftin = (fftw_complex**)fftw_malloc(sizeof(fftw_complex) * logtimes);
	double** ifftout = (double**)fftw_malloc(sizeof(double) * logtimes);
	fftw_complex** firfilter_table_fft = (fftw_complex**)fftw_malloc(sizeof(fftw_complex) * logtimes);

	fftw_plan* FFT = (fftw_plan*)fftw_malloc(sizeof(fftw_plan) * (logtimes));
	fftw_plan* iFFT = (fftw_plan*)fftw_malloc(sizeof(fftw_plan) * (logtimes));

	unsigned int p = 0;
	unsigned int k = 0;
	unsigned int t = 0;
	unsigned int q = 0;
	for (i = 1; i < Times; i = i * 2) 
    {
		nowfftsize[p] = fftsize / (Times / (i * 2));
		realfftsize[p] = nowfftsize[p] / 2 + 1;
		zerosize[p] = nowfftsize[p] / 4;
		puddingsize[p] = nowfftsize[p] - zerosize[p] * 2;
		gain = gain * (2.0 / nowfftsize[p]);
		addsize[p] = zerosize[p] * 2;

		prebuffer[p] = new double[fftsize];
		firfilter_table_fft[logtimes - p - 1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * unsigned int(fftsize / i));
		fftin[logtimes - p - 1] = (double*)fftw_malloc(sizeof(double) * unsigned int(fftsize / i));
		fftout[logtimes - p - 1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * unsigned int(fftsize / i));
		ifftin[logtimes - p - 1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * unsigned int((fftsize / i + 1) / 2 + 1));
		ifftout[logtimes - p - 1] = (double*)fftw_malloc(sizeof(double) * unsigned int(fftsize / i));

		for (k = 0; k < fftsize / i; k++) 
        {
			fftin[logtimes - p - 1][k] = 0;
			ifftout[logtimes - p - 1][k] = 0;
			fftout[logtimes - p - 1][k][0] = 0;
			fftout[logtimes - p - 1][k][1] = 0;
			ifftin[logtimes - p - 1][k / 2][0] = 0;
			ifftin[logtimes - p - 1][k / 2][1] = 0;
		}
		for (k = 0; k < fftsize; k++) 
        {
			prebuffer[p][k] = 0;
		}
		p++;
	}
	p = 0;
	//!!!FFTW3ƒ‰ƒCƒuƒ‰ƒŠ‚É“¯ŽžƒAƒNƒZƒX‚·‚é‚Æ—Ž‚¿‚é
	for (i = 1; i < Times; i = i * 2) 
    {
		FFTInit(&FFT[logtimes - p - 1], fftsize, int(i), fftin[logtimes - p - 1], fftout[logtimes - p - 1]);
		iFFTInit(&iFFT[logtimes - p - 1], fftsize, int(i), ifftin[logtimes - p - 1], ifftout[logtimes - p - 1]);
		p++;
	}
	///H(n)€”õ
	for (k = 0; k < logtimes; k++) 
    {
		for (i = 0; i < section_1; i++) 
        {
			fftin[k][i] = firfilter_table[i];
		}
		for (i = section_1; i < fftsize / pow(2, k); i++) 
        {
			fftin[logtimes - k - 1][i] = 0;
		}
	}
	for (i = 0; i < logtimes; i++) {
		fftw_execute(FFT[logtimes - i - 1]);
		for (p = 0; p < fftsize / pow(2, i + 1) + 1; p++) 
        {
			firfilter_table_fft[logtimes - i - 1][p][0] = fftout[logtimes - i - 1][p][0];
			firfilter_table_fft[logtimes - i - 1][p][1] = fftout[logtimes - i - 1][p][1];
		}
	}

    unsigned __int64 SplitNum = unsigned __int64((samplesize / datasize) * Times);
    deltagain = gain * deltagain;
    for (i = 0; i < SplitNum; i++) 
    {
        read_from_codec(codec, channelIndex, datasize / Times, buffer)
        for (t = 0; t < logtimes; t++) 
        {
            q = 0;
            for (p = 0; p < zerosize[t]; p++) 
            {
                fftin[t][q] = buffer[p];
                q++;
                fftin[t][q] = 0;
                q++;
            }
            memset(fftin[t] + q, 0, 8 * (nowfftsize[t] - q));
            fftw_execute(FFT[t]);
            for (p = 0; p < realfftsize[t]; p++) 
            {
                ifftin[t][p][0] = fftout[t][p][0] * firfilter_table_fft[t][p][0] - fftout[t][p][1] * firfilter_table_fft[t][p][1];
                ifftin[t][p][1] = fftout[t][p][0] * firfilter_table_fft[t][p][1] + firfilter_table_fft[t][p][0] * fftout[t][p][1];
            }
            fftw_execute(iFFT[t]);
            for (p = 0; p < puddingsize[t]; p++) 
            {
                buffer[p] = prebuffer[t][p] + ifftout[t][p];
            }
            q = 0;
            for (p = puddingsize[t]; p < nowfftsize[t]; p++) 
            {
                buffer[p] = prebuffer[t][q] = ifftout[t][p];
                q++;
            }
        }
        //1bit‰»
        //Direct Form IIŒ^‚Å‡™ƒ°•Ï’²
        // 1-bit conversion
        // Frequency conversion using Direct Form II structure
        for (q = 0; q < datasize; q++) 
        {
            x_in = buffer[q] * deltagain;

            for (t = 0; t < order; t++) 
            {
                x_in += NS[0][t] * deltabuffer[t];
            }

            if (x_in >= 0.0) 
            {
                out[q] = 1;
                error_y = -1.0;
            }
            else 
            {
                out[q] = 0;
                error_y = 1.0;
            }
            for (t = order; t > 0; t--) 
            {
                deltabuffer[t] = deltabuffer[t - 1];
            }

            deltabuffer[0] = x_in + error_y;

            for (t = 0; t < order; t++) 
            {
                deltabuffer[0] += NS[1][t] * deltabuffer[t + 1];
            }
        }
        {
            QByteArray arr(out, datasize);
            dsdOut.append(arr);
        }
    }

    for (i = 0; i < logtimes; i++) 
    {
        fftw_destroy_plan(FFT[i]);
        fftw_destroy_plan(iFFT[i]);
        fftw_free(ifftin[i]);
        fftw_free(ifftout[i]);
        fftw_free(fftin[i]);
        fftw_free(fftout[i]);
        fftw_free(firfilter_table_fft[i]);
        delete[] prebuffer[i];
    }
    fftw_free(iFFT);
    fftw_free(FFT);
    fftw_free(ifftin);
    fftw_free(ifftout);
    fftw_free(fftin);
    fftw_free(fftout);
    fftw_free(firfilter_table_fft);
    delete[] NS[0];
    delete[] NS[1];
    delete[] NS;
    delete[] nowfftsize;
    delete[] zerosize;
    delete[] puddingsize;
    delete[] realfftsize;
    delete[] addsize;
    delete[] out;
    delete[] prebuffer;
    delete[] buffer;
    delete[] deltabuffer;
    delete[] firfilter_table;
}

bool dsd_wav_convert(const QString& inFilename)
{
    engine::Codec *codecL engine::Codec::get(inFilename);
    if(codecL == NULL)
        return false;
    if(codecL->init())
        return false;

    int Precision = 0;
    unsigned int DSD_Times = 64; // DSD64
	unsigned int Times = 0;
	unsigned int DSDSamplingRate = 0;
	int samplingrate = codecL->frequency();
	if (0 == samplingrate % 44100) {
		Times = DSD_Times / (samplingrate / 44100);
		DSDSamplingRate = samplingrate * Times;
	}
	else {
		Times = DSD_Times / (samplingrate / 48000);
		DSDSamplingRate = samplingrate * Times;
	}

    QVector<QByteArray> leftDSD, rightDSD;
    wav_filter_renew(codecL, 0, leftDSD);
    delete codecL;

    engine::Codec *codecR engine::Codec::get(inFilename);
    if(codecR == NULL)
        return false;
    if(codecR->init())
        return false;

    wav_filter_renew(codecR, 0, rightDSD);
}