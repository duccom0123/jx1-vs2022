#ifndef KOptionH
#define	KOptionH

enum
{
	LowPlayer,
	LowNpc,
	LowMissle,
};

enum
{
	ManTypeNameIdx = 94,
	WomanTypeNameIdx = 472,
};

class KOption
{
private:
	int		m_nSndVolume;
	int		m_nMusicVolume;
	int		m_nGamma;
	BOOL	m_bLow[LowMissle+1];
public:
	int		GetSndVolume() { return m_nSndVolume; }
	void	SetSndVolume(int nSndVolume);
	int		GetMusicVolume() { return m_nMusicVolume; }
	void	SetMusicVolume(int nMusicVolume);
	int		GetGamma() { return m_nGamma; };
	void	SetGamma(int nGamma);
	void	SetLow(int n, BOOL bLow)
	{
		if(n >= LowPlayer && n <= LowMissle)
			m_bLow[n] = bLow;
	};
	BOOL	GetLow(int n) { return m_bLow[n]; };
};

extern KOption Option;
#endif