```

---

## **Night Garden Moving Parts**

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
│  + onBar()          │          │  - cash             │
│  + onOrder()        │          │  + addOrder()       │
└──────────────────────┘          │  + getTotalValue()  │
         △                        └─────────────────────┘
         │ inherits                        │
         │                                 │ owns
    ┌────┴────┐                           │
    │         │                           ▼
┌───────┐ ┌──────────┐          ┌─────────────────┐
│Momentum│ │MeanRev   │          │     Order       │
│Strategy│ │Strategy  │          │  - price        │
└───────┘ └──────────┘          │  - quantity     │
                                 │  - status       │
                                 └─────────────────┘


┌─────────────────────────────────────────────────────────┐
│                    Data Layer                           │
│                                                          │
│  Bar (struct)          MarketData                       │
│  - open                - historical bars                │
│  - high                - current index                  │
│  - low                                                  │
│  - close                                                │
└─────────────────────────────────────────────────────────┘
```