#include "map.hpp"

using namespace std;

int Map::load(string pathArg)
{
	points.clear();
	string line;
	ifstream map(pathArg.c_str());
	if (map.is_open()) {
		path = pathArg;
		// An incredibly basic map layout parser
		while (!map.eof()) {
			getline(map, line);
			if (line[0] == '#') {
				// Ignore commented lines
				continue;
			} else if (line.find("PERSISTENCE=") == 0) {
				istringstream iss(line.substr(12));
				iss >> persistence;
			} else if (line.find("AMPLITUDE_INIT=") == 0) {
				istringstream iss(line.substr(15));
				iss >> amplitude_init;
			} else if (line.find("SHIFT=") == 0) {
				istringstream iss(line.substr(6));
				iss >> shift;
			} else if (line.find("WIDTH=") == 0) {
				istringstream iss(line.substr(6));
				iss >> width;
			} else if (line.find("HEIGHT=") == 0) {
				istringstream iss(line.substr(7));
				iss >> height;
			} else if (line.find("OCTAVES=") == 0) {
				istringstream iss(line.substr(8));
				iss >> octaves;
			} else if (line.find(",") != -1){
				int comma = line.find(",");
				sf::Vector2i pt;
				istringstream iss1(line.substr(0, comma));
				iss1 >> pt.x;
				istringstream iss2(line.substr(comma + 1));
				iss2 >> pt.y;
				points.push_back(pt);
			}
		}
	} else {
		cerr << "Unable to find map: " << path << endl;
		return EXIT_FAILURE;
	}
	map.close();
	return EXIT_SUCCESS;
}

