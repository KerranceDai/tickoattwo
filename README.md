# tickoattwo

A Win32 C++ implementation of the TickoaTTwo concept by Oats Jenkins. To view the project, open the solution in Visual Studio.

Original video: [I Made BETTER Tic-Tac-Toe](https://www.youtube.com/watch?v=ePxrVU4M9uA)

Rules:
1. Players take turns drawing lines in each of the 9 grid spaces.
    - Player 1 draws a <span style="color:#ed1c23">red</span> horizontal line.
    - Player 2 draws a <span style="color:#00a2e8">blue</span> vertical line.
    - A player can mark a tile that the other player has already marked to form a `+` shape.
2. A player cannot mark a space that the previous player had just marked.
3. The first player to form a row, column, or diagonal of 3 `+` spaces wins the game.