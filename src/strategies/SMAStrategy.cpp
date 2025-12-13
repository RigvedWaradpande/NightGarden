#include <deque>
#include "src/include/IStrategy.h"
#include "src/include/types.h"

class SMAStrategy : public IStrategy{
private:
    //storage for data
    std::deque<double> prices;
    
    //avgs
    double prev_slow = 0.00, prev_fast = 0.00;

    //sizes of the two averages
    int fast_period, slow_period;
public:
    void Initialise(const std::unordered_map<std::string, std::string>& params) override{
        std::string temp;
        if(params.find("fast_period") != params.end()){
            fast_period = std::stoi(params.at("fast_period"));
        }else{
            fast_period = 10;
        }
        if(params.find("slow_period") != params.end()){
            slow_period = std::stoi(params.at("slow_period"));
        }else{
            slow_period = 50;
        }
    }
    Signal OnBar(const Bar& bar, const PortfolioState& state) override{
        if(prices.size() < slow_period){
            prices.push_back(bar.close);
            return Signal::HOLD;
        }else{
            prices.pop_front();
            prices.push_back(bar.close);
            double new_fast = CalculateSMA(fast_period);
            double new_slow = CalculateSMA(slow_period);

            bool crossover_up = (prev_fast <= prev_slow) && (new_fast > new_slow);
            bool crossover_down = (prev_fast >= prev_slow) && (new_fast < new_slow);

            prev_fast = new_fast;
            prev_slow = new_slow;
            if(crossover_up){
                //golden cross - short term momentum is bullish
                return Signal::BUY;
            }else if (crossover_down){
                //death cross - short term momentum is bearish
                return Signal::SELL;
            }else{
                return Signal::HOLD;
            }
        }
    }
    void OnFinish() override{
        
    }
    double CalculateSMA(int period){
        double sum = 0;
        for(int i = 1; i <= period; i++) {
            sum += prices[prices.size() - i];
        }
        return (double)sum/period;
    }
};

extern "C" {
    IStrategy* create_strategy() {
        return new SMAStrategy();
    }
    
    void destroy_strategy(IStrategy* strategy) {
        delete strategy;
    }
}