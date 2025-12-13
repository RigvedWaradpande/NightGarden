#ifndef ISTRATEGY_H
#define ISTRATEGY_H

#include <unordered_map>
#include <string>

struct Bar;
struct PortfolioState;
enum class Signal;

class IStrategy{
public:
    virtual ~IStrategy() = default;

    virtual void Initialise(const std::unordered_map<std::string, std::string>& params) = 0;
    virtual Signal OnBar(const Bar& bar, const PortfolioState& state) = 0;
    virtual void OnFinish() = 0;
};

#endif