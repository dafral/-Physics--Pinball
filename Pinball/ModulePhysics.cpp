#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "math.h"

#include "Box2D/Box2D/Box2D.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	b2BodyDef map;
	map.type = b2_staticBody;
	map.position.Set(0,0 );
	b2Body* map_borders = world->CreateBody(&map);

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
	b2Vec2 chain[74];
	int i = 0, j = 0;
	for (; i < 148; i++, j++) {
		chain[j].Set(PIXEL_TO_METERS(map_points[i]), PIXEL_TO_METERS(map_points[i + 1]));
		i++;
	}
	b2ChainShape borders;
	borders.CreateChain(chain,74);

	b2FixtureDef fixture;
	fixture.shape = &borders;
	map_borders->CreateFixture(&fixture);
	// Bouncer 1
	b2BodyDef bouncing_circle1;
	bouncing_circle1.type = b2_staticBody;
	bouncing_circle1.position.Set(PIXEL_TO_METERS(206), PIXEL_TO_METERS(286));
	b2Body* bouncing1 = world->CreateBody(&bouncing_circle1);
	b2CircleShape bouncer;
	bouncer.m_radius = PIXEL_TO_METERS(54)*0.5;
	b2FixtureDef fixturebouncer1;
	fixturebouncer1.shape = &bouncer;
	bouncing1->CreateFixture(&fixturebouncer1);
	bouncing1->GetFixtureList()->SetRestitution(1.3f);
	//Bouncer 2
	b2BodyDef bouncing_circle2;
	bouncing_circle2.type = b2_staticBody;
	bouncing_circle2.position.Set(PIXEL_TO_METERS(269), PIXEL_TO_METERS(213));
	b2Body* bouncing2 = world->CreateBody(&bouncing_circle2);
	b2CircleShape bouncer2;
	bouncer2.m_radius = PIXEL_TO_METERS(54)*0.5;
	b2FixtureDef fixturebouncer2;
	fixturebouncer2.shape = &bouncer2;
	bouncing2->CreateFixture(&fixturebouncer2);
	bouncing2->GetFixtureList()->SetRestitution(1.3f);
	//Bouncer 3
	b2BodyDef bouncing_circle3;
	bouncing_circle3.type = b2_staticBody;
	bouncing_circle3.position.Set(PIXEL_TO_METERS(322), PIXEL_TO_METERS(289));
	b2Body* bouncing3 = world->CreateBody(&bouncing_circle3);
	b2CircleShape bouncer3;
	bouncer3.m_radius = PIXEL_TO_METERS(54)*0.5;
	b2FixtureDef fixturebouncer3;
	fixturebouncer3.shape = &bouncer3;
	bouncing3->CreateFixture(&fixturebouncer3);
	bouncing3->GetFixtureList()->SetRestitution(1.3f);
	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	// On space bar press, create a circle on mouse position
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		b2BodyDef body;
		body.type = b2_dynamicBody;
		float radius = PIXEL_TO_METERS(8);
		body.position.Set(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()));

		b2Body* b = world->CreateBody(&body);

		b2CircleShape shape;
		shape.m_radius = radius;
		b2FixtureDef fixture;
		fixture.shape = &shape;

		b->CreateFixture(&fixture);
		b->GetFixtureList()->SetRestitution(0.3f);
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		// TODO 1: When pressing 2, create a box on the mouse position

		// TODO 2: To have the box behave normally, set fixture's density to 1.0f
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// TODO 3: Create a chain shape using those vertices
		// remember to convert them from pixels to meters!
		/*
		int points[24] = {
			-38, 80,
			-44, -54,
			-16, -60,
			-16, -17,
			19, -19,
			19, -79,
			61, -77,
			57, 73,
			17, 78,
			20, 16,
			-25, 13,
			-9, 72
		};
		*/
	}

	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}
		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}
