#include "Orders.h"
#include "../GameEngine/GameEngine.h"

#include <iostream>
#include <string>
#include <vector>

// Print whether an order is valid immediately before its execution phase.
static bool reportValidation(const std::string &label, const Order *order)
{
    const bool valid = order != nullptr && order->validate();
    std::cout << label << " validation before execution: "
              << (valid ? "VALID" : "INVALID") << '\n';
    return valid;
}

// Run an integration demonstration through GameEngine::executeOrdersPhase.
int main()
{
    GameEngine &engine = GameEngine::getInstance();
    std::string error;
    if (!engine.loadMap("maps/solar/solar.map", error) || !engine.validateMap())
    {
        std::cerr << "Could not load the demonstration map: " << error << '\n';
        return 1;
    }

    engine.addPlayer("Red");
    engine.addPlayer("Blue");
    engine.gameStart();

    Player *red = nullptr;
    Player *blue = nullptr;
    for (Player *player : engine.getPlayers())
    {
        if (*player->getName() == "Red") red = player;
        if (*player->getName() == "Blue") blue = player;
    }
    const std::vector<Territory *> &territories = engine.getMap()->getTerritories();
    if (red == nullptr || blue == nullptr || territories.size() < 4)
    {
        std::cerr << "The game does not have enough objects for this demonstration.\n";
        return 1;
    }

    // Replace random initial ownership with a small repeatable border scenario.
    for (Player *player : engine.getPlayers())
    {
        for (Territory *territory : player->toDefend())
        {
            player->removeTerritory(territory);
            territory->setOwner(nullptr);
        }
    }
    Territory *redHome = territories[0];
    Territory *redBorder = territories[1];
    Territory *blueBorder = territories[2];
    Territory *blueHome = territories[3];
    red->addTerritory(redHome);
    red->addTerritory(redBorder);
    blue->addTerritory(blueBorder);
    blue->addTerritory(blueHome);
    engine.getMap()->CreateAdjacency(redHome->getId(), redBorder->getId());
    engine.getMap()->CreateAdjacency(redBorder->getId(), blueBorder->getId());
    engine.getMap()->CreateAdjacency(redBorder->getId(), blueHome->getId());
    red->setReinforcementPool(10);
    blue->setReinforcementPool(10);
    redHome->setArmyCount(10);
    redBorder->setArmyCount(1000);
    blueBorder->setArmyCount(20);
    blueHome->setArmyCount(20);

    std::cout << "\nPart 4 order execution integration demonstration\n";
    std::cout << "The game engine validates each queued order immediately before execution.\n\n";

    // Deploy, airlift, and bomb all run through the engine execution phase.
    Deploy deploy(red, redHome->getName(), 5);
    Airlift airlift(red, redHome->getName(), redBorder->getName(), 3);
    Bomb bomb(red, blueBorder->getName());
    reportValidation("Deploy", &deploy);
    reportValidation("Airlift", &airlift);
    reportValidation("Bomb", &bomb);
    red->issueOrder(new Deploy(red, redHome->getName(), 5));
    red->issueOrder(new Airlift(red, redHome->getName(), redBorder->getName(), 3));
    red->issueOrder(new Bomb(red, blueBorder->getName()));
    engine.executeOrdersPhase();
    const bool deployWorked = redHome->getArmyCount() == 12 && *red->getReinforcementPool() == 5;
    const bool airliftWorked = redBorder->getArmyCount() == 1003;
    const bool bombWorked = blueBorder->getArmyCount() == 10;
    std::cout << "Deploy/Airlift/Bomb effects applied: "
              << (deployWorked && airliftWorked && bombWorked ? "PASS" : "FAIL") << '\n';

    // Execute negotiation first in the round, then let the other player attempt an attack.
    const std::vector<Player *> &turnOrder = engine.getPlayers();
    Player *negotiator = turnOrder.front();
    Player *attacker = turnOrder.back();
    Player *negotiatedPlayer = negotiator == red ? blue : red;
    Territory *attackSource = attacker == red ? redBorder : blueBorder;
    Territory *attackTarget = attacker == red ? blueBorder : redBorder;
    attackSource->setArmyCount(20);
    const int sourceArmiesBefore = attackSource->getArmyCount();
    const int targetArmiesBefore = attackTarget->getArmyCount();
    Negotiate negotiate(negotiator, negotiatedPlayer);
    negotiator->negotiateWith(negotiatedPlayer);
    negotiatedPlayer->negotiateWith(negotiator);
    Advance attemptedAttack(attacker, attackSource->getName(), attackTarget->getName(), 5);
    reportValidation("Negotiate", &negotiate);
    const bool negotiationRejectedAttack = !reportValidation("Attack during negotiation", &attemptedAttack);
    negotiator->clearNegotiations();
    negotiatedPlayer->clearNegotiations();
    negotiator->issueOrder(new Negotiate(negotiator, negotiatedPlayer));
    attacker->issueOrder(new Advance(attacker, attackSource->getName(), attackTarget->getName(), 5));
    engine.executeOrdersPhase();
    const bool negotiationBlockedAttack =
        negotiationRejectedAttack &&
        attackSource->getArmyCount() == sourceArmiesBefore &&
        attackTarget->getArmyCount() == targetArmiesBefore &&
        attackTarget->getOwner() == negotiator;
    std::cout << "Negotiation check values: source " << sourceArmiesBefore << "->" << attackSource->getArmyCount()
              << ", target " << targetArmiesBefore << "->" << attackTarget->getArmyCount()
              << ", owner " << *attackTarget->getOwner()->getName() << '\n';
    std::cout << "Negotiation prevented the opposing attack: "
              << (negotiationBlockedAttack ? "PASS" : "FAIL") << '\n';

    // Two captures in one turn must transfer ownership and award exactly one card.
    red->clearNegotiations();
    blue->clearNegotiations();
    std::cout << "Next turn: temporary negotiation agreements have expired.\n";
    redBorder->setArmyCount(1000);
    blueBorder->setArmyCount(0);
    blueHome->setArmyCount(0);
    const int cardsBeforeConquests = red->getHandSize();
    Advance captureFirst(red, redBorder->getName(), blueBorder->getName(), 500);
    Advance captureSecond(red, redBorder->getName(), blueHome->getName(), 500);
    reportValidation("Advance to first enemy territory", &captureFirst);
    reportValidation("Advance to second enemy territory", &captureSecond);
    red->issueOrder(new Advance(red, redBorder->getName(), blueBorder->getName(), 500));
    red->issueOrder(new Advance(red, redBorder->getName(), blueHome->getName(), 500));
    engine.executeOrdersPhase();
    const bool ownershipTransferred = blueBorder->getOwner() == red && blueHome->getOwner() == red;
    const bool oneCardForTurn = red->getHandSize() == cardsBeforeConquests + 1;
    std::cout << "Advance transferred ownership after conquest: "
              << (ownershipTransferred ? "PASS" : "FAIL") << '\n';
    std::cout << "Two conquests awarded exactly one card: "
              << (oneCardForTurn ? "PASS" : "FAIL") << '\n';

    // Blockade doubles armies and transfers the territory to Neutral.
    redHome->setArmyCount(9);
    Blockade blockade(red, redHome->getName());
    reportValidation("Blockade", &blockade);
    red->issueOrder(new Blockade(red, redHome->getName()));
    engine.executeOrdersPhase();
    const bool blockadeTransferred = redHome->getOwner() != nullptr &&
        *redHome->getOwner()->getName() == "Neutral" && redHome->getArmyCount() == 18;
    std::cout << "Blockade doubled armies and transferred to Neutral: "
              << (blockadeTransferred ? "PASS" : "FAIL") << '\n';

    const bool allPassed = deployWorked && airliftWorked && bombWorked &&
        negotiationBlockedAttack && ownershipTransferred && oneCardForTurn && blockadeTransferred;
    std::cout << "\nOrder execution demonstration: " << (allPassed ? "PASS" : "FAIL") << '\n';
    return allPassed ? 0 : 1;
}
