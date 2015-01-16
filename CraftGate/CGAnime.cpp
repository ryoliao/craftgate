
#include "CGGraph_PCH.h"

namespace cg
{

u32 CGMotion::getFrame(u32 milliseconds) const
{
    return (milliseconds / (desc.v1.duration / desc.v1.frames)) % desc.v1.frames;
}

bool CGMotion::reset(defAnimeV2 const & _desc, IStream* fAnime)
{
    if (!fAnime)
        return false;

    desc = _desc;
    frames.assign(desc.v1.frames, defAnimeFrame());
    return (desc.v1.frames == fAnime->read(frames.data(), desc.v1.frames));
}

///////////////////////////////////////////////////////////////////////////////

CGMotion const * CGAnime::findMotion(u16 dir, u16 motion) const
{
    for (u32 i=0; i<motions.size(); ++i)
    {
        if (dir == motions[i].desc.v1.dir &&
            motion == motions[i].desc.v1.motion)
            return &(motions[i]);
    }
    return 0;
}

bool CGAnime::reset(defAnimeInfo const & _desc, IRandomStream* fAnime)
{
    motions.clear();

    if (!fAnime ||
        0 != fAnime->seek(_desc.address, SEEK_SET))
        return false;

    desc = _desc;
    motions.assign(desc.motions, CGMotion());

    s32 version = (desc.version & ECAV_VERSION_BIT);
    defAnimeV2 mdesc;

    if (version == ECAV_VERSION_1)
    {
        mdesc.paletteId = 0;
        mdesc.reserved0 = 0;
        mdesc.wrap = 0;

        for (u32 i=0; i<desc.motions; ++i)
        {
            if (1 != fAnime->read(&mdesc.v1))
                return false;
            if (!motions[i].reset(mdesc, fAnime))
                return false;
        }
    }
    else
    for (u32 i=0; i<desc.motions; ++i)
    {
        if (1 != fAnime->read(&mdesc))
            return false;
        if (!motions[i].reset(mdesc, fAnime))
            return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////

CGAnimeLibrary::CGAnimeLibrary()
{
    // nothing to do with here.
}

CGAnimeLibrary::~CGAnimeLibrary()
{
    // nothing to do with here.
}

void CGAnimeLibrary::reset()
{
    animes.clear();
}

bool CGAnimeLibrary::reset(CGBin* bin)
{
    reset();

    if (!bin ||
        !bin->isOK(ECBC_ANIME) ||
        !bin->isOK(ECBC_ANIMEINFO))
        return false;

    IRandomStream* fInfo = bin->getStream(ECBC_ANIMEINFO);
    IRandomStream* fAnime = bin->getStream(ECBC_ANIME);

    defAnimeInfo ai;
    animes.reserve(fInfo->size() / sizeof(defAnimeInfo));
    while (1 == fInfo->read(&ai))
    {
        // hackcode: process MAOCG2013 version protected file.
        if (ai.version & 0xff00)
            break;

        animes.push_back(CGAnime());
        animes.back().reset(ai, fAnime);
    }

    fInfo->drop();
    fAnime->drop();

    return true;
}

CGAnime const * CGAnimeLibrary::getAnime(u32 id) const
{
    return &animes[id];
}

u32 CGAnimeLibrary::size() const
{
    return animes.size();
}

#if defined(_CG_USE_DUMP)

void CGAnimeLibrary::dump(FILE* fp)
{
    fprintf(fp, "%8s|%8s|%4s|%2s|%2s|%5s|%4s|%8s|%4s|%8s\n",
        "id", "addr", "ver", "d", "m", "pal", "wrap", "gid", "r1", "r0");

    for (u32 i=0; i<animes.size(); ++i)
    {
        CGAnime const & anim = animes[i];

        fprintf(fp, "%8d", anim.desc.Id);
        fprintf(fp, "|%8x", anim.desc.address);
        fprintf(fp, "|%4x", (u32)anim.desc.version);

        for (u32 m=0; m<anim.motions.size(); ++m)
        {
            CGMotion const & motion = anim.motions[0];
            if (m!=0)
                fprintf(fp, "\n%8s|%8s|%4s", "x", "x", "x");

            fprintf(fp, "|%2d", (u32)motion.desc.v1.dir);
            fprintf(fp, "|%2d", (u32)motion.desc.v1.motion);
            if (ECAV_VERSION_2 == (anim.desc.version & ECAV_VERSION_BIT))
            {
                fprintf(fp, "|%5d", (u32)motion.desc.paletteId);
                fprintf(fp, "|%4x", (u32)motion.desc.wrap);
            }
            else
                fprintf(fp, "|%5s|%4s", "x", "x");

            //for (u32 f=0; f<motion.frames.size(); ++f)
            //if (motion.frames.size())
            //{
            //    defAnimeFrame const & frame = motion.frames[0];
                //if (f!=0)
                //    fprintf(fp, "\n%8s|%8s|%4s|%2s|%2s|%5s|%4s", "x", "x", "x", "x", "x", "x", "x");

            //    fprintf(fp, "|%8d", (u32)frame.graphId);
            //    fprintf(fp, "|%4x", (u32)frame.reserved1);
            //    fprintf(fp, "|%8x", frame.reserved0);
            //}
        }
        fprintf(fp, "\n");
    }
}

#endif

}
