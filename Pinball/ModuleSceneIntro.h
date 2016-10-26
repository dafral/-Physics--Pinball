#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	//balls available
	PhysBody* ball;
	uint balls = 3;

	uint score = 0;

	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;

	//map
	//bouncy circles
	p2List<PhysBody*> circles;
	p2List<PhysBody*> carts;

	PhysBody* map;
	PhysBody* r_metal_piece;
	PhysBody* l_metal_piece;
	PhysBody* r_triangle;
	PhysBody* l_triangle;
	PhysBody* r_bouncy_t; 
	PhysBody* l_bouncy_t; 
	

	//sensors
	p2List<PhysBody*> green_sensors;
	PhysBody* loosing_sensor;
	bool reproducing = false; //makes sure loosing sound triggers only one time

	PhysBody* air_sensor;
	PhysBody* grounded_sensor;

	bool sensed;

	//textures
	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	SDL_Texture* background;
	SDL_Texture* bouncer_kicked;
	SDL_Texture* left_kicker;
	SDL_Texture* right_kicker;

	//sounds
	uint bonus_fx;
	uint start_fx;
	uint woob_fx;
	uint iron_fx;
	uint air_fx;
	uint grounded_fx;

	p2Point<int> ray;
	bool ray_on;

private:
	//chains
	int map_points[148] = {
		428, 688,
		449, 688,
		449, 661,
		450, 606,
		451, 531,
		451, 451,
		450, 352,
		450, 245,
		450, 177,
		444, 148,
		421, 113,
		396, 82,
		362, 57,
		332, 41,
		299, 30,
		255, 27,
		214, 35,
		184, 48,
		154, 66,
		126, 92,
		114, 109,
		119, 132,
		126, 152,
		67, 178,
		55, 127,
		43, 94,
		23, 95,
		26, 140,
		30, 184,
		34, 222,
		41, 271,
		51, 319,
		65, 359,
		76, 389,
		94, 421,
		122, 459,
		80, 538,
		61, 575,
		59, 636,
		59, 715,
		169, 771,
		168, 783,
		147, 800,
		140, 911,
		351, 904,
		348, 806,
		293, 782,
		294, 768,
		401, 713,
		403, 553,
		380, 528,
		380, 448,
		420, 427,
		420, 321,
		389, 300,
		388, 268,
		421, 250,
		427, 181,
		421, 152,
		400, 117,
		366, 86,
		322, 58,
		305, 79,
		334, 103,
		354, 131,
		367, 155,
		372, 181,
		428, 185,
		431, 241,
		431, 307,
		431, 361,
		433, 440,
		431, 536,
		428, 682
	};

	int r_metal[12] = {
		307, 723,
		319, 725,
		384, 687,
		383, 603,
		375, 604,
		375, 684
	};

	int l_metal[12] = {
		148, 725,
		79, 687,
		79, 604,
		86, 603,
		86, 684,
		159, 723
	};

	int l_tr[6] = {
		109, 675,
		111, 612,
		149, 698
	};

	int l_bouncy_tr[8] = {
		119, 610,
		111, 612,
		149, 698,
		155, 692
	};

	int r_tr[6] = {
		356, 674,
		352, 612,
		313, 698
	};

	int r_bouncy_tr[8] = {
		346, 609,
		352, 612,
		313, 698,
		308, 692
	};

	int air_s[8] = {
		108, 465,
		118, 469,
		87, 529,
		79, 523
	};

	int grounded_s[6] = {
		415, 181,
		428, 218,
		417, 230
	};



};
