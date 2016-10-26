#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
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
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	// KICKERS

	// Left KICKER
	// Left Circle

	uint x_lc = 155;
	uint y_lc = 728;
	uint diameter = 15;

	b2BodyDef body_l;
	body_l.type = b2_staticBody;
	body_l.position.Set(PIXEL_TO_METERS(x_lc), PIXEL_TO_METERS(y_lc));

	b2Body* left_ball = world->CreateBody(&body_l);

	b2CircleShape shape_l;
	shape_l.m_radius = PIXEL_TO_METERS(diameter) * 0.5f;

	b2FixtureDef fixture_l;
	fixture_l.shape = &shape_l;
	left_ball->CreateFixture(&fixture_l);

	//Creating left kicker
	uint x_lk = 170;
	uint y_lk = 700;
	uint width_lk = 60;
	uint height_lk = 14;

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x_lk), PIXEL_TO_METERS(y_lk));
	body.angle = 30 * DEGTORAD;
	b2Body* b = world->CreateBody(&body);

	b2PolygonShape shape;
	shape.SetAsBox(PIXEL_TO_METERS(width_lk) * 0.5f, PIXEL_TO_METERS(height_lk) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.restitution = 0.1f;
	b->CreateFixture(&fixture);

	l_kicker = new PhysBody();
	l_kicker->body = b;
	b->SetUserData(l_kicker);
	l_kicker->width = width_lk * 0.5f;
	l_kicker->height = height_lk * 0.5f;

	// Set Left Joint bodies
	kicker_left_body = world->CreateBody(&body_l);

	// Set Left Joint

	b2RevoluteJointDef joint_left_def;
	joint_left_def.bodyA = App->physics->kicker_left_body;
	joint_left_def.bodyB = l_kicker->body;
	joint_left_def.collideConnected = true;
	joint_left_def.localAnchorA.Set(PIXEL_TO_METERS(0), 0);
	joint_left_def.localAnchorB.Set(PIXEL_TO_METERS(-40), PIXEL_TO_METERS(-2));
	joint_left_def.referenceAngle = 30 * DEGTORAD;
	joint_left_def.enableLimit = true;
	joint_left_def.lowerAngle = -42 * DEGTORAD;
	joint_left_def.upperAngle = 0 * DEGTORAD;
	joint_left_def.enableMotor = true;
	l_joint = (b2RevoluteJoint*)world->CreateJoint(&joint_left_def);


	// Right KICKER

	// Right Circle
	uint x_rc = 315;
	uint y_rc= 725;
	b2BodyDef body_r;
	body_r.type = b2_staticBody;
	body_r.position.Set(PIXEL_TO_METERS(x_rc), PIXEL_TO_METERS(y_rc));

	b2Body* right_ball = world->CreateBody(&body_r);

	b2CircleShape shape_r;
	shape_r.m_radius = PIXEL_TO_METERS(diameter) * 0.5f;

	b2FixtureDef fixture_r;
	fixture_r.shape = &shape_r;
	right_ball->CreateFixture(&fixture_r);

	//Creating right kicker
	uint x_rk = 240;
	uint y_rk = 730;
	uint width_rk = 60;
	uint height_rk = 14;

	b2BodyDef body_2;
	body_2.type = b2_dynamicBody;
	body_2.position.Set(PIXEL_TO_METERS(x_rk), PIXEL_TO_METERS(y_rk));
	body_2.angle = -30 * DEGTORAD;
	b2Body* b_2 = world->CreateBody(&body_2);

	b2PolygonShape shape_2;
	shape_2.SetAsBox(PIXEL_TO_METERS(width_rk) * 0.5f, PIXEL_TO_METERS(height_rk) * 0.5f);

	b2FixtureDef fixture_2;
	fixture_2.shape = &shape_2;
	fixture_2.density = 1.0f;
	fixture_2.restitution = 0.1f;
	b_2->CreateFixture(&fixture_2);

	r_kicker = new PhysBody();
	r_kicker->body = b_2;
	b_2->SetUserData(r_kicker);
	r_kicker->width = width_rk * 0.5f;
	r_kicker->height = height_rk * 0.5f;


	// Set Right Joint bodies
	kicker_right_body = world->CreateBody(&body_r);

	// Set Right Joint

	b2RevoluteJointDef joint_right_def;
	joint_right_def.bodyA = App->physics->kicker_right_body;
	joint_right_def.bodyB = r_kicker->body;
	joint_right_def.collideConnected = true;
	joint_right_def.localAnchorA.Set(PIXEL_TO_METERS(0), 0);
	joint_right_def.localAnchorB.Set(PIXEL_TO_METERS(40), PIXEL_TO_METERS(-2));
	joint_right_def.referenceAngle = -30 * DEGTORAD;
	joint_right_def.enableLimit = true;
	joint_right_def.lowerAngle = 0 * DEGTORAD;
	joint_right_def.upperAngle = 42 * DEGTORAD;
	joint_right_def.enableMotor = true;
	r_joint = (b2RevoluteJoint*)world->CreateJoint(&joint_right_def);

	spring_box = App->physics->CreateRectangle(438, 675, 12, 12);
	spring_box->body->SetType(b2_dynamicBody);

	spring_start = App->physics->CreateRectangle(438, 698, 12, 12);
	spring_start->body->SetType(b2_staticBody);


	b2DistanceJointDef spring_jointDef;
	spring_jointDef.bodyA = spring_start->body;
	spring_jointDef.bodyB = spring_box->body;
	spring_jointDef.collideConnected = false;
	spring_jointDef.length = PIXEL_TO_METERS(50);
	spring_jointDef.dampingRatio = 0.25f;
	spring_jointDef.frequencyHz = 25.0;

	spring_joint = (b2DistanceJoint*)world->CreateJoint(&spring_jointDef);

	
	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}


// 
update_status ModulePhysics::PostUpdate()
{

	left_rotation = RADTODEG * l_joint->GetJointAngle();
	right_rotation = RADTODEG * r_joint->GetJointAngle();

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		debug = !debug;
	}

	l_joint->SetMotorSpeed(1000 * DEGTORAD);
	r_joint->SetMotorSpeed(-1000 * DEGTORAD);

	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_REPEAT) {
		l_joint->EnableMotor(true);
		l_joint->SetMaxMotorTorque(720);
		l_joint->SetMotorSpeed(-22);
	}

	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_REPEAT) {
		r_joint->EnableMotor(true);
		r_joint->SetMaxMotorTorque(720);
		r_joint->SetMotorSpeed(22);
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
		spring_box->body->ApplyForce(b2Vec2(0, 1000), spring_box->body->GetWorldCenter(), false);

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

			// TODO 1: If mouse button 1 is pressed ...
			// App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN
			// test if the current body contains mouse position
		}
	}

	// If a body was selected we will attach a mouse joint to it
	// so we can pull it around
	// TODO 2: If a body was selected, create a mouse joint
	// using mouse_joint class property


	// TODO 3: If the player keeps pressing the mouse button, update
	// target position and draw a red line between both anchor points

	// TODO 4: If the player releases the mouse button, destroy the joint

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

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}