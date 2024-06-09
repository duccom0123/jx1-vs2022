#ifndef KLadderH
#define	KLadderH

#include "KProtocol.h"

class KLadder
{
public:
	KLadder();
	~KLadder();
	const TRoleList*	TopTenRich();
	const TRoleList*	TopTenMasterHand();
	const TRoleList*	TopTenKiller();
	const TRoleList*	TopTenRepute();
	const TRoleList*	TopTenFuYuan();
	const TRoleList*	TopTenAccumStat1();
	const TRoleList*	TopTenAccumStat2();
	const TRoleList*	TopTenHonorStat();
	const TRoleList*	TopTenTimeStat();
	const TRoleList*	TopTenTongLv();
	const TRoleList*	TopTenTongMn();
	const TRoleList*	TopTenTongEff();
	const TRoleList*	TopTenFacRich(int nFac);
	const TRoleList*	TopTenFacMasterHand(int nFac);
	const TRoleList*	GetTopTen(DWORD	dwLadderID);
	int					GetFacMemberCount(int nFac);
	int					GetFacMasterHandPercent(int nFac);
	int					GetFacMoneyPercent(int nFac);
	BOOL				Init(void* pData, size_t uSize);
private:
	TGAME_STAT_DATA	GameStatData;
};

extern KLadder	Ladder;
#endif