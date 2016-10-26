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

	
	//art
	background = App->textures->Load("Game/pinball/background_without_kickers.png");
	circle = App->textures->Load("Game/pinball/wheel.png");
	box = App->textures->Load("Game/pinball/crate.png");
	rick = App->textures->Load("Game/pinball/rick_head.png");
	left_kicker = App->textures->Load("Game/pinball/Left_kicker.png");
	right_kicker = App->textures->Load("Game/pinball/Right_kicker.png");
	bouncer_kicked = App->textures->Load("Game/pinball/bouncer_kicked.png");
	Ball = App->textures->Load("Game/pinball/Ball.png");

	//fx
	bonus_fx = App->audio->LoadFx("Game/pinball/bonus.wav");
	start_fx = App->audio->LoadFx("Game/pinball/start.wav");
	iron_fx = App->audio->LoadFx("Game/pinball/iron.wav");
	woob_fx = App->audio->LoadFx("Game/pinball/woob.wav");
	air_fx = App->audio->LoadFx("Game/pinball/air.wav");
	grounded_fx = App->audio->LoadFx("Game/pinball/grounded.wav");

	
	//sensors
	loosing_sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, 865, SCREEN_WIDTH, 50);
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
		//bc->data->body->SetActive(false);
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
	r_bouncy_t->body->GetFixtureList()->SetRestitution(2.0f);
	r_bouncy_t->listener = this;

	l_bouncy_t = App->physics->CreateChain(0, 0, l_bouncy_tr, 8);
	l_bouncy_t->body->SetType(b2_staticBody);
	l_bouncy_t->body->GetFixtureList()->SetRestitution(2.0f);
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
	ball = App->physics->CreateCircle(434, 600, 8);
	ball->body->SetFixedRotation(true);
	ball->body->GetFixtureList()->SetRestitution(0.3f);
	
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

	//crates collision destruction
	if (must_destroy != nullptr) {
		int x, y;
		must_destroy->GetPosition(x, y);
		App->renderer->Blit(broken_box, x, y, NULL, 1.0f, NULL);
		must_destroy->body->SetActive(false);
		must_destroy = nullptr;
	}

	//new ball generated if you have enough balls
	if (lost == true) {
		ball->body->SetActive(false);
		lost = false;

		balls--;

		if (balls > 0) {
			ball = App->physics->CreateCircle(434, 630, 8);
			ball->body->SetFixedRotation(true);
			ball->body->GetFixtureList()->SetRestitution(0.2f);
		}
	}




	
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 8));
		//circles.getLast()->data->listener = this;
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

	int x, y;
	App->physics->r_kicker->GetPosition(x, y);
	App->renderer->Blit(right_kicker, x + 10, y - 15, NULL, 1.0f, App->physics->r_kicker->GetRotation() + 35);

	int i, j;
	ball->GetPosition(i, j);
	App->renderer->Blit(Ball, i, j, NULL, 1.0f, ball->GetRotation());

	int d, r;
	App->physics->l_kicker->GetPosition(d, r);
	App->renderer->Blit(left_kicker, d - 15, r - 25, NULL, 1.0f, App->physics->l_kicker->GetRotation() - 35);

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

	else if (bodyA == loosing_sensor) {
		App->audio->PlayFx(start_fx);
		lost = true;

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
				int x, y;
				bc->data->GetPosition(x, y);
				App->renderer->Blit(bouncer_kicked, x, y, NULL, 1.0f, NULL);				
				score += 10;
			}
		}

		for (p2List_item<PhysBody*>* bc = carts.getFirst(); bc != NULL; bc = bc->next) {
			if (bc->data == bodyA) {
				must_destroy = bc->data;
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
