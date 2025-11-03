#include "common/inc/BOParse.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

BOParse::BOParse() : Alloc(),
    prodNo(0),
    rule(0),
    freeUnits(0),
    stack(0),
    first(0),
    lex(0)
{
    tint i;

    for(i=0;i<256;i++)
    {
        symTable[i]=-1;
        symEnd[i]=false;
    }
}

//-------------------------------------------------------------------------------------------

BOParse::~BOParse()
{
    try
    {
        Alloc.FreeAll();
    }
    catch(...) {}

    rule = 0;
    freeUnits = 0;
    stack = 0;
    first = 0;
    lex = 0;
}

//-------------------------------------------------------------------------------------------

bool BOParse::IncRuleArray()
{
    tint n;
    BO_ProdRule **nRule;

    n = prodNo + 32;
    nRule = reinterpret_cast<BO_ProdRule **>(Alloc.MemAlloc(static_cast<tuint>(n),sizeof(BO_ProdRule *)));
    if(nRule==0)
    {
        PrintError("IncRuleArray","Could not increase array size");
        return false;
    }
    if(rule!=0)
    {
        ::memcpy(reinterpret_cast<void *>(nRule),reinterpret_cast<void *>(rule),static_cast<tuint>(prodNo) * sizeof(BO_ProdRule *));
        Alloc.Free(rule);
    }
    ::memset(&nRule[prodNo],0,32*sizeof(BO_ProdRule *));
    rule = nRule;
    return true;
}

//-------------------------------------------------------------------------------------------

tint BOParse::NewRule()
{
    if(!(prodNo&0x1f))
    {
        if(!IncRuleArray())
        {
            return -1;
        }
    }
    prodNo++;
    return prodNo - 1;
}

//-------------------------------------------------------------------------------------------

BO_ProdRule *BOParse::GetProdRule()
{
    BO_ProdRule *item;

    item=reinterpret_cast<BO_ProdRule *>(Alloc.MemAlloc(1,sizeof(BO_ProdRule)));
    if(item==0)
    {
        PrintError("GetProdRule","Out of memory");
    }
    return item;
}

//-------------------------------------------------------------------------------------------

BO_Parse_Unit *BOParse::GetUnit()
{
    BO_Parse_Unit *item;

    item = reinterpret_cast<BO_Parse_Unit *>(Alloc.MemAlloc(1,sizeof(BO_Parse_Unit)));
    return item;
}

//-------------------------------------------------------------------------------------------

void BOParse::FreeUnit(BO_Parse_Unit *item)
{
    Alloc.Free(item);
}

//-------------------------------------------------------------------------------------------

void BOParse::PrintError(const char *strR,const char *strE) const
{
    common::Log::g_Log << "BOParse::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

tint BOParse::String(const tchar *str)
{
    return String(reinterpret_cast<const tuchar *>(str));
}

//-------------------------------------------------------------------------------------------

tint BOParse::String(const tuchar *str)
{
    tint i,a;
    tint state_a,state_b;
    tint len;
    BO_ProdRule *item,*pItem;

    if(str==0)
    {
        return -1;
    }
    len=static_cast<tint>(::strlen(reinterpret_cast<const tchar *>(str)));

    if(len==1)
    {
        a=static_cast<tint>((str[0]>='a' && str[0]<='z') ? str[0]+('A'-'a') : str[0]);
        if(symTable[a]==-1)
        {
            if((symTable[a]=NewRule())==-1)
            {
                return -1;
            }
        }
        state_a=symTable[a];
        symEnd[a]=true;
    }
    else
    {
        for(i=0;i<len;i++)
        {
            a=static_cast<tint>((str[i]>='a' && str[i]<='z') ? str[i]+('A'-'a') : str[i]);
            if(symTable[a]==-1)
            {
                if((symTable[a]=NewRule())==-1)
                {
                    return -1;
                }
            }
        }

        a=static_cast<tint>((str[0]>='a' && str[0]<='z') ? str[0]+('A'-'a') : str[0]);
        state_a=symTable[a];

        for(i=1;i<len;i++)
        {
            a=static_cast<tint>((str[i]>='a' && str[i]<='z') ? str[i]+('A'-'a') : str[i]);
            state_b=symTable[a];

            pItem=0;

            if(rule==0)
            {
                return -1;
            }

            item=rule[state_a];
            while(item!=0)
            {
                if(state_b==item->b)
                {
                    break;
                }
                pItem=item;
                item=item->next;
            }
            if(item==0)
            {
                if((item=GetProdRule())==0)
                {
                    return -1;
                }
                item->end=false;
                item->next=0;
                if(pItem==0)
                {
                    rule[state_a]=item;
                }
                else
                {
                    pItem->next=item;
                }
                item->b=state_b;
                if((item->c=NewRule())==-1)
                {
                    return -1;
                }
            }
            if(i==(len-1))
            {
                item->end=true;
            }
            state_a=item->c;
        }
    }

    return state_a;
}

//-------------------------------------------------------------------------------------------

bool BOParse::LexPush(tint state,tint pos)
{
    BO_Parse_Unit *item=GetUnit();

    if(item==0)
    {
        return false;
    }
    item->next=stack;
    stack=item;
    item->state=state;
    item->start=pos;
    return true;
}

//-------------------------------------------------------------------------------------------

void BOParse::LexClearStack()
{
    BO_Parse_Unit *item;

    while(stack!=0)
    {
        item=stack;
        stack=item->next;
        FreeUnit(item);
    }
}

//-------------------------------------------------------------------------------------------

BO_Parse_Unit *BOParse::Lexical(const tchar *str)
{
    return Lexical(reinterpret_cast<const tuchar *>(str));
}

//-------------------------------------------------------------------------------------------

BO_Parse_Unit *BOParse::Lexical(const tuchar *str)
{
    tint state_a,state_b,i,j;
    BO_Parse_Unit *pItem=0,*item=0;
    tint a,b;
    tint len;
    BO_ProdRule *prule;

    first=lex;

    while(first!=0)
    {
        item=first;
        first=item->next;
        FreeUnit(item);
    }

    stack=0;

    if(str==0)
    {
        lex=0;
        return 0;
    }

    len=static_cast<tint>(::strlen(reinterpret_cast<const tchar *>(str)));

    for(i=0;i<len;i++)
    {
        a=static_cast<tint>((str[i]>='a' && str[i]<='z') ? str[i]+('A'-'a') : str[i]);
        if(symTable[a]!=-1)
        {
            state_a=symTable[a];
            if(symEnd[a])
            {
                if(!LexPush(state_a,i))
                {
                    return 0;
                }
            }

            for(j=i+1;j<len;j++)
            {
                b=static_cast<tint>((str[j]>='a' && str[j]<='z') ? str[j]+('A'-'a') : str[j]);
                state_b=symTable[b];

                if(rule==0)
                {
                    return 0;
                }

                prule=rule[state_a];
                while(prule!=0)
                {
                    if(prule->b==state_b)
                    {
                        if(prule->end)
                        {
                            if(!LexPush(prule->c,j))
                            {
                                return 0;
                            }
                        }
                        break;
                    }
                    prule=prule->next;
                }
                if(prule==0)
                {
                    break;
                }
                else
                {
                    state_a=prule->c;
                }
            }

            if(stack!=0)
            {

                item = GetUnit();
                if(item==0)
                {
                    return 0;
                }
                if(pItem==0)
                {
                    first=item;
                }
                else
                {
                    pItem->next=item;
                }
                item->next=0;
                item->state=stack->state;
                item->start=i;
                item->length=(stack->start-i)+1;
                pItem=item;
                i+=item->length-1;
                LexClearStack();
            }
        }
    }

    lex=first;

    return lex;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
