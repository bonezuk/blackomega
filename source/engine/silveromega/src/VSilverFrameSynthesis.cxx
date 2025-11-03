#include "engine/silveromega/inc/VSilverContainer.h"
#include "engine/silveromega/inc/VSilverOgg.h"
#include "engine/silveromega/inc/VSilverFunctions.h"
#include "engine/silveromega/inc/VSilverFloorBase.h"
#include "engine/silveromega/inc/VSilverMapData.h"
#include "engine/silveromega/inc/VSilverWindow.h"
#include "engine/inc/OmegaDCT.h"
#if defined(OMEGA_OGG_COMPARE)
#include "engine/inc/Compare.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

void frameSynthesis(VSilverContainer *data,engine::Sequence *seq,VSilverWindow *window)
{
    tint i,j;

    void **floormemo = reinterpret_cast<void **>(::malloc(data->m_information->m_audioChannels * sizeof(void *)));
    tint *nonzero = reinterpret_cast<tint *>(::malloc(data->m_information->m_audioChannels * sizeof(tint)));
    tint *zerobundle = reinterpret_cast<tint *>(::malloc(data->m_information->m_audioChannels * sizeof(tint)));
    tfloat32 **pcmbundle = reinterpret_cast<tfloat32 **>(::malloc(data->m_information->m_audioChannels * sizeof(tfloat32 *)));

    VSilverMapData *map = data->m_data->m_mappings[window->m_mode];

    // floor curve decode
    {
        tint submap;
        VSilverFloorBase *floor;

        for(i=0;i<data->m_information->m_audioChannels;++i)
        {
            if(map->m_multiplexes!=0)
            {
                submap = map->m_multiplexes[i];
            }
            else
            {
                submap = 0;
            }
            floor = data->m_data->m_floors[map->m_floorSubmap[submap]];
            floormemo[i] = floor->decode(data->m_data,seq);
            if(floormemo[i]!=0)
            {
                nonzero[i] = 1;
            }
            else
            {
                nonzero[i] = 0;
            }
            ::memset(window->m_pcm[i],0,(window->m_pcmEnd / 2) * sizeof(tfloat32));
        }
    }

    // nonzero vector propagate
    {
        for(i=0;i<map->m_couplingSteps;++i)
        {
            if(map->m_couplingAngle[i] || map->m_couplingMagnitude[i])
            {
                nonzero[map->m_couplingAngle[i]] = 1;
                nonzero[map->m_couplingMagnitude[i]] = 1;
            }
        }
    }

    // residue decode
    {
        tint ch;
        VSilverResidueBase *residue;

        for(i=0;i<map->m_submap;++i)
        {
            ch = 0;
            for(j=0;j<data->m_information->m_audioChannels;++j)
            {
                if(map->m_multiplexes[j]==i)
                {
                    zerobundle[ch] = (nonzero[j]) ? 1 : 0;
                    pcmbundle[ch++] = window->m_pcm[j];
                }
            }
            residue = data->m_data->m_residues[map->m_residueSubmap[i]];
            residue->decode(seq,pcmbundle,zerobundle,ch);
        }
    }

#if defined(OMEGA_OGG_COMPARE)
    {
        if(g_Compare.isThreadA())
        {
            for(i=0;i<data->m_information->m_audioChannels;++i)
            {
                int frame = g_Compare.frameA();
                g_Compare.compareA(window->m_pcm[i],window->m_pcmEnd / 2,false);
            }
        }
        else
        {
            for(i=0;i<data->m_information->m_audioChannels;++i)
            {
                int frame = g_Compare.frameB();
                g_Compare.compareB(window->m_pcm[i],window->m_pcmEnd / 2,false);
            }
        }
    }
#endif

    // inverse channel coupling
    {
        tint N = window->m_pcmEnd;

        for(i=map->m_couplingSteps-1;i>=0;--i)
        {
            tfloat32 *pcmM = window->m_pcm[map->m_couplingMagnitude[i]];
            tfloat32 *pcmA = window->m_pcm[map->m_couplingAngle[i]];

            for(j=0;j<N/2;++j)
            {
                tfloat32 mag = pcmM[j];
                tfloat32 ang = pcmA[j];

                if(mag>0.0f)
                {
                    if(ang>0.0f)
                    {
                        pcmM[j] = mag;
                        pcmA[j] = mag - ang;
                    }
                    else
                    {
                        pcmA[j] = mag;
                        pcmM[j] = mag + ang;
                    }
                }
                else
                {
                    if(ang>0.0f)
                    {
                        pcmM[j] = mag;
                        pcmA[j] = mag + ang;
                    }
                    else
                    {
                        pcmA[j] = mag;
                        pcmM[j] = mag - ang;
                    }
                }
            }
        }
    }

    // compute and apply spectral envelope
    {
        tint submap;
        VSilverFloorBase *floor;

        for(i=0;i<data->m_information->m_audioChannels;++i)
        {
            if(map->m_multiplexes!=0)
            {
                submap = map->m_multiplexes[i];
            }
            else
            {
                submap = 0;
            }
            floor = data->m_data->m_floors[map->m_floorSubmap[submap]];
            floor->product(window,floormemo[i],window->m_pcm[i]);
        }
    }

#if defined(OMEGA_OGG_COMPARE)
    {
        if(g_Compare.isThreadA())
        {
            for(i=0;i<data->m_information->m_audioChannels;++i)
            {
                int frame = g_Compare.frameA();
                g_Compare.compareA(window->m_pcm[i],window->m_pcmEnd / 2,false);
            }
        }
        else
        {
            for(i=0;i<data->m_information->m_audioChannels;++i)
            {
                int frame = g_Compare.frameB();
                g_Compare.compareB(window->m_pcm[i],window->m_pcmEnd / 2,false);
            }
        }
    }
#endif

    // transform to pcm data using mdct
    {
        OmegaDCT *MDCT = OmegaDCT::get(window->m_pcmEnd);

        for(i=0;i<data->m_information->m_audioChannels;++i)
        {
            MDCT->VSInverseMDCT(window->m_pcm[i],window->m_pcm[i]);
        }
    }

#if defined(OMEGA_OGG_COMPARE)
    {
        if(g_Compare.isThreadA())
        {
            for(i=0;i<data->m_information->m_audioChannels;++i)
            {
                int frame = g_Compare.frameA();
                g_Compare.compareA(window->m_pcm[i],window->m_pcmEnd / 2,false);
            }
        }
        else
        {
            for(i=0;i<data->m_information->m_audioChannels;++i)
            {
                int frame = g_Compare.frameB();
                g_Compare.compareB(window->m_pcm[i],window->m_pcmEnd / 2,false);
            }
        }
    }
#endif

    window->window();

    for(i=0;i<data->m_information->m_audioChannels;++i)
    {
        if(floormemo[i]!=0)
        {
            free(floormemo[i]);
        }
    }
    ::free(pcmbundle);
    ::free(zerobundle);
    ::free(nonzero);
    ::free(floormemo);
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

