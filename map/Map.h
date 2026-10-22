#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>
#include <iosfwd>

/** @brief A group of territories with a reinforcement bonus and display color. */
class Continent
{
private:
    std::string *name; // owns
    int id;
    int bonusArmyCount;
    std::string *color; // owns

public:
    /** @brief Create a continent, copying its name and color into owned strings. */
    Continent(const std::string &name, int id, int bonusArmyCount, const std::string &color);
    /** @brief Deep-copy the owned name and color. */
    Continent(const Continent &other);
    /** @brief Copy metadata without sharing the owned strings. */
    Continent &operator=(const Continent &other);
    /** @brief Release the owned name and color. */
    ~Continent();
    /** @brief Print the continent's name, ID, bonus, and color. */
    friend std::ostream &operator<<(std::ostream &out, const Continent &continent);
    /** @brief Return this continent's numeric ID. */
    int getId() const;
};

// Player is defined by the separate Player component; territories only borrow its pointer.
class Player;

/** @brief One map node, with army state, a continent, and adjacent territories. */
class Territory
{
private:
    std::string *name; // owns
    int x;
    int y;
    int id;
    std::vector<Territory *> *adjacentTerritories; // owns the vector, not its neighbor pointers
    Player *owner;                                 // borrows; never deletes the player
    int armyCount;
    Continent *continent; // borrows; Map owns the continent

public:
    /** @brief Create a territory; copy its name, start with no neighbors or owner, and borrow continent. */
    Territory(const std::string &name, int x, int y, int id, int armyCount, Continent *continent);
    /** @brief Copy owned storage; borrowed graph links are rebound when a whole Map is copied. */
    Territory(const Territory &other);
    /** @brief Copy state and owned storage while retaining borrowed-link semantics. */
    Territory &operator=(const Territory &other);
    /** @brief Delete the owned name and adjacency vector, not neighbors, player, or continent. */
    ~Territory();
    /** @brief Print the territory's name, ID, coordinates, and army count. */
    friend std::ostream &operator<<(std::ostream &out, const Territory &territory);
    // Map alone may connect territories and rebind links during a deep copy.
    friend class Map;
    /** @brief Return this territory's numeric ID. */
    int getId() const;
};

/** @brief An owned territory graph with undirected borders and continent membership. */
class Map
{
private:
    std::vector<Territory *> *Territories; // owns the vector and every territory in it
    std::vector<Continent *> *Continents;  // owns the vector and every continent in it

public:
    /** @brief Construct an empty graph with empty owned collections. */
    Map();
    /** @brief Deep-copy objects and rebind all continent and neighbor links to the new graph. */
    Map(const Map &other);
    /** @brief Replace this graph with a deep copy of another, including on self-assignment. */
    Map &operator=(const Map &other);
    /** @brief Delete owned territories, continents, and their collection vectors. */
    ~Map();
    /** @brief Print the stored continents and territories. */
    friend std::ostream &operator<<(std::ostream &out, const Map &map);
    /** @brief Add a continent with a unique ID and positive reinforcement bonus.
     *  @return A borrowed pointer owned by Map, or nullptr for a duplicate ID or nonpositive bonus.
     */
    Continent *CreateContinent(const std::string &name, int id, int bonusArmyCount, const std::string &color);
    /** @brief Add a territory assigned to a continent in this map.
     *  @param continentId ID of an existing continent in this map.
     *  @return A borrowed pointer owned by Map, or nullptr for a duplicate or unknown ID.
     */
    Territory *CreateTerritory(const std::string &name, int x, int y, int id, int armyCount, int continentId);
    /** @brief Add an undirected border; repeating either direction is a successful no-op.
     *  @return False for an unknown territory ID or a self-border; true otherwise.
     */
    bool CreateAdjacency(int territoryId1, int territoryId2);
    /** @brief Check membership, whole-map connectivity, and continent subgraph connectivity.
     *  @return True if all checks pass; otherwise prints the failing check to std::cerr.
     */
    bool validate() const;
};

/** @brief A stateless reader for Domination-format .map text files. */
class MapLoader
{
public:
    /** @brief Construct a loader with no stored state. */
    MapLoader();
    /** @brief Copy a stateless loader. */
    MapLoader(const MapLoader &other);
    /** @brief Assign a stateless loader. */
    MapLoader &operator=(const MapLoader &other);
    /** @brief Print a short loader description. */
    friend std::ostream &operator<<(std::ostream &out, const MapLoader &mapLoader);
    /** @brief Parse a file into output, preserving output when parsing fails.
     *  Graph connectivity is checked separately by Map::validate().
     *  @param path Path to the text file.
     *  @param output Map replaced only after structural parsing succeeds.
     *  @param error Failure explanation; empty after success.
     *  @return True when the file was parsed; false for missing or malformed data.
     */
    bool loadMap(const std::string& path, Map& output, std::string& error) const;
};
#endif // MAP_H