#ifndef __OMEGA_COMMON_BOPARSE_H
#define __OMEGA_COMMON_BOPARSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "common/inc/Allocation.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

//Holds production rule ( rule[a] x b -> c )
typedef struct s_BO_ProdRule 
{
    struct s_BO_ProdRule *next;
    tint b;
    tint c;
    bool end;
} BO_ProdRule;

//The BO_Parse_Unit structure holds the attributes connected with a language unit
typedef struct s_BO_Parse_Unit 
{
    struct s_BO_Parse_Unit *next;     //The next language unit in the list.
    tint state;                        //The ID (state) identifying the language unit.
    tint start;                        //The entry in the string array where the unit's first character is located.
    tint length;                        //The string length of the language unit.
} BO_Parse_Unit;

class COMMON_EXPORT BOParse 
{
    public:
        BOParse();
        ~BOParse();

        int String(const tchar *str);
        int String(const tuchar *str);

        BO_Parse_Unit *Lexical(const tchar *str);
        BO_Parse_Unit *Lexical(const tuchar *str);
        
    private:
        Allocation Alloc;

        tint prodNo;
        BO_ProdRule **rule;
        tint symTable[256];
        bool symEnd[256];

        BO_Parse_Unit *freeUnits;
        BO_Parse_Unit *stack,*first;

        BO_Parse_Unit *lex;

        bool IncRuleArray();
        tint NewRule();
        BO_ProdRule *GetProdRule();
        BO_Parse_Unit *GetUnit();
        void FreeUnit(BO_Parse_Unit *item);
        bool LexPush(tint state,tint pos);
        void LexClearStack();

        void PrintError(const char *strR,const char *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif
