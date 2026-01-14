#include <party/RewardData_Result.h>
#include <game/party/RewardData_Result.h>

          QuickParty::RewardData_Result::RewardData_Result((QuickParty::RewardData_Result *)v75);
          v75[0] = MonsterTotalExp;

QuickParty::RewardData_Result *__cdecl QuickParty::RewardData_Result::clear(QuickParty::RewardData_Result *this)
{
  memset(this, 0, SIZE_20_BYTES);
  *((_DWORD *)this + 5) = 1065353216;
  *((_DWORD *)this + 6) = 1;
  return this;
}

QuickParty::RewardData_Result *__cdecl QuickParty::RewardData_Result::RewardData_Result(
        QuickParty::RewardData_Result *this)

  return QuickParty::RewardData_Result::clear(this);
}

//----- (084699DA) --------------------------------------------------------
int __cdecl CParty::GetSelectedEPLPCmd(CParty *this)
{

