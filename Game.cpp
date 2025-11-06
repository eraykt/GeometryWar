#include "Game.h"

#include <iostream>
#include <fstream>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& path)
{
	std::ifstream fin(path);
	std::string command;

	while (fin >> command)
	{
		if (command == "Window")
		{
			fin >> m_windowConfig.W >> m_windowConfig.H >> m_windowConfig.FL >> m_windowConfig.FS;
		}

		else if (command == "Font")
		{
			std::string fontDir;
			int fontSize;
			int c[3] = {};
			fin >> fontDir >> fontSize >> c[0] >> c[1] >> c[2];
			sf::Color color(c[0], c[1], c[2]);

			m_font.loadFromFile(fontDir);
			m_text.setFont(m_font);
			m_text.setCharacterSize(fontSize);
			m_text.setFillColor(color);
		}

		else if (command == "Player")
		{
			fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >>
				m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >>
				m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
		}

		else if (command == "Enemy")
		{
			fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >>
				m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >>
				m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
		}

		else if (command == "Bullet")
		{
			fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >>
				m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >>
				m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
		}
	}

	sf::Uint32 fullscreen = sf::Style::Default;

	if (m_windowConfig.FS)
	{
		fullscreen = sf::Style::Fullscreen;
	}

	m_window.create(sf::VideoMode(m_windowConfig.W, m_windowConfig.H), "Game Window", fullscreen);
	m_window.setFramerateLimit(m_windowConfig.FL);

	ImGui::SFML::Init(m_window);

	spawnPlayer();
}

void Game::run()
{
	// TODO:
	// - add pause functionality in here
	// - some systems should function while paused (rendering)
	// - some systems shouldn't (movement / input)
	while (m_running)
	{
		// update the entity manager
		m_entities.update();

		// required update call to imgui
		ImGui::SFML::Update(m_window, m_deltaClock.restart());

		sEnemySpawner();
		sMovement();
		sCollision();
		sUserInput();
		sGUI();
		sRender();

		// increment the current frame
		// may need to be moved when pause implemented
		m_currentFrame++;
	}
}

void Game::setPaused(bool paused)
{
	// TODO
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
	auto entity = m_entities.addEntity("player");

	entity->cTransform = std::make_shared<CTransform>(Vec2(m_windowConfig.W * 0.5f, m_windowConfig.H * 0.5f),
		Vec2(m_playerConfig.S, m_playerConfig.S), 0.0f);

	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
		m_playerConfig.OT);

	entity->cInput = std::make_shared<CInput>();

	m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy()
{
	// TODO:
	// make sure the enemy is spawned properly with the m_enemyConfig variables
	// the enemy must be spawned completely within the bounds of the window

	auto e = m_entities.addEntity("enemy");
	e->cTransform = std::make_shared<CTransform>(Vec2(300.0f, 300.0f), Vec2(1.0f, 1.0f), 0.0f);
	e->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color::Green, sf::Color::Black, 4.0f);
	std::cout << "spawne\n";
	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	// TODO: spawn small enemies at the location of the input enemy e

	// when we create the smaller enemy, we have to read the values of the original enemy
	// - spawn a number of small enemies equal to the vertices of the original enemy
	// - set each small enemy to the same color as the original, half the size
	// - small enemies are worth double points of the original enemy
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	// TODO: implement the spawning of a bullet which travels toward target
	// - bullet speed is given as a scalar speed
	// - you must set the velocity by using formula in notes
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	// TODO: implement your own special weapon
}

void Game::sMovement()
{
	for (auto e : m_entities.getEntities())
	{
		if (e->cTransform)
		{
			if (e->cInput)
			{
				if (e->cInput->up)
				{
					m_player->cTransform->pos.y -= m_player->cTransform->velocity.y;
				}
				if (e->cInput->down)
				{
					m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
				}
				if (e->cInput->left)
				{
					m_player->cTransform->pos.x -= m_player->cTransform->velocity.x;
				}
				if (e->cInput->right)
				{
					m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
				}
			}

			else
			{
				e->cTransform->pos += e->cTransform->velocity;
			}
		}
	}
}

void Game::sLifespan()
{
	// TODO: implement all lifespan functionality
	//
	// for all entities
	// - if entity has no lifespan component, skip it
	// - if entity has > 0 remaining lifespan, subtract 1
	// - if it has lifespan and is alive scale its alpha channel properly
	// - if it has lifespan and its time is up destroy the entity
}

void Game::sCollision()
{
	// TODO: implement all proper collisions between entities
	// be sure to use the collision radius, not the shape radius
	// sample
//    for (auto bullet: m_entities.getEntities("bullet")) {
//        for (auto m_entities: m_entities.getEntities()) {
//            //check collision
//        }
//        // we need another loop for small entities because small entities don't spawn further
//    }
}

void Game::sEnemySpawner()
{
	// TODO: code which implements enemy spawning should go here
}

void Game::sGUI()
{
	ImGui::Begin("Geometry Wars");

	ImGui::Text("Stuff Goes Here");

	ImGui::End();
}

void Game::sRender()
{
	// TODO: change the code below to draw ALL of the entities
	// sample drawing of the player Entity that we have created
	m_window.clear();

	for (auto& e : m_entities.getEntities())
	{
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);
		m_window.draw(e->cShape->circle);
	}

	// draw the ui last
	ImGui::SFML::Render(m_window);

	m_window.display();
}

void Game::sUserInput()
{
	// TODO: handle user input here
	// note that you should only be setting the player's input component variables here
	// you shold not implement the player's movement logic here
	// the movement system will read the variables you set in this functioin

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// pass the event to imgui to be parsed
		ImGui::SFML::ProcessEvent(m_window, event);

		// this event triggers when the window is closed
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = true;
				break;

			case sf::Keyboard::S:
				m_player->cInput->down = true;
				break;

			case sf::Keyboard::A:
				m_player->cInput->left = true;
				break;

			case sf::Keyboard::D:
				m_player->cInput->right = true;
				break;
			default:
				break;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;

			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;

			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;

			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;

			default:
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			// this line ignores mouse events if ImGui is the thing being clicked
			if (ImGui::GetIO().WantCaptureMouse)
			{
				continue;
			}

			if (event.mouseButton.button == sf::Mouse::Left)
			{
				std::cout << "Left Mouse Button Clicked at(" << event.mouseButton.x
					<< ", " << event.mouseButton.y << ")\n";
				// TODO: call spawnBullet here
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				std::cout << "Right Mouse Button Clicked at(" << event.mouseButton.x
					<< ", " << event.mouseButton.y << ")\n";
				// TODO: call special weapon here
			}
		}
	}
}


int Game::getRandomInt(int min, int max) const
{
	srand(time(NULL));
	int diff = 1 + max - min;
	int r = rand() % diff;
	r += min;
	return r;
}

// void collisions()
// {
// for (auto b : m_entities.getEntities("bullet"))
// for (auto e : m_entities.getEntities("enemy"))
//     if (Physics::CheckCollision(b, e))
//     {
//     b->destroy();
//     e->destroy();
//     }
// }
