// SFML includes
#include <SFML/Graphics.hpp>

// The defines that state the size of the grid and the speed of the game.
#define GRID_SIZE_X 50
#define GRID_SIZE_Y 50
#define SQUARE_SIZE 10
#define FRAMES_PER_SECOND 5
#define MOVE_CHANCE 5

// A GLOBAL!!! 
bool grid[GRID_SIZE_Y][GRID_SIZE_X] = { false };
bool gridTemp[GRID_SIZE_Y][GRID_SIZE_X] = { false };

/* Rule 1
 * Random Movement
 * Goes through every square that can move and randomly decides if it can
 */
void random_movement()
{
	// Go through every item in our grid
	for (int y = 0; y < GRID_SIZE_Y; y++)
	{
		for (int x = 0; x < GRID_SIZE_X; x++)
		{
			// Check if the item in the grid is valid
			if (grid[y][x])
			{
				// Generate a move roll and check its lower than MOVE_CHANCE
				int move = (rand() % 100) + 1;
				if (MOVE_CHANCE < move)
				{
					// Roll for direction
					int direction = rand() % 100;

					// Check each direction and that there isnt currently a node at the target position
					if (direction < 25 && y > 0 && !grid[y - 1][x])
					{
						// Set target to true
						grid[y - 1][x] = true;

						// Set current to false
						grid[y][x] = false;

						// Continue so we don't check other directions
						continue;
					}

					// Repeat above
					if (direction >= 25 && direction < 50 && y < (GRID_SIZE_Y - 1) && !grid[y + 1][x])
					{
						grid[y + 1][x] = true;
						grid[y][x] = false;
						continue;
					}
					if (direction >= 50 && direction < 75 && x > 0 && !grid[y][x - 1])
					{
						grid[y][x - 1] = true;
						grid[y][x] = false;
						continue;
					}
					if (direction >= 75 && x < (GRID_SIZE_X - 1) && !grid[y][x + 1])
					{
						grid[y][x + 1] = true;
						grid[y][x] = false;
					}
				}
			}
		}
	}
}

void updateGridArray()
{
	for (int y = 0; y < GRID_SIZE_Y; y++)
	{
		for (int x = 0; x < GRID_SIZE_X; x++)
		{
			grid[y][x] = gridTemp[y][x];
			gridTemp[y][x] = false;
		}
	}
}

/* Rule 2
 * Grow
 * Goes through every square, calculates the number of valid neighbours and either
 * generates a new item or kills and existing one
 */
void grow()
{
	// Go through every item in our grid
	for (int y = 0; y < GRID_SIZE_Y; y++)
	{
		for (int x = 0; x < GRID_SIZE_X; x++)
		{
			int number_of_neighbours = 0;

			// above
			if (y < GRID_SIZE_Y - 1 && grid[y + 1][x]) number_of_neighbours++;
			// top left
			if (y < GRID_SIZE_Y - 1 && x > 0 && grid[y + 1][x - 1]) number_of_neighbours++;
			// left
			if (x > 0 && grid[y][x - 1]) number_of_neighbours++;
			// bottom left
			if (x > 0 && y > 0 && grid[y - 1][x - 1]) number_of_neighbours++;
			// bottom
			if (y > 0 && grid[y - 1][x]) number_of_neighbours++;
			// bottom right
			if (x < GRID_SIZE_X - 1 && y > 0 && grid[y - 1][x + 1]) number_of_neighbours++;
			// right
			if (x < GRID_SIZE_X - 1 && grid[y][x + 1]) number_of_neighbours++;
			// top right
			if (x < GRID_SIZE_X - 1 && y < GRID_SIZE_Y - 1 && grid[y + 1][x + 1]) number_of_neighbours++;

			// For a space that is 'populated':
			if (grid[y][x])
			{
				gridTemp[y][x] = true;

				// Each cell with one or no neighbours dies, as if by solitude.
				// Each cell with four or more neighbours dies, as if by overpopulation.
				if (number_of_neighbours <= 1 || number_of_neighbours >= 4)
				{
					gridTemp[y][x] = false;
				}

				// Each cell with two or three neighbours survives.
				continue;
			}

			// For a space that is 'empty' or 'unpopulated'
			// Each cell with three neighbours becomes populated.
			if (number_of_neighbours == 3)
			{
				gridTemp[y][x] = true;
			}
		}
	}

	updateGridArray();
}

void run_rules()
{
	// Run the rules
	// random_movement();
	grow();
}

int main()
{
	// Seed the random number generator
	srand((unsigned)std::time(NULL));

	// Create the window and UI bar on the right
	const sf::Vector2f gridSize(GRID_SIZE_X*SQUARE_SIZE, GRID_SIZE_Y*SQUARE_SIZE);
	sf::RenderWindow window(sf::VideoMode((unsigned)(gridSize.x + 100.f), (unsigned)gridSize.y), "SFML_RuleBasedSystem", sf::Style::Close);
	sf::RectangleShape shape(sf::Vector2f(100.f, gridSize.y));
	shape.setPosition(sf::Vector2f(gridSize.x, 0.f));
	shape.setFillColor(sf::Color::Green);

	// Create the minibox textures
	sf::RectangleShape minibox(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
	sf::Texture minibox_texture;
	minibox_texture.loadFromFile("minibox_texture.png");
	minibox.setTexture(&minibox_texture);

	// Create the font
	sf::Font font;
	font.loadFromFile("DefaultAriel.ttf");

	// Create the start/stop text and position+rotate it
	sf::Text text;
	text.setFont(font);
	text.setString("START");
	text.setCharacterSize(48);
	text.setRotation(90);
	sf::Vector2f text_position(shape.getPosition().x + shape.getSize().x*0.75f, shape.getPosition().y + shape.getSize().y*0.35f);
	text.setPosition(text_position);

	// Flag for when the game plays
	bool event_playing = false;

	// Clock for timing
	sf::Clock clock;
	float elapsed = 0.0f;

	// While the window is open, update
	while (window.isOpen())
	{
		// Parse events
		sf::Event sf_event;
		while (window.pollEvent(sf_event)) {
			// Close the window when the close button is pressed
			if (sf_event.type == sf::Event::Closed) {
				window.close();
			}
			// If the left mouse button is pressed
			if (sf_event.type == sf::Event::MouseButtonPressed) {
				if (sf_event.mouseButton.button == sf::Mouse::Left) {
					// If the click a square, set it to valid or invalid (dependant on whether a valid square exists there)
					// Otherwise start the game
					if ((sf_event.mouseButton.x <= GRID_SIZE_X * SQUARE_SIZE) && !event_playing)
					{
						int x_on_grid = sf_event.mouseButton.x / SQUARE_SIZE;
						int y_on_grid = sf_event.mouseButton.y / SQUARE_SIZE;
						grid[y_on_grid][x_on_grid] = !grid[y_on_grid][x_on_grid];
					}
					else
					{
						event_playing = !event_playing;
						shape.setFillColor(event_playing ? sf::Color::Red : sf::Color::Green);
						text.setString(event_playing ? "STOP" : "START");
					}
				}
			}
		}

		// Update clock. If enough time has elapsed, perform an update
		elapsed += clock.restart().asSeconds();
		if (elapsed > (1.f / (float)FRAMES_PER_SECOND))
		{
			elapsed = 0.0f;
			if (event_playing)
			{
				run_rules();
			}
		}

		// Clear window, draw everything, display window
		window.clear();

		window.draw(shape);

		window.draw(text);

		for (int y = 0; y < GRID_SIZE_Y; y++)
		{
			for (int x = 0; x < GRID_SIZE_X; x++)
			{
				// Interestingly, we only draw inactive nodes (we have a texture for that)
				if (!grid[y][x])
				{
					minibox.setPosition(sf::Vector2f((float)(x*SQUARE_SIZE), (float)(y*SQUARE_SIZE)));
					window.draw(minibox);
				}
			}
		}

		window.display();
	}


	return 0;
}