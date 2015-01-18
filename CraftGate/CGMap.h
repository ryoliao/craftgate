
#ifndef _CG_MAP_H_
#define _CG_MAP_H_

namespace cg
{

/*
    class CGMap

    description:
        Cross Gate map file descriptor.
*/

enum E_CG_MAP_TYPE
{
    ECMT_CLIENT_MAP=0,
    ECMT_SERVER_MAP,
};

class CGMap
{
public:
    struct SData
    {
        u16 L0;
        u16 L1;
        u16 flags;
    };

    CGMap(wchar_t const * hint_map_file = 0);
    ~CGMap();

    void reset();
    void reset(wchar_t const * hint_map_file);

    u32 getId() const;
    std::string const & getName() const;
    CGSizei getSize() const;
    std::vector<SData> & getData();

    static bool isMap(wchar_t const * file, s32* mapType=0);

private:
    void doReadServerMap(IRandomStream* s);
    void doReadClientMap(IRandomStream* s, u32 fid);

    u32 id;
    CGSizei size;
    std::vector<SData> data;
    std::string name;
};

typedef CGSharedRef<CGMap> CGMapRef;

}

#endif