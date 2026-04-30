#include "gtest/gtest.h"
#include "fftw3.h"

#include "engine/inc/Codec.h"
#include "engine/inc/RData.h"

//-------------------------------------------------------------------------------------------

using namespace omega;

// FIR filter coefficients (generated from FIRFilter.dat)
// Length: 4096

//extern const double dsd_fir_coeffs[4095];
#include "DSDFIRFilter.cxx"

//-------------------------------------------------------------------------------------------

void read_from_codec(engine::Codec *codec, int channelIdx, int noSamples, double *buffer)
{
    engine::RData *codecData = NULL;
    sample_t *in;
    int offset, idx = 0;
    bool isMore = true;

    codecData = new engine::RData(4096, codec->noChannels(), codec->noChannels());

    while(idx < noSamples && isMore)
    {
        codecData->reset();
        isMore = codec->next(*codecData);
        offset = 0;
        in = codecData->partData(0);
        while(offset < codecData->part(0).length() && idx < noSamples)
        {
            buffer[idx] = in[(offset * codec->noChannels()) + channelIdx];
            offset++;
            idx++;
        }
    }
    while(idx < noSamples)
    {
        buffer[idx] = 0.0;
        idx++;
    }
    delete codecData;
}

//-------------------------------------------------------------------------------------------

void FFTInit(fftw_plan *plan, unsigned int fftsize, int Times, double *fftin, fftw_complex *fftout) 
{
	fftw_plan_with_nthreads(QThread::idealThreadCount());
	*plan = fftw_plan_dft_r2c_1d(int(fftsize / Times), fftin, fftout, FFTW_ESTIMATE);
}

void iFFTInit(fftw_plan *plan, unsigned int fftsize, int Times, fftw_complex *ifftin, double *ifftout) 
{
	fftw_plan_with_nthreads(QThread::idealThreadCount());
	*plan = fftw_plan_dft_c2r_1d(int(fftsize / Times), ifftin, ifftout, FFTW_ESTIMATE);
}

double *read_wav_channel_data(engine::Codec *codec, int channelIndex, unsigned __int64& samplesize, unsigned int section_1, unsigned int Times)
{
    double nS = static_cast<double>(codec->length()) * static_cast<double>(codec->frequency());
    int noSamples = (int)(round(nS));

    int fillsize = (section_1 + 1) * Times - (noSamples % ((section_1 + 1) * Times));

    int i;
    double *mem = new double [fillsize + noSamples];
    for(i = 0; i < fillsize; i++)
    {
        mem[i] = 0.0;
    }
    read_from_codec(codec, channelIndex, noSamples, &mem[fillsize]);

    samplesize = fillsize + noSamples;

    return mem;
}

void wav_filter_renew(engine::Codec *codec, int channelIndex, QVector<QByteArray>& dsdOut, unsigned int Times)
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
    for(s = 0; s < (int)order; s++)
    {
        NS[0][s] = c_noiseShapeCoefficient[0][s];
        NS[1][order - s - 1] = c_noiseShapeCoefficient[1][s];
    }
	for (i = 0; i < order; i++) 
    {
		NS[1][i] = NS[1][i];
		NS[0][i] = NS[0][i] - NS[1][i];
	}

    unsigned __int64 wavOffset = 0, samplesize = 0;
    double *wavData = read_wav_channel_data(codec, channelIndex, samplesize, section_1, Times);

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
        double progress = static_cast<double>(i) * 100.0 / static_cast<double>(SplitNum);
        printf("Progress = %d\r", (int)progress);

        int noSampleBlock = datasize / Times;
        for(int j = 0; j < noSampleBlock; j++)
        {
            if(wavOffset < samplesize)
            {
                buffer[j] = wavData[wavOffset];
                wavOffset++;
            }
            else
            {
                buffer[j] = 0.0;
            }
        }

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
            QByteArray arr(reinterpret_cast<const char *>(out), datasize);
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

template<class T> T reverse_endian(T value)
{
	char* first = reinterpret_cast<char*>(&value);
	char* last = first + sizeof(T);
	std::reverse(first, last);
	return value;
}

tuint64 bytearray_size(QVector<QByteArray>& arrList)
{
    tuint64 size = 0;
    for(QVector<QByteArray>::const_iterator ppI = arrList.begin(); ppI != arrList.end(); ppI++)
    {
        const QByteArray& arr = *ppI;
        size += arr.size();
    }
    return size;
}

int bytearray_seek(QVector<QByteArray>& arrList, tuint64 seekP, int& offset)
{
    int blockIdx = 0;
    tuint64 pos;

    pos = 0;
    offset = 0;
    blockIdx = 0;
    while(blockIdx < arrList.size() && pos < seekP)
    {
        const QByteArray& arr = arrList.at(blockIdx);
        tuint64 remain = seekP - pos;
        if(remain < (tuint64)arr.size())
        {
            offset = remain;
            pos += remain;
        }
        else
        {
            offset = 0;
            pos += arr.size();
            blockIdx++;
        }
    }
    return blockIdx;
}

void bytearray_read(QVector<QByteArray>& arrList, int& blockIdx, int& offset, unsigned char *out, int len)
{
    int pos = 0;
    while(pos < len && blockIdx < arrList.size())
    {
        const QByteArray& arr = arrList.at(blockIdx);
        const unsigned char *in = reinterpret_cast<const unsigned char *>(arr.data());
        int remain = len - pos;
        int amount = arr.size() - offset;
        if(amount <= remain)
        {
            memcpy(&out[pos], &in[offset], amount);
            offset = 0;
            pos += amount;
            blockIdx++;
        }
        else
        {
            memcpy(&out[pos], &in[offset], remain);
            offset += remain;
            pos += remain;
        }
    }
    if(pos < len)
    {
        memset(&out[pos], 0, len - pos);
    }
}

bool dsd_write(engine::Codec *inCodec, const QString& outFilename, int DSD_Times, QVector<QByteArray>& leftDSD, QVector<QByteArray> rightDSD)
{
    FILE *WriteData = fopen(outFilename.toUtf8().constData(), "wb");
    if(WriteData == NULL)
        return false;

	int OrigSamplingRate = inCodec->frequency();
	int BaseSamplingRate;
	if (OrigSamplingRate % 44100 == 0) {
		BaseSamplingRate = 44100;
	}
	else {
		BaseSamplingRate = 48000;
	}
    int DSD_SamplingRate = BaseSamplingRate * DSD_Times;

    unsigned __int64 OrigDataSize = static_cast<unsigned __int64>(static_cast<tfloat64>(inCodec->length()) * static_cast<tfloat64>(inCodec->frequency()));
	unsigned __int64 DSD_SampleSize = OrigDataSize * (DSD_SamplingRate / OrigSamplingRate);
	unsigned __int64 DSD_DataSize = DSD_SampleSize / 4;

    int blockIdxL = 0, offsetL = 0;
    tuint64 seekL = bytearray_size(leftDSD) - DSD_SampleSize;
    blockIdxL = bytearray_seek(leftDSD, seekL, offsetL);

    int blockIdxR = 0, offsetR = 0;
    tuint64 seekR = bytearray_size(rightDSD) - DSD_SampleSize;
    blockIdxR = bytearray_seek(rightDSD, seekR, offsetR);

 	fwrite("FRM8", 4, 1, WriteData);//FRM8
	unsigned __int64 binary = 0;
	unsigned short ushort = 0;
	unsigned char uchar = 0;
	unsigned __int64 ulong = 0;
	unsigned __int64 tell = 0;
	binary = reverse_endian(DSD_DataSize + 152);
	fwrite(&binary, 8, 1, WriteData);
	fwrite("DSD ", 4, 1, WriteData);//DSD

	fwrite("FVER", 4, 1, WriteData);//FVER
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);// Chunk size
	binary = reverse_endian(4);
	fwrite(&binary, 4, 1, WriteData); //Chunk size

	//Version
	binary = 1;
	fwrite(&binary, 1, 1, WriteData);
	binary = 5;
	fwrite(&binary, 1, 1, WriteData);
	binary = 0;
	fwrite(&binary, 1, 1, WriteData);
	binary = 0;
	fwrite(&binary, 1, 1, WriteData);

	fwrite("PROP", 4, 1, WriteData);//PROP
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian(108);
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	fwrite("SND ", 4, 1, WriteData);//SND

	fwrite("FS  ", 4, 1, WriteData);//FS
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian(4);
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	unsigned __int32 binary1;
	binary1 = reverse_endian(DSD_SamplingRate);
	fwrite(&binary1, 4, 1, WriteData);//SamplingRate

	fwrite("CHNL", 4, 1, WriteData);//CHNL
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian(10);
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = 0;//number of channel
	fwrite(&binary, 1, 1, WriteData);
	binary = 2;
	fwrite(&binary, 1, 1, WriteData);
	fwrite("SLFT", 4, 1, WriteData);//SLFT
	fwrite("SRGT", 4, 1, WriteData);//SRGT

	fwrite("CMPR", 4, 1, WriteData);//CMPR
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian(20);
	fwrite(&binary, 4, 1, WriteData);//Chunk size

	fwrite("DSD ", 4, 1, WriteData);//DSD
	binary = 14;
	fwrite(&binary, 1, 1, WriteData);
	fwrite("not compressed ", 15, 1, WriteData);//not compressed

	fwrite("ABSS", 4, 1, WriteData);//ABSS
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian(8);
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	fwrite(&ushort, 2, 1, WriteData);//Hours
	fwrite(&uchar, 1, 1, WriteData);//Minutes
	fwrite(&uchar, 1, 1, WriteData);//Seconds
	fwrite(&ulong, 4, 1, WriteData);//samples

	fwrite("LSCO", 4, 1, WriteData);//LSCO
	binary = 0;
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	binary = reverse_endian(2);
	fwrite(&binary, 4, 1, WriteData);//Chunk size
	fwrite(&ushort, 2, 1, WriteData);//IsConfig

	fwrite("DSD ", 4, 1, WriteData);//DSD
	binary = reverse_endian(DSD_DataSize);
	fwrite(&binary, 8, 1, WriteData);//Chunk size   

	unsigned __int64 i = 0;
	int buffersize = 16384 * 2 * 8;
	unsigned char* onebit = new unsigned char[buffersize / 4];
	unsigned char* tmpdataL = new unsigned char[buffersize];
	unsigned char* tmpdataR = new unsigned char[buffersize];
	unsigned char tmpL = 0; unsigned char tmpR = 0;
	int n = 0;
	int p = 0;
	int t = 0;
	unsigned __int64 k = 0;
	//WAV_Filter‚ÍLR‚²‚ÆUnsignedChar‚Å‘‚«o‚µ‚Ä‚¢‚é‚Ì‚ÅA‚»‚ê‚ð8ƒTƒ“ƒvƒ‹1ƒoƒCƒg‚É‚Ü‚Æ‚ß‚Ä‚©‚ç
	//ƒf[ƒ^—Ìˆæ‚Æ‚µ‚Ä‘‚«o‚·
	for (i = 0; i < DSD_SampleSize / buffersize; i++) {
		p = 0;
        bytearray_read(leftDSD, blockIdxL, offsetL, tmpdataL, buffersize);
        bytearray_read(rightDSD, blockIdxR, offsetR, tmpdataR, buffersize);
		for (k = 0; k < buffersize / 4; k++) {
			onebit[k] = tmpdataL[p] << 7;
			onebit[k] += tmpdataL[p + 1] << 6;
			onebit[k] += tmpdataL[p + 2] << 5;
			onebit[k] += tmpdataL[p + 3] << 4;
			onebit[k] += tmpdataL[p + 4] << 3;
			onebit[k] += tmpdataL[p + 5] << 2;
			onebit[k] += tmpdataL[p + 6] << 1;
			onebit[k] += tmpdataL[p + 7] << 0;
			k++;
			onebit[k] = tmpdataR[p] << 7;
			onebit[k] += tmpdataR[p + 1] << 6;
			onebit[k] += tmpdataR[p + 2] << 5;
			onebit[k] += tmpdataR[p + 3] << 4;
			onebit[k] += tmpdataR[p + 4] << 3;
			onebit[k] += tmpdataR[p + 5] << 2;
			onebit[k] += tmpdataR[p + 6] << 1;
			onebit[k] += tmpdataR[p + 7] << 0;
			p += 8;
		}
		fwrite(onebit, 1, buffersize / 4, WriteData);//DSD_Data
	}

	for (k = i * buffersize / 4; k < DSD_DataSize; k += 2) {
        bytearray_read(leftDSD, blockIdxL, offsetL, tmpdataL, 8);
        bytearray_read(rightDSD, blockIdxR, offsetR, tmpdataR, 8);
		for (n = 0; n < 8; n++) {
			if (tmpdataL[n] == 1) {
				tmpL += unsigned char(pow(2, 7 - n));
			}
			if (tmpdataR[n] == 1) {
				tmpR += unsigned char(pow(2, 7 - n));
			}
		}
		fwrite(&tmpL, 1, 1, WriteData);
		fwrite(&tmpR, 1, 1, WriteData);
		tmpL = 0;
		tmpR = 0;
	}
	tell = _ftelli64(WriteData);

	delete[] onebit;
	delete[] tmpdataL;
	delete[] tmpdataR; 

    fclose(WriteData);
    return true;
}

bool dsd_wav_convert(const QString& inFilename, const QString& outFilename)
{
    engine::Codec *codecL = engine::Codec::get(inFilename);
    if(codecL == NULL)
        return false;
    if(!codecL->init())
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
    wav_filter_renew(codecL, 0, leftDSD, Times);
    delete codecL;

    engine::Codec *codecR = engine::Codec::get(inFilename);
    if(codecR == NULL)
        return false;
    if(!codecR->init())
        return false;

    wav_filter_renew(codecR, 0, rightDSD, Times);

    bool res = dsd_write(codecR, outFilename, DSD_Times, leftDSD, rightDSD);
    delete codecR;
    return res;
}

TEST(PCM2DSDRevA, convertA)
{
    QString inF = "D:\\Development\\Temp\\dsd\\battle_10sec.wav";
    QString outF = "D:\\Development\\Temp\\dsd\\battle_10sec.dff";
    EXPECT_TRUE(dsd_wav_convert(inF, outF));
}