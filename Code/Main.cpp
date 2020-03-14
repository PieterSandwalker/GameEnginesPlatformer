#include <SFML\Graphics\RenderWindow.hpp>
#include "GameObject.hpp"
#include "StaticPlatform.hpp"
#include "MovingPlatform.hpp"
#include "Path.hpp"
#include "Character.hpp"
#include "CharacterMovement.hpp"
#include "Collider.hpp"
#include "DeathBoundary.hpp"
#include "SpawnPoint.hpp"
#include "SideBoundary.hpp"
#include "Scroller.hpp"

#include <zmq.hpp>
#include <string>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)    Sleep(n)
#endif
#include <thread>
#include <mutex>
#include <fstream>
#include <iostream>
#include <duktape.c>
#include <duk_config.h>
#include <dukglue/dukglue.h>


#include "Gametime.hpp"
#include "Localtime.hpp"

using namespace std;
using namespace sf;

vector<GameObject*> scene;
vector<Vector2f> locations;
Gametime mainTime(1000);
int objects_en_scene;

/*
 * Uses the load script function from Noah's example
 */
static void load_script_from_file(duk_context* ctx, const char* filename)
{
	std::ifstream t(filename);
	std::stringstream buffer;
	buffer << t.rdbuf();
	duk_push_lstring(ctx, buffer.str().c_str(), (duk_size_t)(buffer.str().length()));
}

struct CharacterUpdate {

	int charID;
	sf::Vector2f position;

} typedef CharacterUpdate;


void sceneOne(vector<GameObject*>* scene) {

	//section 1
	scene->push_back(new StaticPlatform(Vector2f(100, 50), Vector2f(150, 450), "Textures/SciFi.png"));
	scene->push_back(new MovingPlatform(Vector2f(100, 50), Vector2f(350, 450), Vector2f(550, 300), "Textures/SciFi.png"));
	
	//section 2
	scene->push_back(new MovingPlatform(Vector2f(100, 50), Vector2f(950, 300), Vector2f(950, 450), "Textures/SciFi.png"));
	scene->push_back(new StaticPlatform(Vector2f(100, 50), Vector2f(1200, 450), "Textures/SciFi.png"));	
	scene->push_back(new StaticPlatform(Vector2f(100, 50), Vector2f(1400, 350), "Textures/SciFi.png"));
	
	//walls
	scene->push_back(new StaticPlatform(Vector2f(50, 600), Vector2f(0, 0), "Textures/SciFi2.jpg"));
	scene->push_back(new StaticPlatform(Vector2f(100, 300), Vector2f(750, 300), "Textures/SciFi3.jpg"));
	scene->push_back(new StaticPlatform(Vector2f(50, 600), Vector2f(1550, 0), "Textures/SciFi2.jpg"));
	
	//external forces
	scene->push_back(new DeathBoundary(Vector2f(4000,50), Vector2f(0, 650)));
	scene->push_back(new SpawnPoint(Vector2f(25, 50), Vector2f(175, 400)));
	scene->push_back(new SpawnPoint(Vector2f(25, 50), Vector2f(810, 250)));

}

void req(zmq::socket_t* socket, mutex* mutex, condition_variable* condition_variable) {

	CharacterUpdate message = { 0, sf::Vector2f(100.f,100.f) };

	while (1) {

		//  Wait for next request from client
		zmq_recv(*socket, &message, sizeof(CharacterUpdate), 0);

		if (message.charID == 0) {
			std::cout << "Connection requested" << std::endl;

			message.charID = objects_en_scene;

			std::cout << "Connection approved for player " << message.charID << std::endl;

			unique_lock<std::mutex> cv_lock(*mutex);

			objects_en_scene++;

			locations.push_back(message.position);
			scene.push_back(new Character(Vector2f(25, 50), Vector2f(0, 0), "Textures/SciFi2.jpg"));

			cv_lock.unlock();
			condition_variable->notify_all();

			

		}
		else if (message.charID > 0) {

			std::unique_lock<std::mutex> cv_lock(*mutex);

			locations[message.charID] = message.position;

			message.charID = objects_en_scene;

			cv_lock.unlock();
			condition_variable->notify_all();
		}

		zmq_send(*socket, &message, sizeof(CharacterUpdate), 0);

	}

}

void pub(zmq::socket_t* socket, mutex* mutex, condition_variable* condition_variable) {

	// Game loop

	// Create window
	sf::RenderWindow window(sf::VideoMode(800, 600), "My window", sf::Style::Default);

	Localtime loopIterator(&mainTime);

	// Create a heap and initial context
	duk_context* ctx = NULL;

	ctx = duk_create_heap_default();
	if (!ctx) {
		printf("Failed to create a Duktape heap.\n");
		exit(1);
	}

	while (1)
	{
		// Server - Update moving object locations
		// Client - Recieve locations and Check collisions

		//recieve locations

		for (int i = 0; i < scene.size(); i++) {
			
			int j = scene[i]->hasComponent("Path");
			
			if (j >= 0) {
				Path* path = (Path*)scene[i]->getComponent(j);
				
				load_script_from_file(ctx, "Scripts/pathUpdate.js");
				if (duk_peval(ctx) != 0) {
					printf("Error: %s\n", duk_safe_to_string(ctx, -1));
					duk_destroy_heap(ctx);
					exit(1);
				}
				duk_pop(ctx);

				duk_push_global_object(ctx);
				duk_get_prop_string(ctx, -1, "pathUpdate");

				dukglue_push(ctx, path);

				dukglue_register_method(ctx, &Path::update, "update");

				if (duk_pcall(ctx, 1) != 0)
					printf("Error: %s\n", duk_safe_to_string(ctx, -1));

				duk_pop(ctx);

				
				locations[i] = scene[i]->getShape()->getPosition();
			}

		}

		zmq_send(*socket, locations.data(), sizeof(Vector2f) * objects_en_scene, 0);
		
		window.clear(sf::Color::Black);

		for (int i = 0; i < scene.size(); i++) {
			scene[i]->getShape()->setPosition(locations[i]);
			
			if (scene[i]->isRendered()) {
				window.draw(*scene[i]->getShape());
			}
		}

		window.display();

		int x = max(33 - loopIterator.getTime(), 0);

		sleep(x);

		loopIterator.restart();

	}

	duk_destroy_heap(ctx);

}

int main()
{

	// Client & Server - Load scene
	sceneOne(&scene);

	objects_en_scene += scene.size();

	for (int i = 0; i < scene.size(); i++) {
		locations.push_back(scene[i]->getShape()->getPosition());
	}

	//  Prepare our context and socket
	zmq::context_t context1(1);
	zmq::socket_t socket1(context1, ZMQ_REP);

	socket1.bind("tcp://*:5555");

	zmq::context_t context2(1);
	zmq::socket_t socket2(context2, ZMQ_PUB);

	socket2.bind("tcp://*:5556");

	mutex m;
	condition_variable cv;
	thread req_thread(req, &socket1, &m, &cv);
	thread pub_thread(pub, &socket2, &m, &cv);

	req_thread.join();
	pub_thread.join();

	return 0;
}


