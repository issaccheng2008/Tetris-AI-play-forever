# Tetris-AI-play-forever

PLEASE RUN THIS CODE ON VISUAL STUDIO.
WHEN RUNED ON OTHER PLATFORMS, I CAN NOT GUARANTEE IT WILL WORK

I created a Tetris AI that never dies. it's written in C++
the AI is actually two programms, one is used to find all the illegal states, and another is to find the correct move.
if you want to learn more, I made a video expaining it all.[I tech](https://www.youtube.com/watch?v=UBbdVaojjMM)
the file'input.txt' is the file containing all the 'illegal' states for a tetris game.
the file 'tetrominoes.txt' is the file containing all the information about each tetrominoes.
the file 'play_tetris.cpp' is the program that automatically plays tetris on [Jstris](https://jstris.jezevec10.com/)
to see the AI play tetris yourself, first you will have to know:

the coordinate of the the top-left corner of the place where they show the coming blocks(789,324)

![piece](https://github.com/issaccheng2008/Tetris-AI-play-forever/blob/main/piece.png)

the width of each grid(31)

![width](https://github.com/issaccheng2008/Tetris-AI-play-forever/blob/main/width.png)

the bottom_left corner of the board(430,896)

![width](https://github.com/issaccheng2008/Tetris-AI-play-forever/blob/main/board.png)

the y_cordinate of the top of the board(291)

![width](https://github.com/issaccheng2008/Tetris-AI-play-forever/blob/main/y_coordinate.png)

and put them into px,py,w,bx,by,top_y

my program probably has some bugs. plus the fact that Jstris wasn't built for such high speed, it may take a few tries to finish the 1000 line speedrun. if you decrease the speed at which the program places blocks, then you can finish it almost every time.

if you want to run the program on some other platform besides Jstris, then you will probabily need to write your own recognition function.

if you have any questions or suggestions, then please comment.
