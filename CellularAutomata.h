// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "DungeonGenerator.h" // for MAX size
#include "GeneratorMap.h"

#define TILE_FLOOR 0
#define TILE_WALL 1

#define CA_DEFAULT		0
#define CA_LARGE_CAVERN 1
#define CA_TWISTY		2

 typedef struct 
 {
 	int r1_cutoff, r2_cutoff;
 	int reps;
 } generation_params; 

class CellularAutomata
{
public:

	CellularAutomata(int fillProbability, int r1_adjacent, int r2_adjacent ,int numRepititions, int width, int height);

	CellularAutomata(void);

	int SetType (int type);

	int Create();
	int Clean();
	int Connect();
    int getCell(const unsigned int x, const unsigned int y) const;

private:
	void CellularAutomata::initmap(void);
	void CellularAutomata::printmap(void);
	void CellularAutomata::printfunc(void);
	void CellularAutomata::generation(void);
	int CellularAutomata::randpick(void);
    void CellularAutomata::FloodFill(int x, int y);
    void setSize(const unsigned int x, const unsigned int y);

    GeneratorMap<int> grid;
    GeneratorMap<int> grid2;
 
	int fillprob;
	int r1_cutoff, r2_cutoff;
	int mSizeX, mSizeY;
	int reps;
	generation_params params;  

	int generations;

	bool use_2R;
	int cutof_2R;

	int current_rep;

};
