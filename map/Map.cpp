#include "Map.h"
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <fstream>
#include <sstream>

int Continent::getId() const
{
    return id;
}

Continent::Continent(const std::string &name, int id, int bonusArmyCount, const std::string &color)
{
    this->name = new std::string(name);
    this->id = id;
    this->bonusArmyCount = bonusArmyCount;
    this->color = new std::string(color);
}

Continent::Continent(const Continent &other)
{
    this->name = new std::string(*other.name);
    this->id = other.id;
    this->bonusArmyCount = other.bonusArmyCount;
    this->color = new std::string(*other.color);
}

Continent &Continent::operator=(const Continent &other)
{
    if (this != &other)
    {
        *name = *other.name;
        this->id = other.id;
        this->bonusArmyCount = other.bonusArmyCount;
        *color = *other.color;
    }
    return *this;
}

Continent::~Continent()
{
    delete name;
    delete color;
}

std::ostream &operator<<(std::ostream &out, const Continent &continent)
{
    out << "Continent: " << *continent.name << " (ID: " << continent.id << ", Bonus Armies: " << continent.bonusArmyCount << ", Color: " << *continent.color << ")";
    return out;
}

int Territory::getId() const
{
    return id;
}

Territory::Territory(const std::string &name, int x, int y, int id, int armyCount, Continent *continent)
{
    this->name = new std::string(name);
    this->x = x;
    this->y = y;
    this->id = id;
    this->adjacentTerritories = new std::vector<Territory *>();
    this->owner = nullptr;
    this->armyCount = armyCount;
    this->continent = continent;
}

Territory::Territory(const Territory &other)
{
    this->name = new std::string(*other.name);
    this->x = other.x;
    this->y = other.y;
    this->id = other.id;
    this->adjacentTerritories = new std::vector<Territory *>(*other.adjacentTerritories);
    this->owner = other.owner;
    this->armyCount = other.armyCount;
    this->continent = other.continent;
}

Territory &Territory::operator=(const Territory &other)
{
    if (this != &other)
    {
        *name = *other.name;
        this->x = other.x;
        this->y = other.y;
        this->id = other.id;
        *adjacentTerritories = *other.adjacentTerritories;
        this->owner = other.owner;
        this->armyCount = other.armyCount;
        this->continent = other.continent;
    }
    return *this;
}

Territory::~Territory()
{
    delete name;
    delete adjacentTerritories;
}

std::ostream &operator<<(std::ostream &out, const Territory &territory)
{
    out << "Territory: " << *territory.name << " (ID: " << territory.id << ", Coordinates: (" << territory.x << ", " << territory.y << "), Army Count: " << territory.armyCount << ")";
    return out;
}

Map::Map()
{
    Territories = new std::vector<Territory *>();
    Continents = new std::vector<Continent *>();
}

Map::Map(const Map &other)
{
    Territories = new std::vector<Territory *>();
    Continents = new std::vector<Continent *>();

    std::unordered_map<const Continent *, Continent *> continentCopies;
    for (const Continent *original : *other.Continents)
    {
        Continent *copy = new Continent(*original);
        Continents->push_back(copy);
        continentCopies.emplace(original, copy);
    }

    std::unordered_map<const Territory *, Territory *> territoryCopies;
    for (const Territory *original : *other.Territories)
    {
        Territory *copy = new Territory(*original);
        Territories->push_back(copy);
        territoryCopies.emplace(original, copy);
    }

    for (const Territory *original : *other.Territories)
    {
        Territory *copy = territoryCopies.at(original);
        copy->continent = original->continent == nullptr ? nullptr : continentCopies.at(original->continent);

        copy->adjacentTerritories->clear();
        for (const Territory *adjacent : *original->adjacentTerritories)
        {
            copy->adjacentTerritories->push_back(territoryCopies.at(adjacent));
        }
    }
}

Map &Map::operator=(const Map &other)
{
    if (this != &other)
    {
        Map temp(other);
        std::swap(Territories, temp.Territories);
        std::swap(Continents, temp.Continents);
    }
    return *this;
}

Map::~Map()
{
    for (Territory *territory : *Territories)
    {
        delete territory;
    }
    delete Territories;

    for (Continent *continent : *Continents)
    {
        delete continent;
    }
    delete Continents;
}

std::ostream &operator<<(std::ostream &out, const Map &map)
{
    out << "Map:\n";
    out << "Continents:\n";
    for (const Continent *continent : *map.Continents)
    {
        out << *continent << "\n";
    }
    out << "Territories:\n";
    for (const Territory *territory : *map.Territories)
    {
        out << *territory << "\n";
    }
    return out;
}

Continent *Map::CreateContinent(const std::string &name, int id, int bonusArmyCount, const std::string &color)
{
    if (bonusArmyCount <= 0)
    {
        std::cerr << "Continent bonus must be positive.\n";
        return nullptr;
    }

    for (const Continent *continent : *Continents)
    {
        if (continent->getId() == id)
        {
            std::cerr << "Continent with ID " << id << " already exists.\n";
            return nullptr;
        }
    }

    Continent *continent = new Continent(name, id, bonusArmyCount, color);
    Continents->push_back(continent);
    return continent;
}

Territory *Map::CreateTerritory(const std::string &name, int x, int y, int id, int armyCount, int continentId)
{
    for (const Territory *territory : *Territories)
    {
        if (territory->getId() == id)
        {
            std::cerr << "Territory with ID " << id << " already exists.\n";
            return nullptr;
        }
    }

    Continent *continent = nullptr;
    for (Continent *c : *Continents)
    {
        if (c->getId() == continentId)
        {
            continent = c;
            break;
        }
    }

    if (!continent)
    {
        std::cerr << "Continent with ID " << continentId << " does not exist.\n";
        return nullptr;
    }

    Territory *territory = new Territory(name, x, y, id, armyCount, continent);
    Territories->push_back(territory);
    return territory;
}

bool Map::CreateAdjacency(int territoryId1, int territoryId2)
{
    if (territoryId1 == territoryId2)
    {
        std::cerr << "Cannot create adjacency between the same territory.\n";
        return false;
    }

    Territory *territory1 = nullptr;
    Territory *territory2 = nullptr;

    for (Territory *territory : *Territories)
    {
        if (territory->getId() == territoryId1)
        {
            territory1 = territory;
        }
        else if (territory->getId() == territoryId2)
        {
            territory2 = territory;
        }
    }

    if (!territory1 || !territory2)
    {
        std::cerr << "One or both territories do not exist.\n";
        return false;
    }

    for (Territory *adjacent : *territory1->adjacentTerritories)
    {
        if (adjacent->getId() == territoryId2)
        {
            return true;
        }
    }

    territory1->adjacentTerritories->push_back(territory2);
    territory2->adjacentTerritories->push_back(territory1);
    return true;
}

MapLoader::MapLoader() = default;
MapLoader::MapLoader(const MapLoader &other) = default;
MapLoader &MapLoader::operator=(const MapLoader &other) = default;

std::ostream &operator<<(std::ostream &out, const MapLoader &)
{
    out << "MapLoader\n";
    return out;
}

bool MapLoader::loadMap(const std::string &path, Map &output, std::string &error) const
{
    std::ifstream input(path);
    if (!input)
    {
        error = "Cannot open map file: " + path;
        return false;
    }

    enum class Section
    {
        None,
        Files,
        Continents,
        Countries,
        Borders,
        Other
    };
    Section section = Section::None;
    std::string line;
    std::vector<std::string> ContinentsLines = {};
    std::vector<std::string> CountriesLines = {};
    std::vector<std::string> BordersLines = {};
    while (std::getline(input, line))
    {
        // Remove comments
        const std::size_t comment = line.find(';');
        if (comment != std::string::npos)
        {
            line.erase(comment);
        }

        // skip blank lines
        const std::size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
        {
            continue;
        }

        // Trim whitespace from both ends
        const std::size_t last = line.find_last_not_of(" \t\r\n");
        line = line.substr(first, last - first + 1);

        // find headers
        if (line.front() == '[' && line.back() == ']')
        {
            if (line == "[files]")
                section = Section::Files;
            else if (line == "[continents]")
                section = Section::Continents;
            else if (line == "[countries]")
                section = Section::Countries;
            else if (line == "[borders]")
                section = Section::Borders;
            else
                section = Section::Other;
            continue;
        }

        switch (section)
        {
        case Section::Continents:
            ContinentsLines.push_back(line);
            break;
        case Section::Countries:
            CountriesLines.push_back(line);
            break;
        case Section::Borders:
            BordersLines.push_back(line);
            break;
        case Section::None:
        case Section::Files:
        case Section::Other:
            break;
        }
    }

    if (input.bad())
    {
        error = "Error reading map file: " + path;
        return false;
    }

    Map temporaryMap;
    if (ContinentsLines.empty())
    {
        error = "Missing [continents] records";
        return false;
    }

    else if (CountriesLines.empty())
    {
        error = "Missing [countries] records";
        return false;
    }

    else if (BordersLines.empty())
    {
        error = "Missing [borders] records";
        return false;
    }

    for (std::size_t index = 0; index < ContinentsLines.size(); ++index)
    {
        std::istringstream fields(ContinentsLines[index]);
        std::string name;
        std::string color;
        std::string extra;
        int bonus = 0;
        if (!(fields >> name >> bonus >> color) || (fields >> extra) || bonus <= 0)
        {
            error = "Invalid continent record #" + std::to_string(index + 1);
            return false;
        }

        const int continentId = static_cast<int>(index + 1); // Domination uses 1-based continent indices.
        if (temporaryMap.CreateContinent(name, continentId, bonus, color) == nullptr)
        {
            error = "Could not create continent #" + std::to_string(index + 1);
            return false;
        }
    }

    std::unordered_set<int> countryIds;
    for (std::size_t index = 0; index < CountriesLines.size(); ++index)
    {
        std::istringstream fields(CountriesLines[index]);
        int id = 0;
        std::string name;
        int continentId = 0;
        int x = 0;
        int y = 0;
        std::string extra;
        const int armyCount = 0; // Army count is not specified in the map file; default to 0.
        if (!(fields >> id >> name >> continentId >> x >> y) || (fields >> extra) || id <= 0)
        {
            error = "Invalid country record #" + std::to_string(index + 1);
            return false;
        }

        if (temporaryMap.CreateTerritory(name, x, y, id, armyCount, continentId) == nullptr)
        {
            error = "Could not create country #" + std::to_string(index + 1);
            return false;
        }
        countryIds.insert(id);
    }

    for (std::size_t index = 0; index < BordersLines.size(); ++index)
    {
        std::istringstream fields(BordersLines[index]);
        int territoryId = 0;
        if (!(fields >> territoryId))
        {
            error = "Invalid border record #" + std::to_string(index + 1);
            return false;
        }
        if (countryIds.find(territoryId) == countryIds.end())
        {
            error = "Border record #" + std::to_string(index + 1) + " references unknown source territory " + std::to_string(territoryId);
            return false;
        }

        int adjacentId = 0;
        while (fields >> adjacentId)
        {
            if (!temporaryMap.CreateAdjacency(territoryId, adjacentId))
            {
                error = "Could not create adjacency between territory " + std::to_string(territoryId) + " and " + std::to_string(adjacentId);
                return false;
            }
        }
        if (!fields.eof())
        {
            error = "Invalid border record #" + std::to_string(index + 1);
            return false;
        }
    }

    output = temporaryMap;
    error.clear();
    return true;
}

bool Map::validate() const
{
    // Check if the map has at least one territory and one continent
    if (Territories->empty() || Continents->empty())
    {
        std::cerr << "Map must contain territories and continents.\n";
        return false;
    }

    // Check if each territory belongs to a continent in this map
    for (const Territory *territory : *Territories)
    {
        if (territory == nullptr)
        {
            std::cerr << "Invalid territory found in map.\n";
            return false;
        }

        bool belongsToThisMap = false;
        for (const Continent *continent : *Continents)
        {
            if (continent != nullptr && continent == territory->continent)
            {
                belongsToThisMap = true;
                break;
            }
        }
        if (!belongsToThisMap)
        {
            std::cerr << "Territory " << *territory->name << " does not belong to a continent in this map.\n";
            return false;
        }
    }

    // Check if each continent has at least one territory
    for (const Continent *continent : *Continents)
    {
        if (continent == nullptr)
        {
            std::cerr << "Invalid continent found in map.\n";
            return false;
        }

        bool hasTerritory = false;
        for (const Territory *territory : *Territories)
        {
            if (territory != nullptr && territory->continent == continent)
            {
                hasTerritory = true;
                break;
            }
        }
        if (!hasTerritory)
        {
            std::cerr << "Continent " << continent->getId() << " does not have any territories.\n";
            return false;
        }
    }
    
    // Check if the map is a connected graph
    const std::unordered_set<const Territory *> ownedTerritories(Territories->begin(), Territories->end());
    std::unordered_set<const Territory *> visited;
    std::vector<const Territory *> pending;
    pending.push_back(Territories->front());

    while (!pending.empty())
    {
        const Territory *current = pending.back();
        pending.pop_back();
        if (!visited.insert(current).second)
        {
            continue;
        }
        if (current->adjacentTerritories == nullptr)
        {
            std::cerr << "Territory has no adjacency list.\n";
            return false;
        }
        for (const Territory *neighbor : *current->adjacentTerritories)
        {
            if (ownedTerritories.find(neighbor) == ownedTerritories.end())
            {
                std::cerr << "Border points to a territory outside this map.\n";
                return false;
            }
            if (visited.find(neighbor) == visited.end())
            {
                pending.push_back(neighbor);
            }
        }
    }

    if (visited.size() != Territories->size())
    {
        std::cerr << "Map is not a connected graph.\n";
        return false;
    }

    // Check that each continent is connected using only its own territories.
    for (const Continent *continent : *Continents)
    {
        const Territory *start = nullptr;
        std::size_t memberCount = 0;
        for (const Territory *territory : *Territories)
        {
            if (territory->continent == continent)
            {
                ++memberCount;
                if (start == nullptr)
                {
                    start = territory;
                }
            }
        }

        std::unordered_set<const Territory *> continentVisited;
        std::vector<const Territory *> continentPending;
        continentPending.push_back(start); // The earlier membership check guarantees a member.
        while (!continentPending.empty())
        {
            const Territory *current = continentPending.back();
            continentPending.pop_back();
            if (!continentVisited.insert(current).second)
            {
                continue;
            }
            for (const Territory *neighbor : *current->adjacentTerritories)
            {
                if (neighbor->continent == continent && continentVisited.find(neighbor) == continentVisited.end())
                {
                    continentPending.push_back(neighbor);
                }
            }
        }

        if (continentVisited.size() != memberCount)
        {
            std::cerr << "Continent " << continent->getId() << " is not a connected subgraph.\n";
            return false;
        }
    }

    return true;
}
