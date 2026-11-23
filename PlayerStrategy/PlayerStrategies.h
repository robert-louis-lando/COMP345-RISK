#ifndef PLAYER_STRATEGIES_H
#define PLAYER_STRATEGIES_H

#include <vector>

class Deck;
class Player;
class Territory;

// Polymorphic decision-making interface owned by a Player.
class PlayerStrategy
{
public:
    virtual ~PlayerStrategy() = default;

    // Let a strategy create orders for its player during the issue phase.
    virtual void issueOrder(Player *player, Deck *deck,
                            const std::vector<Player *> &players) = 0;
    // Return the strategy-specific attack and defense priorities.
    virtual std::vector<Territory *> toAttack(Player *player) = 0;
    virtual std::vector<Territory *> toDefend(Player *player) = 0;
    // Preserve the concrete strategy when a Player is copied.
    virtual PlayerStrategy *clone() const = 0;
};

// Uses the existing interactive order-issuing behavior.
class HumanPlayerStrategy : public PlayerStrategy
{
public:
    void issueOrder(Player *player, Deck *deck,
                    const std::vector<Player *> &players) override;
    std::vector<Territory *> toAttack(Player *player) override;
    std::vector<Territory *> toDefend(Player *player) override;
    PlayerStrategy *clone() const override;
};

// Reinforces the strongest territory and attacks from owned borders.
class AggressivePlayerStrategy : public PlayerStrategy
{
public:
    void issueOrder(Player *player, Deck *deck,
                    const std::vector<Player *> &players) override;
    std::vector<Territory *> toAttack(Player *player) override;
    std::vector<Territory *> toDefend(Player *player) override;
    PlayerStrategy *clone() const override;
};

// Reinforces the weakest territory without attacking.
class BenevolentPlayerStrategy : public PlayerStrategy
{
public:
    void issueOrder(Player *player, Deck *deck,
                    const std::vector<Player *> &players) override;
    std::vector<Territory *> toAttack(Player *player) override;
    std::vector<Territory *> toDefend(Player *player) override;
    PlayerStrategy *clone() const override;
};

// Remains inactive until the game changes its strategy after an attack.
class NeutralPlayerStrategy : public PlayerStrategy
{
public:
    void issueOrder(Player *player, Deck *deck,
                    const std::vector<Player *> &players) override;
    std::vector<Territory *> toAttack(Player *player) override;
    std::vector<Territory *> toDefend(Player *player) override;
    PlayerStrategy *clone() const override;
};

// Captures adjacent enemy territories automatically.
class CheaterPlayerStrategy : public PlayerStrategy
{
public:
    void issueOrder(Player *player, Deck *deck,
                    const std::vector<Player *> &players) override;
    std::vector<Territory *> toAttack(Player *player) override;
    std::vector<Territory *> toDefend(Player *player) override;
    PlayerStrategy *clone() const override;
};

#endif
