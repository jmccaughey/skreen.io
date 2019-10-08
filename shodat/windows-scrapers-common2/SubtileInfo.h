#pragma once
class SubtileInfo
{
public:
	~SubtileInfo(void);
	SubtileInfo(const char * hash, int x, int y);
	SubtileInfo(void);	
	SubtileInfo & operator=(const SubtileInfo &rhs);
	bool operator==(const SubtileInfo &other) const;
	bool operator>(const SubtileInfo &other) const;
	void copyTo(char* buff, int offset);
	int getX(){ return _x; }
	int getY(){ return _y; }

private:
	int _x;
	int _y;
	char _hash[32];
};
