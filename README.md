# 🐍 Snake Game

A classic Snake Game developed using **C++** and **SFML (Simple and Fast Multimedia Library)**.\
Eat food, avoid obstacles, and don’t crash into yourself. The game gets more challenging as you progress!

---

##  Features

- Classic snake movement and food collection
- Randomly placed **obstacles**
- Automatically updating **high score list** (top 5)
- **Game Over** screen with score summary
- Smooth visuals using SFML sprites and fonts

---

## 🛠  Technologies Used

- **C++**
- **SFML** - for graphics and rendering
- **Standard Library** for file handling and score saving

---


### Prerequisites

- C++ compiler (e.g., g++)
- [SFML library](https://www.sfml-dev.org/download.php)

### How to Compile

```bash
g++ main.cpp Game.cpp Snake.cpp -o snake -lsfml-graphics -lsfml-window -lsfml-system
