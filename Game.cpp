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
		sLifespan();
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
	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	m_player = entity;
}

void Game::spawnEnemy()
{
	int enemyVerticle = getRandomInt(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
	sf::Color fillColor(getRandomInt(0, 255), getRandomInt(0, 255), getRandomInt(0, 255));
	sf::Color outlineColor(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB);
	Vec2 velocity(getRandomFloat(m_enemyConfig.SMIN, m_enemyConfig.SMAX), getRandomFloat(m_enemyConfig.SMIN, m_enemyConfig.SMAX));

	auto e = m_entities.addEntity("enemy");
	e->cTransform = std::make_shared<CTransform>(getRandomPositionInBorder(m_enemyConfig.CR), velocity, 0.0f);
	e->cShape = std::make_shared<CShape>(m_enemyConfig.SR, enemyVerticle, fillColor, outlineColor, m_enemyConfig.OT);
	e->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

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

	auto bullet = m_entities.addEntity("bullet");

	Vec2 dir = (target - entity->cTransform->pos).normalize();

	bullet->cTransform = std::make_shared<CTransform>(entity->cTransform->pos, dir * m_bulletConfig.S, 0.0f);

	bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
		m_bulletConfig.OT);

	bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

	bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
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
				e->cTransform->pos.x += e->cInput->inputAxis.normalize().x * e->cTransform->velocity.x;
				e->cTransform->pos.y += e->cInput->inputAxis.normalize().y * e->cTransform->velocity.y;
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

	for (auto& e : m_entities.getEntities())
	{
		if (e->cLifespan)
		{
			if (e->cLifespan->remaining > 0)
			{
				e->cLifespan->remaining--;
				int alpha = ((float)e->cLifespan->remaining / (float)e->cLifespan->total) * 255;
				auto colorFill = e->cShape->circle.getFillColor();
				auto colorOutline = e->cShape->circle.getOutlineColor();
				colorFill.a = alpha;
				colorOutline.a = alpha;
				e->cShape->circle.setFillColor(colorFill);
				e->cShape->circle.setOutlineColor(colorOutline);
			}
			else
			{
				e->destroy();
			}
		}
	}
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

	// Player border clamp
	auto playerPos = m_player->cTransform->pos;
	auto playerRadius = m_player->cCollision->radius;

	if (playerPos.x < playerRadius) { playerPos.x = playerRadius; }
	else if (playerPos.x + playerRadius > m_windowConfig.W) { playerPos.x = m_windowConfig.W - playerRadius; }
	if (playerPos.y < playerRadius) { playerPos.y = playerRadius; }
	else if (playerPos.y + playerRadius > m_windowConfig.H) { playerPos.y = m_windowConfig.H - playerRadius; }


	// Enemy border clamp
	for (auto& e : m_entities.getEntities("enemy"))
	{
		if (e->cTransform->pos.x < e->cCollision->radius)
		{
			e->cTransform->velocity.x *= -1.0f;
			e->cTransform->pos.x = e->cCollision->radius;
		}
		else if (e->cTransform->pos.x + e->cCollision->radius > m_windowConfig.W)
		{
			e->cTransform->velocity.x *= -1.0f;
			e->cTransform->pos.x = m_windowConfig.W - e->cCollision->radius;
		}

		if (e->cTransform->pos.y < e->cCollision->radius)
		{
			e->cTransform->velocity.y *= -1.0f;
			e->cTransform->pos.y = e->cCollision->radius;
		}
		else if (e->cTransform->pos.y + e->cCollision->radius > m_windowConfig.H)
		{
			e->cTransform->velocity.y *= -1.0f;
			e->cTransform->pos.y = m_windowConfig.H - e->cCollision->radius;
		}
	}

}

void Game::sEnemySpawner()
{
	// TODO: code which implements enemy spawning should go here
}

void Game::sGUI()
{
	ImGui::Begin("Geometry Wars");

	if (ImGui::BeginTabBar("Tabs"))
	{
		if (ImGui::BeginTabItem("Systems"))
		{
			ImGui::Text("Systems will be implemented here.");

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Entities"))
		{

			if (ImGui::CollapsingHeader("Mapped Entities"))
			{
				ImGui::Indent();
				for (auto const& dict : m_entities.getEntityMap())
				{
					if (ImGui::CollapsingHeader(dict.first.c_str()))
					{
						ImGui::Indent();
						drawEntityList(dict.second);
						ImGui::Unindent();
					}
				}
				ImGui::Unindent();
			}
			if (ImGui::CollapsingHeader("All Entities"))
			{
				ImGui::Indent();
				drawEntityList(m_entities.getEntities());
				ImGui::Unindent();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Game::drawEntityList(const EntityVec& entities)
{
	static std::string stringBuffer;

	for (auto& e : entities)
	{
		stringBuffer = "D##" + std::to_string(e->id());

		auto color = e->cShape->circle.getFillColor();

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f));

		if (ImGui::Button(stringBuffer.c_str()))
		{
			e->destroy();
		}

		ImGui::PopStyleColor(1);

		ImGui::SameLine();
		ImGui::Text(std::to_string(e->id()).c_str());
		ImGui::SameLine(70);

		ImGui::Text(e->tag().c_str());

		ImGui::SameLine(120);

		stringBuffer = "(" +
			std::to_string(static_cast<int>(e->cTransform->pos.x)) +
			", " +
			std::to_string(static_cast<int>(e->cTransform->pos.y)) +
			")";

		ImGui::Text(stringBuffer.c_str());
	}
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

		auto& input = m_player->cInput;
		input->inputAxis = { 0.0f, 0.0f };

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { input->inputAxis.y -= 1.0f; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { input->inputAxis.y += 1.0f; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { input->inputAxis.x -= 1.0f; }
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { input->inputAxis.x += 1.0f; }

		if (event.type == sf::Event::MouseButtonPressed)
		{
			// this line ignores mouse events if ImGui is the thing being clicked
			if (ImGui::GetIO().WantCaptureMouse)
			{
				continue;
			}

			if (event.mouseButton.button == sf::Mouse::Left)
			{
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
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
	int diff = 1 + max - min;
	int r = rand() % diff;
	r += min;
	return r;
}

float Game::getRandomFloat(float min, float max) const
{
	float random_0_to_1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	return min + random_0_to_1 * (max - min);
}

Vec2 Game::getRandomPositionInBorder(int radius) const
{
	int x = getRandomInt(radius, m_windowConfig.W - radius);
	int y = getRandomInt(radius, m_windowConfig.H - radius);
	Vec2 vec(x, y);
	return vec;
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
