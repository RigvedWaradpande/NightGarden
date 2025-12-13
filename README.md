## **1. Night Garden Moving Parts**
```
┌─────────────────────────────────────────────────────────┐
│                    TradingEngine                        │
│  - Owns strategies (unique_ptr)                         │
│  - Owns portfolio                                       │
│  - Manages backtest loop                                │
│  - Processes orders                                     │
└─────────────────────────────────────────────────────────┘
         │                                  │
         │ owns (composition)               │ owns
         │                                  │
         ▼                                  ▼
┌──────────────────────┐          ┌─────────────────────┐
│   Strategy (ABC)     │          │     Portfolio       │
│  - engine* (observe) │          │  - orders           │
│  + onBar()           │          │  - cash             │
│  + onOrder()         │          │  + addOrder()       │
└──────────────────────┘          │  + getTotalValue()  │
         △                        └─────────────────────┘
         │ inherits                        │
         │                                 │ owns
    ┌────┴────┐                            │
    │         │                            ▼
┌──────-─┐ ┌──────────┐          ┌─────────────────┐
│Momentum│ │MeanRev   │          │     Order       │
│Strategy│ │Strategy  │          │  - price        │
└──────-─┘ └──────────┘          │  - quantity     │
                                 │  - status       │
                                 └─────────────────┘


┌─────────────────────────────────────────────────────────┐
│                    Data Layer                           │
│                                                         │
│  Bar (struct)          MarketData                       │
│  - open                - historical bars                │
│  - high                - current index                  │
│  - low                                                  │
│  - close                                                │
└─────────────────────────────────────────────────────────┘
```
## **2. High Level System Diagram**

```
┌─────────────────────────────────────────────────────────────┐
│                         UI LAYER                            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │ Chart Panel  │  │ Control Panel│  │  Log Panel   │       │
│  │ (ImGui Plot) │  │  (Buttons)   │  │  (Status)    │       │
│  └──────┬───────┘  └──────-┬──────┘  └────-─-─┬─────┘       │
│         │                  │                  │             │
│         └──────────────────┼──────────────────┘             │
│                            │                                │
│                     Commands (Write)                        │
│                     Data (Read-Only)                        │
└────────────────────────────┼────────────────────────────────┘
                             │
┌────────────────────────────┼────────────────────────────────┐
│                      ENGINE LAYER                           │
│                            │                                │
│  ┌─────────────────────────▼──────────────────────────┐     │
│  │           BacktestEngine (Orchestrator)            │     │
│  │  - Loads historical data                           │     │
│  │  - Drives event loop                               │     │
│  │  - Coordinates strategy + execution                │     │
│  └─────┬────────────────────────────┬─────────────────┘     │
│        │                            │                       │
│        │                            │                       │
│   ┌────▼────────┐            ┌──────▼──────────┐            │
│   │  Strategy   │            │  Core Domain    │            │
│   │   Loader    │            │                 │            │
│   │             │            │  ┌──────────┐   │            │
│   │ - dlopen    │            │  │Portfolio │   │            │
│   │ - dlsym     │            │  │Manager   │   │            │
│   │ - validate  │            │  └────┬─────┘   │            │
│   └─────────────┘            │       │         │            │
│                              │  ┌────▼─────┐   │            │
│                              │  │Execution │   │            │
│                              │  │ Engine   │   │            │
│                              │  └──────────┘   │            │
│                              └─────────────────┘            │
└─────────────────────────────────────────────────────────────┘
                             │
┌────────────────────────────┼────────────────────────────────┐
│                    PLUGIN LAYER                             │
│                            │                                │
│  ┌─────────────────────────▼─────────────────────────┐      │
│  │         IStrategy Interface (.so files)           │      │
│  │                                                   │      │
│  │  virtual void onBar(const Bar& bar) = 0;          │      │
│  │  virtual void init(StrategyContext& ctx) = 0;     │      │
│  │                                                   │      │
│  └─────────────────────────────────────────────────┬─┘      │
│                                                    │        │
│         ┌────────────────┬──────────────┬──────────┘        │
│         │                │              │                   │
│  ┌──────▼─────┐  ┌───────▼──────┐ ┌─────▼────────┐          │
│  │SMA Strategy│  │Mean Reversion│ │Black-Scholes │          │
│  │ (.so)      │  │   (.so)      │ │   (.so)      │          │
│  └────────────┘  └──────────────┘ └──────────────┘          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## 3. KEY INTERACTION SEQUENCES

### **Sequence 1: Loading & Running a Strategy**
```
User               UI              Engine           StrategyLoader      Strategy (.so)
  │                 │                 │                    │                  │
  │ Click "Load"    │                 │                    │                  │
  ├────────────────>│                 │                    │                  │
  │                 │ loadStrategy()  │                    │                  │
  │                 ├────────────────>│                    │                  │
  │                 │                 │ dlopen(path)       │                  │
  │                 │                 ├───────────────────>│                  │
  │                 │                 │                    │ Load library     │
  │                 │                 │                    ├─────────────────>│
  │                 │                 │                    │<─────────────────┤
  │                 │                 │ dlsym("create")    │                  │
  │                 │                 ├───────────────────>│                  │
  │                 │                 │<───────────────────┤                  │
  │                 │                 │ createStrategy()   │                  │
  │                 │                 ├────────────────────┼─────────────────>│
  │                 │                 │                    │     new Strategy │
  │                 │                 │<───────────────────┼──────────────────┤
  │                 │                 │ init(context)      │                  │
  │                 │                 ├────────────────────┼─────────────────>│
  │                 │<────────────────┤                    │                  │
  │                 │                 │                    │                  │
  │ Click "Run"     │                 │                    │                  │
  ├────────────────>│                 │                    │                  │
  │                 │ run()           │                    │                  │
  │                 ├────────────────>│                    │                  │
  │                 │                 │ [EVENT LOOP]       │                  │
  │                 │                 │ for each bar:      │                  │
  │                 │                 │   onBar(bar)       │                  │
  │                 │                 ├────────────────────┼─────────────────>│
  │                 │                 │                    │  (strategy logic)│
  │                 │                 │<───────────────────┼──────────────────┤
  │                 │<────────────────┤                    │                  │
  │                 │ Display results │                    │                  │
  │<────────────────┤                 │                    │                  │
```

### **Sequence 2: Strategy Submits Order**
```
Strategy          Context         Portfolio      ExecutionEngine
   │                 │                 │                 │
   │ submitOrder()   │                 │                 │
   ├────────────────>│                 │                 │
   │                 │ createOrder()   │                 │
   │                 ├────────────────>│                 │
   │                 │                 │ executeOrder()  │
   │                 │                 ├────────────────>│
   │                 │                 │                 │ Calculate slippage
   │                 │                 │                 │ Check liquidity
   │                 │                 │                 │ Set fill price
   │                 │                 │<────────────────┤
   │                 │                 │ updatePosition()│
   │                 │                 │                 │
   │                 │<────────────────┤                 │
   │<────────────────┤                 │                 │
```

### **Sequence 3: Hot-Reload Mechanism**
```
User               UI              Engine           StrategyLoader
  │                 │                 │                    │
  │ Click "Reload"  │                 │                    │
  ├────────────────>│                 │                    │
  │                 │ unloadStrategy()│                    │
  │                 ├────────────────>│                    │
  │                 │                 │ shutdown()         │
  │                 │                 │ [call on strategy] │
  │                 │                 │ destroyStrategy()  │
  │                 │                 │ [call on strategy] │
  │                 │                 │ dlclose(handle)    │
  │                 │                 ├───────────────────>│
  │                 │<────────────────┤                    │
  │                 │                 │                    │
  │                 │ loadStrategy()  │                    │
  │                 ├────────────────>│                    │
  │                 │                 │ [Same as Seq 1]    │
  │                 │<────────────────┤                    │
  │ "Strategy reloaded successfully"  │                    │
  │<────────────────┤                 │                    │
```

## 4. DATA FLOW DIAGRAM
```
┌──────────────┐
│ Historical   │
│ Data (CSV)   │
└──────┬───────┘
       │
       ▼
┌──────────────────────────────────────┐
│      BacktestEngine.run()            │
│                                      │
│  for each Bar in historicalData:     │
│    1. strategy.onBar(bar)            │◄──── Strategy makes decisions
│    2. Process pending orders         │
│    3. Update portfolio positions     │
│    4. Calculate equity               │──────► Store in equityCurve[]
│    5. Record state                   │
└──────────────┬───────────────────────┘
               │
               ▼
┌──────────────────────────────────────┐
│         UI Rendering                 │
│                                      │
│  - Plot equityCurve                  │
│  - Display current positions         │
│  - Show order history                │
│  - Log strategy messages             │
└──────────────────────────────────────┘
```

## 5. FILE STRUCTURE
```
NightGarden/
├── CMakeLists.txt
├── src/
│   ├── main.cpp                    # Application entry
│   ├── core/
│   │   ├── Portfolio.h/cpp         # Portfolio management
│   │   ├── Order.h/cpp             # Order types
│   │   ├── Bar.h                   # Market data
│   │   └── ExecutionEngine.h/cpp  # Order execution simulation
│   ├── engine/
│   │   ├── BacktestEngine.h/cpp    # Main orchestrator
│   │   └── StrategyLoader.h/cpp    # dlopen/dlclose logic
│   ├── strategy/
│   │   └── IStrategy.h             # Strategy interface
│   ├── ui/
│   │   └── Application.h/cpp       # ImGui application
│   └── utils/
│       └── Logger.h/cpp            # Logging utility
├── strategies/
│   ├── sma_crossover/
│   │   ├── CMakeLists.txt
│   │   └── SMAStrategy.cpp         # Builds to libsma.so
│   ├── mean_reversion/
│   │   └── ...
│   └── black_scholes/
│       └── BlackScholesStrategy.cpp
├── data/
│   └── historical_data.csv
└── external/
    ├── imgui/
    └── glfw/
```