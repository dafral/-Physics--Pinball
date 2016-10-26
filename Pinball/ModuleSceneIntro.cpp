#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleWindow.h"
#include <cstdlib>

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	background = App->textures->Load("pinball/background_without_kickers.png");
	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	start_fx = App->audio->LoadFx("pinball/start.wav");
	iron_fx = App->audio->LoadFx("pinball/iron.wav");
	woob_fx = App->audio->LoadFx("pinball/woob.wav");
	air_fx = App->audio->LoadFx("pinball/air.wav");
	grounded_fx = App->audio->LoadFx("pinball/grounded.wav");

	
	/*spring_box = App->physics->CreateRectangle(438, 680, 10, 5);
	spring_box->body->SetType(b2_staticBody);
	spring_start = App->physics->CreateRectangle(438, 698, 10, 5);
	spring_start->body->SetType(b2_staticBody);

	b2RopeJointDef spring_jointDef;
	spring_jointDef.bodyA = spring_box;
	spring_jointDef.bodyB = spring_start;
	spring_jointDef.maxLength = 5;

	b2RopeJoint* spring_joint = (b2RopeJoint*)world->CreateJoint(&spring_jointDef);*/

	//sensors
	loosing_sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50);
	loosing_sensor->listener = this;

	air_sensor = App->physics->CreateChain(0, 0, air_s, 8);
	air_sensor->body->GetFixtureList()->SetSensor(true);
	air_sensor->body->SetType(b2_staticBody);
	air_sensor->listener = this;

	green_sensors.add(App->physics->CreateCircle(152, 97, 4));
	green_sensors.add(App->physics->CreateCircle(173, 74, 7));
	green_sensors.add(App->physics->CreateCircle(201, 61, 10));
	green_sensors.add(App->physics->CreateCircle(237, 49, 12));

	for (p2List_item<PhysBody*>* bc = green_sensors.getFirst(); bc != NULL; bc = bc->next) {
		bc->data->body->SetType(b2_staticBody);
		bc->data->body->GetFixtureList()->SetSensor(true);
		bc->data->listener = this;
	}

	carts.add(App->physics->CreateRectangle(70, 210, 29, 21));
	carts.add(App->physics->CreateRectangle(60, 190, 29, 21));
	carts.add(App->physics->CreateRectangle(50, 170, 29, 21));

	for (p2List_item<PhysBody*>* bc = carts.getFirst(); bc != NULL; bc = bc->next) {
		bc->data->body->SetType(b2_staticBody);
		bc->data->listener = this;
	}

	grounded_sensor = App->physics->CreateChain(0, 0, grounded_s, 6);
	grounded_sensor->body->GetFixtureList()->SetSensor(true);
	grounded_sensor->body->SetType(b2_staticBody);
	grounded_sensor->listener = this;

	map = App->physics->CreateChain(0, 0, map_points, 148);
	map->body->SetType(b2_staticBody);

	r_metal_piece = App->physics->CreateChain(0, 0, r_metal, 12);
	r_metal_piece->body->SetType(b2_staticBody);

	l_metal_piece = App->physics->CreateChain(0, 0, l_metal, 12);
	l_metal_piece->body->SetType(b2_staticBody);

	r_triangle = App->physics->CreateChain(0, 0, r_tr, 6);
	r_triangle->body->SetType(b2_staticBody);

	l_triangle = App->physics->CreateChain(0, 0, l_tr, 6);
	l_triangle->body->SetType(b2_staticBody);

	r_bouncy_t = App->physics->CreateChain(0, 0, r_bouncy_tr, 8);
	r_bouncy_t->body->SetType(b2_staticBody);
	r_bouncy_t->body->GetFixtureList()->SetRestitution(4.0f);
	r_bouncy_t->listener = this;

	l_bouncy_t = App->physics->CreateChain(0, 0, l_bouncy_tr, 8);
	l_bouncy_t->body->SetType(b2_staticBody);
	l_bouncy_t->body->GetFixtureList()->SetRestitution(4.0f);
	l_bouncy_t->listener = this;

	circles.add(App->physics->CreateCircle(269, 213, 27));
	circles.add(App->physics->CreateCircle(206, 286, 27));
	circles.add(App->physics->CreateCircle(322, 289, 27));

	for (p2List_item<PhysBody*>* bc = circles.getFirst(); bc != NULL; bc = bc->next) {
		bc->data->body->SetType(b2_staticBody);
		bc->data->body->GetFixtureList()->SetRestitution(1.3f);
		bc->data->listener = this;
	}

	//game ball
	ball = App->physics->CreateCircle(434, 651, 8);
	ball->body->SetFixedRotation(true);
	ball->body->GetFixtureList()->SetRestitution(0.3f);
	


	/*b2World* world;
	b2BodyDef spring_box;

	spring_box.type = b2_dynamicBody;
	spring_box.position.Set(PIXEL_TO_METERS(438), PIXEL_TO_METERS(680));

	b2Body* myBodyA = world->CreateBody(&spring_box);
	b2PolygonShape Spring_Box;
	Spring_Box.SetAsBox(PIXEL_TO_METERS(8), PIXEL_TO_METERS(5));

	b2FixtureDef spring_fixture;
	spring_fixture.shape = &Spring_Box;
	myBodyA->CreateFixture(&spring_fixture);

	b2BodyDef spring_start;

	spring_start.type = b2_staticBody;
	spring_start.position.Set(PIXEL_TO_METERS(438), PIXEL_TO_METERS(698));

	b2Body* myBodyB = world->CreateBody(&spring_start);
	b2PolygonShape Spring_Start;
	Spring_Start.SetAsBox(PIXEL_TO_METERS(8), PIXEL_TO_METERS(5));

	b2FixtureDef spring_st_fixture;
	spring_st_fixture.shape = &Spring_Start;
	myBodyB->CreateFixture(&spring_st_fixture);

	b2RopeJointDef spring_jointDef;
	spring_jointDef.bodyA = myBodyA;
	spring_jointDef.bodyB = myBodyB;
	spring_jointDef.maxLength = 5;

	b2RopeJoint* spring_joint = (b2RopeJoint*)world->CreateJoint(&spring_jointDef);*/
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{

	App->renderer->Blit(background, 0, 0, NULL, 0, 0);

	//shows score and balls remaining
	char title[150];
	char buffer[30];

	//score
	strcpy_s(title, "Score: ");
	_itoa_s(score, buffer, 10);
	strcat_s(title, buffer);

	//balls
	strcat_s(title, " Balls: ");
	_itoa_s(balls, buffer, 10);
	strcat_s(title, buffer);

	App->window->SetTitle(title);


	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		/*ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();*/
		
		ball->body->ApplyLinearImpulse(b2Vec2(0, -5), ball->body->GetWorldCenter(), false);
	}

	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		App->physics->stick_left_body->ApplyLinearImpulse(b2Vec2(0, -5), App->physics->stick_left_body->GetWorldCenter(), false);
	}

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		App->physics->stick_right_body->ApplyLinearImpulse(b2Vec2(0, -20), App->physics->stick_right_body->GetWorldCenter(), true);
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 8));
		//circles.getLast()->data->listener = this;
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50));
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64));
	}

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		//if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
			App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	if (bodyA == r_bouncy_t || bodyA == l_bouncy_t)
		App->audio->PlayFx(iron_fx);

	else if (bodyA == loosing_sensor && reproducing == false) {
		App->audio->PlayFx(start_fx);
		reproducing = true;
		balls--;
		

	}

	else if (bodyA == air_sensor) {
		App->audio->PlayFx(air_fx);
		score += 100;
	}

	else if (bodyA == grounded_sensor) {
		App->audio->PlayFx(grounded_fx);
		score += 100;
	}

	else {
		for (p2List_item<PhysBody*>* bc = green_sensors.getFirst(); bc != NULL; bc = bc->next) {
			if (bc->data == bodyA) {
				App->audio->PlayFx(woob_fx);
				score += 10;
			}
		}

		for (p2List_item<PhysBody*>* bc = circles.getFirst(); bc != NULL; bc = bc->next) {
			if (bc->data == bodyA) {
				App->audio->PlayFx(iron_fx);
				score += 10;
			}
		}

		for (p2List_item<PhysBody*>* bc = carts.getFirst(); bc != NULL; bc = bc->next) {
			if (bc->data == bodyA) {
				//bc->data->body->GetFixtureList()->SetSensor(true);
				score += 500;
			}
		}
		
	}

	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}
}
