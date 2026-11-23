# Player Strategy Pattern

## Overview

The Player Strategy Pattern gives each `Player` its own decision-making behavior. `Player` delegates order issuance and territory prioritization to a polymorphic `PlayerStrategy`.

The implementation will use one strategy interface and five concrete strategies:

- `HumanPlayerStrategy`
- `AggressivePlayerStrategy`
- `BenevolentPlayerStrategy`
- `NeutralPlayerStrategy`
- `CheaterPlayerStrategy`

## Strategy Interface

```cpp
class PlayerStrategy
{
public:
    virtual ~PlayerStrategy();

    virtual void issueOrder(
        Player *player,
        Deck *deck,
        const std::vector<Player *> &players) = 0;

    virtual std::vector<Territory *> toAttack(Player *player) = 0;
    virtual std::vector<Territory *> toDefend(Player *player) = 0;
};
```

Each strategy receives the `Player` for which it is making decisions. The strategy does not replace the player's identity or owned territory collection.

## Integration with `Player`

`Player` stores a strategy pointer:

```cpp
PlayerStrategy *strategy;
```

It exposes a strategy setter and getters:

```cpp
void setStrategy(PlayerStrategy *newStrategy);
PlayerStrategy *getStrategy() const;
```

`Player` delegates to the current strategy:

```cpp
void Player::issueOrder(Deck *deck, std::vector<Player *> players)
{
    strategy->issueOrder(this, deck, players);
}

std::vector<Territory *> Player::toAttack() const
{
    return strategy->toAttack(const_cast<Player *>(this));
}

std::vector<Territory *> Player::toDefend() const
{
    return strategy->toDefend(const_cast<Player *>(this));
}
```

`Player` owns the strategy object and deletes it with the player's copy constructor, assignment operator, and destructor following the project's existing ownership conventions. Strategies should provide a `clone()` operation so copying a `Player` creates an independent strategy.

## Concrete Strategies

### `HumanPlayerStrategy`

Requires user interaction for every decision.

- `toDefend()` asks the player or, for compatibility, returns the owned territories.
- `toAttack()` asks the player or returns adjacent enemy territories.
- `issueOrder()` prompts for deployments, advances, and card usage.

This strategy replaces the existing human order loop rather than duplicating it inside `Player`.

### `AggressivePlayerStrategy`

Focuses on attack and reinforcement.

- `toDefend()` returns owned territories, ordered from strongest to weakest by army count.
- `toAttack()` returns adjacent enemy territories, ordered from weakest to strongest.
- `issueOrder()` deploys all reinforcements to the strongest owned territory.
- It then advances armies from a strong owned territory to an adjacent enemy territory.
- It repeats attacks until no owned source can legally advance or no adjacent enemy territory remains.

Attack-source army count may be used to leave at least no armies behind. Every generated order must be an existing `Deploy` or `Advance` order.

### `BenevolentPlayerStrategy`

Focuses on protecting weak territories.

- `toDefend()` returns owned territories, ordered from weakest to strongest by army count.
- `toAttack()` returns an empty list.
- `issueOrder()` deploys reinforcements to the weakest owned territory.
- It may issue `Advance` orders between owned territories to strengthen weak territories.
- It never issues an attack order against an enemy territory.

### `NeutralPlayerStrategy`

Does not issue orders.

- `toDefend()` returns an empty list.
- `toAttack()` returns an empty list.
- `issueOrder()` does nothing.
- If the neutral player is attacked, it becomes an `AggressivePlayerStrategy` on the next decision or immediately through `Player::setStrategy()`.

The attack-to-aggression transition should be triggered by the battle/conquest path and not by ordinary territory inspection.

### `CheaterPlayerStrategy`

Automatically conquers adjacent enemy territories.

- `toDefend()` returns all owned territories.
- `toAttack()` returns all adjacent enemy territories.
- `issueOrder()` conquers each adjacent enemy territory once during the player's turn.
- The player collects a territory through the normal game state (`removeTerritory` on the old owner and `addTerritory` on the cheater).
- The same territory is not processed again until the next turn.

The per-turn guard should use a conquered-this-turn flag or a strategy-local set reset at the beginning of each turn.

## Recommended File Layout

```text
PlayerStrategy/
    PlayerStrategies.h
    PlayerStrategies.cpp
```

The file pair contains:

- `PlayerStrategy`
- `HumanPlayerStrategy`
- `AggressivePlayerStrategy`
- `BenevolentPlayerStrategy`
- `NeutralPlayerStrategy`
- `CheaterPlayerStrategy`

## Neutral Transition

One possible integration point is in `Advance::execute()`:

```cpp
if (target->getOwner() == neutralPlayer)
{
    neutralPlayer->setStrategy(new AggressivePlayerStrategy());
}
```

The neutral player should change only after a valid attack against it, not during strategy selection or territory listing.
