#ifndef PLAYER_STRATEGIES_H
#define PLAYER_STRATEGIES_H

#include <iosfwd>
#include <vector>

class Deck;
class Player;
class Territory;

/** Decision interface used by Player during the issuing-orders phase. */
class PlayerStrategy
{
public:
    virtual ~PlayerStrategy();
    virtual PlayerStrategy *clone() const = 0;
    virtual void issueOrder(Player &player, Deck *deck, const std::vector<Player *> &players) = 0;
    virtual std::vector<Territory *> toAttack(const Player &player) const = 0;
    virtual std::vector<Territory *> toDefend(const Player &player) const = 0;
    virtual const char *name() const = 0;
};

/** Human decisions are read from standard input. */
class HumanPlayerStrategy : public PlayerStrategy
{
public:
    PlayerStrategy *clone() const override;
    void issueOrder(Player &player, Deck *deck, const std::vector<Player *> &players) override;
    std::vector<Territory *> toAttack(const Player &player) const override;
    std::vector<Territory *> toDefend(const Player &player) const override;
    const char *name() const override;
};

/** Concentrates reinforcements and attacks from the strongest territory bordering an enemy. */
class AggressivePlayerStrategy : public PlayerStrategy
{
public:
    PlayerStrategy *clone() const override;
    void issueOrder(Player &player, Deck *deck, const std::vector<Player *> &players) override;
    std::vector<Territory *> toAttack(const Player &player) const override;
    std::vector<Territory *> toDefend(const Player &player) const override;
    const char *name() const override;
};

/** Reinforces weak territories and only moves armies between owned territories. */
class BenevolentPlayerStrategy : public PlayerStrategy
{
public:
    PlayerStrategy *clone() const override;
    void issueOrder(Player &player, Deck *deck, const std::vector<Player *> &players) override;
    std::vector<Territory *> toAttack(const Player &player) const override;
    std::vector<Territory *> toDefend(const Player &player) const override;
    const char *name() const override;
};

/** Does nothing unless attacked, when Player changes it to aggressive. */
class NeutralPlayerStrategy : public PlayerStrategy
{
public:
    PlayerStrategy *clone() const override;
    void issueOrder(Player &player, Deck *deck, const std::vector<Player *> &players) override;
    std::vector<Territory *> toAttack(const Player &player) const override;
    std::vector<Territory *> toDefend(const Player &player) const override;
    const char *name() const override;
};

/** Captures all currently adjacent enemy territories once during issuing. */
class CheaterPlayerStrategy : public PlayerStrategy
{
public:
    PlayerStrategy *clone() const override;
    void issueOrder(Player &player, Deck *deck, const std::vector<Player *> &players) override;
    std::vector<Territory *> toAttack(const Player &player) const override;
    std::vector<Territory *> toDefend(const Player &player) const override;
    const char *name() const override;
};

#endif
